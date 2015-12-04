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

#ifndef _RABBITS_QEMU_FACTORY_H
#define _RABBITS_QEMU_FACTORY_H

#include <sstream>

#include <rabbits/component/component.h>
#include <rabbits/component/parameters.h>

#include "cpu.h"
#include "master.h"
#include "instance.h"

template <typename CPU>
class QemuCpuFactory : public Component {
protected:
    std::vector<QemuCpu*> m_cpus;

public:
    QemuCpuFactory(const std::string &name, ComponentParameters &params)
        : Component(name + "_factory", params)
    {
        HasIrqInIface::iterator it;
        HasIrqOutIface::iterator ito;

        QemuInstance &inst = QemuInstance::get();
        declare_master("qemu-master", inst.get_master());

        QemuCpu *cpu = new CPU(name, params);
        declare_component(name, *cpu);

        for (it = cpu->irqs_in_begin(); it != cpu->irqs_in_end(); it++) {
            declare_irq_in(it->second);
        }

        for (ito = cpu->irqs_out_begin(); ito != cpu->irqs_out_end(); ito++) {
            declare_irq_out(ito->second);
        }
    }

    virtual ~QemuCpuFactory() {}
};

#endif
