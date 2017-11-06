/*
 *  This file is part of Rabbits
 *  Copyright (C) 2017  Clement Deschamps and Luc Michel
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

#ifndef _QEMU_COMPONENTS_CPU_ARM_CORTEX_M4_H
#define _QEMU_COMPONENTS_CPU_ARM_CORTEX_M4_H

#include "arm.h"
#include "qemu/port/link.h"

class QemuCpuArmCortexM4 : public QemuCpuArm {
public:
    QemuLinkDstPort p_nvic_link;

    QemuCpuArmCortexM4(sc_core::sc_module_name name, const Parameters &params, ConfigManager &c)
        : QemuCpuArm(name, params, c, "cortex-m4")
        , p_nvic_link("nvic-link", this, m_lib, m_obj, "nvic")
    {
        realize();
    }

    static void discover(const std::string &name, Parameters &params, ConfigManager &config)
    {
        QemuInstance::get(config).cpu_discover(QemuInstance::CpuType::ARM,
                                         "cortex-m4", params);
    }
};

#endif

