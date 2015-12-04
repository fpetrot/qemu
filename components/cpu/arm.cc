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

#include <sc_qemu.h>

#include "arm.h"
#include "libsc_qemu/libsc_qemu.h"

QemuCpuArm::QemuCpuArm(std::string name, ComponentParameters &params)
    : QemuCpu(name, params)
{
    declare_irq_in("irq", SC_QEMU_ARM_IRQ_IRQ);
    declare_irq_in("fiq", SC_QEMU_ARM_IRQ_FIQ);

    if (params["model"].as<std::string>() == "cortex-a15") {
        /* Timers irq */
        declare_irq_out("timer-phys", 0);
        declare_irq_out("timer-virt", 1);
        declare_irq_out("timer-hyp", 2);
        declare_irq_out("timer-sec", 3);
    }
}
