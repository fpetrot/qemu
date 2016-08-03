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
#include <algorithm>

#include <sc-qemu/sc_qemu.h>

#include <rabbits/platform/description.h>
#include <rabbits/logger.h>

#include "arm_gic.h"
#include "qemu/instance.h"

class IrqInGenerator {
private:
    LibScQemu &m_lib;
    sc_qemu_qdev *m_qdev;
    int m_num_irq, m_num_cpu;

    int m_cur_irq = 0;

public:
    IrqInGenerator(LibScQemu& lib, sc_qemu_qdev *qdev, int num_irq, int num_cpu) 
        : m_lib(lib), m_qdev(qdev), m_num_irq(num_irq), m_num_cpu(num_cpu) {}

    QemuInPort* operator()() {
        std::stringstream ss;
        QemuInPort *irq;

        ss << "irq" << m_cur_irq;
        irq = new QemuInPort(ss.str(), m_lib, m_qdev, m_cur_irq++);

        irq->set_autoconnect_to(0);

        return irq;
    }
};


QemuArmGic::QemuArmGic(sc_core::sc_module_name name, const Parameters &params, ConfigManager &c)
    : QemuSlave(name, params, c)
{
    const int num_irq = params["num-irq"].as<int>();
    const int num_cpu = QemuInstance::get(Component::get_config()).get_num_cpu();

    m_qdev = m_lib.qdev_create_arm_gic(num_irq);

    IrqInGenerator irq_gen(m_lib, m_qdev, num_irq, num_cpu);

    p_irqs_in.resize(num_irq - 32 + (32 * num_cpu));
    std::generate(p_irqs_in.begin(), p_irqs_in.end(), irq_gen);

    for (int i = 0; i < num_cpu; i++) {
        std::stringstream cpu_num;
        cpu_num << "cpu" << i;
        p_irqs_to_cpus.push_back(new QemuOutPort(cpu_num.str() + "-irq", m_lib, m_qdev, i));
        p_irqs_to_cpus.push_back(new QemuOutPort(cpu_num.str() + "-fiq", m_lib, m_qdev, num_cpu + i));
    }
}

QemuArmGic::~QemuArmGic() 
{
    m_lib.qdev_destroy(m_qdev);
}

void QemuArmGic::do_mmio_map(const AddressRange &range)
{
    m_lib.qdev_mmio_map(m_qdev, 0, range.begin() + 0x1000);  /* GIC distributor */
    m_lib.qdev_mmio_map(m_qdev, 1, range.begin() + 0x2000);  /* GIC CPU interface */
}
