/*
 *  This file is part of Rabbits
 *  Copyright (C) 2017 Chigot Clément
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 lo *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "nucleo_timer.h"

#include <rabbits/logger.h>

QemuNucleoTimer::QemuNucleoTimer(sc_core::sc_module_name name, const Parameters &params, ConfigManager &c)
	: QemuSlave(name, params, c)
{
	m_obj = m_lib.object_new("stm32f2xx-timer");
	m_obj->realize();
	p_irq = new QemuOutPort("irq", m_lib, m_obj, QemuGpio("sysbus-irq", 0));
	
}

QemuNucleoTimer::~QemuNucleoTimer(){
    delete p_irq;  
}

void QemuNucleoTimer::do_mmio_map(const AddressRange &range){
	m_obj->mmio_map(0, range.begin());
}

