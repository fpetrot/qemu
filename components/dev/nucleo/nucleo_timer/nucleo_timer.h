/*
 *  This file is part of Rabbits
 *  Copyright (C) 2017  Clement Chigot
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

#ifndef _NUCLEO_TIMER_H_
#define _NUCLEO_TIMER_H_

#include "qemu/slave.h"
#include "qemu/port/out.h"

class QemuNucleoTimer : public QemuSlave<>{
protected:
	using QemuSlave<>::m_obj; 

    void do_mmio_map(const AddressRange &range);

public:
	QemuOutPort *p_irq;
  
	QemuNucleoTimer(sc_core::sc_module_name name, const Parameters &params, ConfigManager &c);
	virtual ~QemuNucleoTimer();

	
};


#endif
