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
#include <rabbits/logger.h>

#include "arm.h"
#include "libsc_qemu/libsc_qemu.h"

using std::string;

QemuCpuArm::QemuCpuArm(sc_core::sc_module_name name, ComponentParameters &params)
    : QemuCpu(name, params)
{
    declare_irq_in("irq", SC_QEMU_ARM_IRQ_IRQ);
    declare_irq_in("fiq", SC_QEMU_ARM_IRQ_FIQ);
    const string & model = params["model"].as<string>();


    if ((model == "cortex-a15") || (model == "cortex-a7")) {
        /* Timers irq */
        declare_irq_out("timer-phys", 0);
        declare_irq_out("timer-virt", 1);
        declare_irq_out("timer-hyp", 2);
        declare_irq_out("timer-sec", 3);
    }

    string gdb_port = params["gdb-server"].as<string>();

    if (gdb_port != "") {
        INF_STREAM("Starting gdb server on port " << gdb_port << "\n");
        m_lib.start_gdb_server("tcp::" + gdb_port);
    }
}

