/*
 *  This file is part of Rabbits
 *  Copyright (C) 2015  Clement Deschamps and Luc Michel
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef _QEMU_CPU_H
#define _QEMU_CPU_H

#include <map>
#include <systemc>

#include <rabbits/arith.h>
#include <rabbits/config/manager.h>

#include "qemu/master.h"

template <unsigned int BUSWIDTH = 32>
class QemuCpu : public QemuMaster<BUSWIDTH>, public qemu_io_callbacks {
protected:
    using QemuMaster<BUSWIDTH>::m_lib;
    using QemuMaster<BUSWIDTH>::m_qdev;
    using QemuMaster<BUSWIDTH>::m_obj;
    using QemuMaster<BUSWIDTH>::m_params;

public:
    using QemuMaster<BUSWIDTH>::p_bus;

protected:
    int m_cpuid;

    void mainloop_thread() {
        bool qemu_want_quit = false;
        int64_t elapsed;

        for(;;) {
            qemu_want_quit = m_lib.cpus_loop(&elapsed);

            if(qemu_want_quit) {
                sc_core::sc_stop();
            }

            sc_core::wait(elapsed, sc_core::SC_NS);
        }
    }

    void map_as_io(AddressRange region) {
        Parameters &g = Component::get_config().get_global_params();
        if (!(g["report-non-mapped-access"].as<bool>())) {
            /* Mapping I/O only if non-mapped access report is disabled.
             * Otherwise, the whole address space is already mapped. */
            MLOG(APP, DBG) << "Mapping region " << region << " in QEMU as I/O\n";
            m_lib.map_io(region.begin(), region.size());
        }
    }

    void map_as_dmi(AddressRange region, void *ptr, bool readonly) {
        MLOG(APP, DBG) << "Mapping region " << region << " in QEMU as DMI (ro:" << readonly << ")\n";
        m_lib.map_dmi(region.begin(), region.size(), ptr, readonly);
    }

    bool subcut_dmi_region(AddressRange region, const DmiInfo &info)
    {
        /* Ensure dmi region is included into region. Otherwise, the DMI info
         * are buggy and we should report it. */
        if (info.range.begin() < region.begin() || info.range.end() > region.end()) {
            MLOG(APP, ERR) << "Buggy DMI info for address range " << region << ". Falling back to I/O\n";
            map_as_io(region);
            return false;
        }

        AddressRange before_dmi(region.begin(), info.range.begin() - region.begin());
        AddressRange after_dmi(info.range.end() + 1, region.size() - before_dmi.size() - info.range.size());

        if (before_dmi.size()) {
            map_as_io(before_dmi);
        }

        if (after_dmi.size()) {
            map_as_io(after_dmi);
        }

        return true;
    }

    void declare_dmi_region(AddressRange region, const DmiInfo &info)
    {
        bool readonly = !info.write_allowed;

        /* Validate the DMI info before giving it to QEMU. Fallback to IO
         * region if not enough requirements are met. */
        if (!info.read_allowed) {
            map_as_io(region);
            return;
        }

        if (info.range != region) {
            /* We should sub-cut the region in this case */
            if (!subcut_dmi_region(region, info)) {
                return;
            }
        }

        map_as_dmi(info.range, info.ptr, readonly);
    }

    void declare_memory_region(AddressRange region)
    {
        DmiInfo info;

        if (p_bus.dmi_probe(region, info)) {
            declare_dmi_region(region, info);
        } else {
            map_as_io(region);
        }
    }

    void declare_memory_regions() {
        const std::vector<AddressRange> &mem_map = p_bus.get_memory_mapping();

        for (AddressRange r : mem_map) {
            if (QemuInstance::get(Component::get_config()).self_mapping_contains(r)) {
                continue;
            }

            declare_memory_region(r);
        }
    }

public:
    SC_HAS_PROCESS(QemuCpu);

    QemuCpu(sc_core::sc_module_name name, const Parameters &params, ConfigManager &c, const std::string type_name)
        : QemuMaster<BUSWIDTH>(name, params, c)
    {
        QemuInstance &inst = QemuInstance::get(Component::get_config());

        m_obj = m_lib.object_new(type_name.c_str());

        m_cpuid = inst.get_next_cpuid();
        m_qdev = m_lib.cpu_get_qdev(m_cpuid);

        if (!m_cpuid) {
            SC_THREAD(mainloop_thread);
            m_lib.register_io_callback(*this);

            Component::get_config().add_param_alias("gdb-server", m_params["gdb-server"]);
            std::string gdb_port = m_params["gdb-server"].template as<std::string>();

            if (gdb_port != "") {
                MLOG(APP, INF) << "Starting gdb server on port " << gdb_port << "\n";
                m_lib.start_gdb_server("tcp::" + gdb_port);
            }
        } else {
            m_params["gdb-server"].set_advanced();
        }
    }

    virtual ~QemuCpu() {}

    uint32_t qemu_io_read(uint32_t addr, uint32_t size) {
        uint32_t buf;

        assert(size <= sizeof(buf));
        p_bus.bus_read(addr, (uint8_t*)&buf, size);

        return buf;
    }

    void qemu_io_write(uint32_t addr, uint32_t val, uint32_t size) {
        assert(size <= sizeof(val));
        p_bus.bus_write(addr, (uint8_t*)&val, size);
    }

    virtual void end_of_elaboration() {
        QemuMaster<BUSWIDTH>::end_of_elaboration();

        if (!m_cpuid) {
            declare_memory_regions();
        }
    }

};
#endif
