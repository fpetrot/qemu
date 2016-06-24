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

/* 
 *  This QEMU device encapsulate the privates devices of a cortex-a15 mpcore
 *  It includes the GIC (interrupt controller), which is already connected to
 *  the cpus, and generic timer irq lines connected to the GIC.
 */

#ifndef _QEMU_WRAPPER_DEV_ARM_GIC_H
#define _QEMU_WRAPPER_DEV_ARM_GIC_H

#include <vector>

#include "qemu/slave.h"
#include "qemu/port/in.h"
#include "qemu/port/out.h"


class QemuArmGic : public QemuSlave<>
{
public:
    std::vector< QemuInPort* > p_irqs_in;
    std::vector< QemuOutPort* > p_irqs_to_cpus;

protected:
    void do_mmio_map(const AddressRange &range);

public:
    QemuArmGic(sc_core::sc_module_name name, ComponentParameters &params);
    ~QemuArmGic();
};

#endif
