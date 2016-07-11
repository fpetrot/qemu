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

#ifndef _QEMU_COMPONENTS_CPU_ARM_CORTEX_A7_H
#define _QEMU_COMPONENTS_CPU_ARM_CORTEX_A7_H

#include "arm.h"

#include "qemu/port/out.h"

class QemuCpuArmCortexA7 : public QemuCpuArm {
public:
    QemuOutPort p_irq_timer_phys;
    QemuOutPort p_irq_timer_virt;
    QemuOutPort p_irq_timer_hyp;
    QemuOutPort p_irq_timer_sec;

    QemuCpuArmCortexA7(sc_core::sc_module_name name, Parameters &params)
        : QemuCpuArm(name, params)
        , p_irq_timer_phys("irq-timer-phys", m_lib, m_qdev, 0)
        , p_irq_timer_virt("irq-timer-virt", m_lib, m_qdev, 1)
        , p_irq_timer_hyp("irq-timer-hyp", m_lib, m_qdev, 2)
        , p_irq_timer_sec("irq-timer-sec", m_lib, m_qdev, 3) {}

    static void discover(const std::string &name, Parameters &params)
    {
        QemuInstance::get().cpu_discover(QemuInstance::CpuType::ARM,
                                         "cortex-a7", params);
    }
};

#endif
