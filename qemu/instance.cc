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

using std::string;

QemuInstance *QemuInstance::m_inst = NULL;

const string QemuInstance::CpuType::LIB[] = {
    [CpuType::NONE] = "",
    [CpuType::ARM] = "libqemu-system-arm",
};

QemuInstance::QemuInstance()
    : m_master("qemu-master", ComponentParameters::EMPTY, m_lib)
    , m_next_cpuid(0)
{
}

void QemuInstance::add_cpu(CpuType::value type, const string model)
{
    if (m_discovery.cpu_type == CpuType::NONE) {
        m_discovery.cpu_type = type;
        m_discovery.cpu_model = model;
    } else if ((m_discovery.cpu_type != type) || (m_discovery.cpu_model != model)) {
        /* XXX non smp systems not supported */
    }

    m_discovery.cpu_count++;
}

void QemuInstance::discover(const std::string &name, ComponentParameters &params)
{
    PlatformDescription &descr = params.get_base_description();
    const string cpu_type = descr["type"].as<string>();
    const string model = params["model"].as<string>();

    if (cpu_type == "cpu-arm") {
        add_cpu(CpuType::ARM, model);
    }
}

void QemuInstance::lib_init()
{
    if (m_discovery.cpu_type == CpuType::NONE) {
        /* XXX no cpu in description */
        abort();
    }

    const string lib_name = CpuType::LIB[m_discovery.cpu_type];

    DBG_STREAM("Initializing sc-qemu with "
               << m_discovery.cpu_count << " " << m_discovery.cpu_model
               << " cpu(s)" << std::endl);

    m_lib.register_io_callback(m_master);
    m_lib.init(lib_name, m_discovery.cpu_count, m_discovery.cpu_model);
}
