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


#ifndef _QEMU_WRAPPER_DEV_ARM_NVIC_H
#define _QEMU_WRAPPER_DEV_ARM_NVIC_H

#include <vector>

#include "qemu/slave.h"
#include "qemu/port/in.h"
#include "qemu/port/out.h"
#include "qemu/port/link.h"
#include <rabbits/component/port/vector.h>


class QemuArmNvic : public QemuSlave<>
{
public:
    QemuOutPort p_irq_to_cpu;
    VectorPort<QemuInPort> *p_irqs;

    QemuLinkSrcPort p_link_out;

protected:
    using QemuSlave<>::m_obj;

    void do_mmio_map(const AddressRange &range);

public:
    QemuArmNvic(sc_core::sc_module_name name, const Parameters &params, ConfigManager &c);
    ~QemuArmNvic();
};

#endif
