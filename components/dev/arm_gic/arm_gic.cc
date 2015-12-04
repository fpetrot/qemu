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

#include <sstream>

#include <sc_qemu.h>

#include <rabbits/platform/description.h>
#include <rabbits/logger.h>

#include "arm_gic.h"
#include "qemu/instance.h"

QemuArmGic::QemuArmGic(std::string name, ComponentParameters &params)
    : QemuComponent(name, params)
{
    int num_irq = params["num-irq"].as<int>();
    int num_cpu = QemuInstance::get().get_num_cpu();

    m_num_irq = num_irq;
    m_qdev = m_lib.qdev_create_arm_gic(num_irq);

    declare_vector_irq_in("", 0, num_irq - 32 + (32*num_cpu));

    for (int i = 0; i < num_cpu; i++) {
	    std::stringstream cpu_num;
	    cpu_num << "cpu" << i;
	    declare_irq_out(cpu_num.str() + "-irq", i);
	    declare_irq_out(cpu_num.str() + "-fiq", num_cpu + i);
    }

    mem_map(params);
}

QemuArmGic::~QemuArmGic() 
{
    m_lib.qdev_destroy(m_qdev);
}

void QemuArmGic::mem_map(ComponentParameters &params) 
{
    PlatformDescription &descr = params.get_base_description();
    AddressRange map;

    try {
        map = descr["bus-mapping"].as<AddressRange>();
    } catch(PlatformDescription::InvalidConversionException e) {
        WRN_STREAM("Missing or invalid `bus-mapping` attribute for component `" << name() << "`" << std::endl);
        return;
    }

    m_lib.qdev_mmio_map(m_qdev, 0, map.begin() + 0x1000);  /* GIC distributor */
    m_lib.qdev_mmio_map(m_qdev, 1, map.begin() + 0x2000);  /* GIC CPU interface */
}
