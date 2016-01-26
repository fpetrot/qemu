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

#ifndef _QEMU_WRAPPER_H
#define _QEMU_WRAPPER_H

#include <vector>
#include <cassert>

#include "libsc_qemu/libsc_qemu.h"

#include <rabbits/component/master.h>
#include <rabbits/logger.h>

class QemuMaster: public Master, public qemu_io_callbacks
{
protected:
    LibScQemu &m_lib;

    virtual uint32_t qemu_io_read(uint32_t addr, uint32_t size) {
        uint32_t buf;

        assert(size <= sizeof(buf));
        bus_read(addr, (uint8_t*)&buf, size);

        return buf;
    }

    virtual void qemu_io_write(uint32_t addr, uint32_t val, uint32_t size) {
        assert(size <= sizeof(val));
        bus_write(addr, (uint8_t*)&val, size);
    }

    void mainloop_thread() {
        bool qemu_want_quit = false;
        int64_t elapsed;

        for(;;) {
            qemu_want_quit = m_lib.cpus_loop(&elapsed);

            if(qemu_want_quit) {
                sc_core::sc_stop();
            }

            wait(elapsed, sc_core::SC_NS);
        }
    }

    void add_map(uint32_t base_address, uint32_t size) {
        m_lib.map_io(base_address, size);
    }

    void add_map_dmi(uint32_t base_address, uint32_t size, void *data) {
        m_lib.map_dmi(base_address, size, data);
    }

    virtual void dmi_hint_cb(uint64_t start, uint64_t size, void *data,
                             sc_core::sc_time read_latency, sc_core::sc_time write_latency)
    {
        if (data != NULL) {
            DBG_PRINTF("DMI mapping from %" PRIx64 " to %" PRIx64 "\n", start, start+size);
            add_map_dmi(start, size, data);
        } else {
            DBG_PRINTF("I/O mapping from %" PRIx64 " to %" PRIx64 "\n", start, start+size);
            add_map(start, size);
        }
    }

public:
    SC_HAS_PROCESS (QemuMaster);
    QemuMaster(sc_core::sc_module_name name, ComponentParameters &params, LibScQemu &lib)
        : Master(name, params)
        , m_lib(lib)
    {
        SC_THREAD(mainloop_thread);
    }

    virtual ~QemuMaster() {}
};

#endif
