/*
 *  This file is part of Rabbits
 *  Copyright (C) 2017  Frédéric Pétrot and others
 *
 *  Moreorless raw copy of cortex_a7.h
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

#ifndef _QEMU_COMPONENTS_CPU_ARM_V7M_H
#define _QEMU_COMPONENTS_CPU_ARM_V7M_H

#include <string>

#include "arm.h"

#include "qemu/port/in.h"
#include "qemu/port/out.h"

class QemuCpuArmV7M : public QemuCpu<32> {
protected:
    using QemuCpu<32>::m_lib;
    using QemuCpu<32>::m_obj;

public:
    using QemuCpu<32>::p_bus;

public:
    QemuCpuArmV7M(sc_core::sc_module_name name, const Parameters &params, ConfigManager &c)
        : QemuCpu<32>(name, params, c, "armv7m")
    {
        m_obj->set_prop_int ("num-irq",   params["num-irq"].as<uint32_t>());
        m_obj->set_prop_str ("cpu-model", params["cpu-model"].as<std::string>().c_str());
        m_obj->set_prop_link("memory",    m_lib.object_get_root_mr());
        realize();
    }

    static void discover(const std::string &name, Parameters &params, ConfigManager &config)
    {
        QemuInstance::get(config).cpu_discover(QemuInstance::CpuType::ARM, "armv7m", params);
    }

    void end_of_elaboration()
    {
        QemuCpu<32>::end_of_elaboration();
    }
};

#endif
