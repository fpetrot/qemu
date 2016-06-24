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

#include "instance.h"
#include <rabbits/logger.h>

using std::string;

QemuInstance *QemuInstance::m_inst = NULL;

const string QemuInstance::CpuType::LIB[] = {
    [CpuType::NONE] = "",
    [CpuType::ARM] = "libqemu-system-arm",
};

QemuInstance::QemuInstance()
    : m_next_cpuid(0)
{
}

void QemuInstance::add_cpu(CpuType::value type, const string model)
{
    if (m_discovery.cpu_type == CpuType::NONE) {
        m_discovery.cpu_type = type;
        m_discovery.cpu_model = model;
    } else if ((m_discovery.cpu_type != type) || (m_discovery.cpu_model != model)) {
        ERR_STREAM("Non-SMP systems not supported.\n");
        abort();
    }

    m_discovery.cpu_count++;
}

void QemuInstance::cpu_discover(CpuType::value architecture,
                                const string &model,
                                ComponentParameters &params)
{
    int insn_limit = params["insn-limit"].as<int>();
    int mips_shift = params["mips-shift"].as<int>();

    add_cpu(architecture, model);

    if (insn_limit > 0) {
        DBG_STREAM("Setting instruction execution limit to " << insn_limit << "\n");
        m_discovery.insn_limit = insn_limit;
    }

    if (mips_shift > 0) {
        DBG_STREAM("Setting MIPS shift to " << mips_shift << "\n");
        m_discovery.mips_shift = mips_shift;
    }
}

void QemuInstance::lib_init()
{
    if (m_discovery.cpu_type == CpuType::NONE) {
        ERR_STREAM("No CPU in platform description. Unable to use QEMU\n");
        abort();
    }

    const string lib_name = CpuType::LIB[m_discovery.cpu_type];

    DBG_STREAM("Initializing sc-qemu with "
               << m_discovery.cpu_count << " " << m_discovery.cpu_model
               << " cpu(s)\n");

    //m_lib.register_io_callback(m_master);
    m_lib.set_insn_limit(m_discovery.insn_limit);
    m_lib.set_mips_shift(m_discovery.mips_shift);
    m_lib.init(lib_name, m_discovery.cpu_count, m_discovery.cpu_model);
}
