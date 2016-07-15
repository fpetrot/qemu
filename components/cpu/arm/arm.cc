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

#include <string>
#include <sc-qemu/sc_qemu.h>
#include <rabbits/logger.h>

#include "arm.h"
#include "libsc_qemu/libsc_qemu.h"

using std::string;

QemuCpuArm::QemuCpuArm(sc_core::sc_module_name name, Parameters &params, ConfigManager &c)
    : QemuCpu<32>(name, params, c)
    , p_in_irq("irq", m_lib, m_qdev, SC_QEMU_ARM_IRQ_IRQ)
    , p_in_fiq("fiq", m_lib, m_qdev, SC_QEMU_ARM_IRQ_FIQ)
{
    string gdb_port = params["gdb-server"].as<string>();

    if (gdb_port != "") {
        LOG(APP, INF) << "Starting gdb server on port " << gdb_port << "\n";
        m_lib.start_gdb_server("tcp::" + gdb_port);
    }
}

