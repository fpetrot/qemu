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

#ifndef _RABBITS_QEMU_INSTANCE_H
#define _RABBITS_QEMU_INSTANCE_H

#include <vector>
#include <string>
#include <cassert>

#include <rabbits/component/parameters.h>

#include "libsc_qemu/libsc_qemu.h"

class QemuInstance {
public:
    struct CpuType {
        enum value {
            NONE = 0,
            ARM,
            CPU_TYPE_COUNT
        };

        static const std::string LIB[CPU_TYPE_COUNT];
    };

protected:
    static QemuInstance *m_inst;

    LibScQemu  m_lib;
    int m_next_cpuid;

    /* Contains regions mapped by QEMU devices.
     * These regions must be ignored when passing the address map to QEMU. */
    std::vector<AddressRange> m_self_mapping;

    struct DiscoveryInfo {
        CpuType::value cpu_type;
        int cpu_count;
        std::string cpu_model;
        int insn_limit;
        int mips_shift;

        DiscoveryInfo()
            : cpu_type(CpuType::NONE)
            , cpu_count(0)
            , insn_limit(0)
            , mips_shift(0) {}
    } m_discovery;

    void add_cpu(CpuType::value, const std::string model);

    void lib_init();

    QemuInstance();

public:

    LibScQemu &get_lib() {
        if (!m_lib.is_inited()) {
            lib_init();
        }

        return m_lib;
    }

    void cpu_discover(CpuType::value architecture,
                      const std::string &model,
                      ComponentParameters &params);

    int get_next_cpuid() {
        assert(m_next_cpuid < m_discovery.cpu_count);
        return m_next_cpuid++;
    }

    int get_num_cpu() {
        return m_discovery.cpu_count;
    }

    void self_mapping_add(const AddressRange &r) {
        m_self_mapping.push_back(r);
    }

    bool self_mapping_contains(const AddressRange &r) {
        for (AddressRange r2 : m_self_mapping) {
            if (r == r2) {
                return true;
            }
        }
        return false;
    }

    static QemuInstance& get() {
        if (m_inst == NULL) {
            m_inst = new QemuInstance;
        }
        return *m_inst;
    }
};

#endif
