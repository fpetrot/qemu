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

#include "uart_16550.h"

#include <rabbits/logger.h>

QemuUart16550::QemuUart16550(sc_core::sc_module_name name, const Parameters &params, ConfigManager &c)
    : QemuSlave(name, params, c)
    , m_regshift(params["regshift"].as<int>())
    , m_baudbase(params["clk"].as<int>())
{
    m_obj = m_lib.object_new("serial");
    m_obj->set_prop_int("it-shift", m_regshift);
    m_obj->set_prop_int("baudbase", m_baudbase);
    m_obj->realize();

    p_irq = new QemuOutPort("irq", m_lib, m_obj, QemuGpio("sysbus-irq", 0));
}

QemuUart16550::~QemuUart16550() {
    delete p_irq;
}

void QemuUart16550::do_mmio_map(const AddressRange &range)
{
    m_obj->mmio_map(0, range.begin());
}

