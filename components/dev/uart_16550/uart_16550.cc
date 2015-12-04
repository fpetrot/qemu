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

QemuUart16550::QemuUart16550(std::string name, ComponentParameters &params)
    : QemuComponent(name, params)
{
    PlatformDescription &descr = params.get_base_description();
    AddressRange map;

    m_regshift = params["regshift"].as<int>();
    m_baudbase = params["baudbase"].as<int>();
    
    try {
        map = descr["bus-mapping"].as<AddressRange>();
    } catch(PlatformDescription::InvalidConversionException e) {
        WRN_STREAM("Missing or invalid `bus-mapping` attribute for component `"
                   << name << "`" << std::endl);
        return;
    }

    m_qdev = m_lib.qdev_create_uart_16550(m_regshift, m_baudbase);

    m_lib.qdev_mmio_map(m_qdev, 0, map.begin());

    declare_irq_out("irq", 0);
}