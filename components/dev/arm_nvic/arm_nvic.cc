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

#include <sstream>
#include <algorithm>

#include <rabbits/platform/description.h>
#include <rabbits/logger.h>

#include "arm_nvic.h"
#include "qemu/instance.h"

class IrqGenerator {
private:
    LibScQemu &m_lib;
    QemuObject *m_obj;

public:
    IrqGenerator(LibScQemu& lib, QemuObject *obj)
        : m_lib(lib), m_obj(obj) {}

    QemuInPort* operator()(const std::string &s, int idx) {
        std::stringstream ss;
        QemuInPort *irq;

        ss << s << (idx);

        irq = new QemuInPort(ss.str(), m_lib, m_obj, idx);
        irq->set_autoconnect_to(0);

        return irq;
    }
};


QemuArmNvic::QemuArmNvic(sc_core::sc_module_name name, const Parameters &params, ConfigManager &c)
    : QemuSlave(name, params, c, "armv7m_nvic")
    , p_irq_to_cpu("irq-out", m_lib, m_obj, QemuGpio("sysbus-irq", 0))
    , p_link_out("link", this, m_lib, m_obj)
{
    const uint32_t num_irq = params["num-irq"].as<uint32_t>();

    m_obj->set_prop_int("num-irq", num_irq);
    m_obj->realize();

    IrqGenerator irq_gen(m_lib, m_obj);
    p_irqs = new VectorPort<QemuInPort>("irq", num_irq, irq_gen);
}

QemuArmNvic::~QemuArmNvic()
{
    delete p_irqs;
}

void QemuArmNvic::do_mmio_map(const AddressRange &range)
{
    m_obj->mmio_map(0, range.begin());
}
