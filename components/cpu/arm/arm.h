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

#ifndef _QEMU_WRAPPER_QEMU_CPU_ARM_H
#define _QEMU_WRAPPER_QEMU_CPU_ARM_H

#include <string>

#include "../cpu.h"
#include "qemu/port/in.h"

class QemuCpuArm : public QemuCpu<32> {
protected:
    using QemuCpu<32>::m_lib;
    using QemuCpu<32>::m_qdev;

public:
    using QemuCpu<32>::p_bus;

public:
    QemuInPort p_in_irq;
    QemuInPort p_in_fiq;

    QemuCpuArm(sc_core::sc_module_name name, const Parameters &params, ConfigManager &c);
};

#endif
