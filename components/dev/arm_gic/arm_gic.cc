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

class PpiGenerator {
private:
    LibScQemu &m_lib;
    sc_qemu_qdev *m_qdev;
    int m_num_irq, m_cpu_idx;

    int m_cur_irq = 0;

public:
    PpiGenerator(LibScQemu& lib, sc_qemu_qdev *qdev, int num_irq, int cpu_idx)
        : m_lib(lib), m_qdev(qdev), m_num_irq(num_irq), m_cpu_idx(cpu_idx) {}

    QemuInPort* operator()(const std::string &s) {
        QemuInPort *irq;
        const int qemu_idx = m_num_irq + 32*(m_cpu_idx-1) + m_cur_irq++;

        irq = new QemuInPort(s, m_lib, m_qdev, qemu_idx);
        irq->set_autoconnect_to(0);

        return irq;
    }
};

class SpiGenerator {
private:
    LibScQemu &m_lib;
    sc_qemu_qdev *m_qdev;

public:
    SpiGenerator(LibScQemu& lib, sc_qemu_qdev *qdev)
        : m_lib(lib), m_qdev(qdev) {}

    QemuInPort* operator()(const std::string &s, int idx) {
        std::stringstream ss;
        QemuInPort *irq;

        ss << s << (idx);

        irq = new QemuInPort(ss.str(), m_lib, m_qdev, idx);
        irq->set_autoconnect_to(0);

        return irq;
    }
};


QemuArmGic::QemuArmGic(sc_core::sc_module_name name, const Parameters &params, ConfigManager &c)
    : QemuSlave(name, params, c)
{
    const uint32_t revision = params["revision"].as<uint32_t>();
    const uint32_t num_irq = params["num-irq"].as<uint32_t>();
    const bool has_sec_extn = params["has-security-extensions"].as<bool>();
    const uint32_t cpu_if_id = params["cpu-iface-id"].as<uint32_t>();
    const uint32_t min_bpr = params["min-bpr"].as<uint32_t>();
    const int num_cpu = QemuInstance::get(Component::get_config()).get_num_cpu();

    m_qdev = m_lib.qdev_create_arm_gic(revision, num_irq, has_sec_extn,
                                       cpu_if_id, min_bpr);

    SpiGenerator spi_gen(m_lib, m_qdev);
    p_spis = new VectorPort<QemuInPort>("spi", num_irq - 32, spi_gen);

    for (int i = 0; i < num_cpu; i++) {
        PpiGenerator ppi_gen(m_lib, m_qdev, num_irq, i);
        std::stringstream ss;
        ss << "cpu" << i;

        p_ppis.push_back(new VectorPort<QemuInPort>(ss.str() + "-ppi", 32, ppi_gen));

        p_irqs_to_cpus.push_back(new QemuOutPort(ss.str() + "-irq", m_lib,
                                                 m_qdev, i));
        p_irqs_to_cpus.push_back(new QemuOutPort(ss.str() + "-fiq", m_lib,
                                                 m_qdev, num_cpu + i));
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
