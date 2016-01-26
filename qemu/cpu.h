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

#ifndef _QEMU_CPU_H
#define _QEMU_CPU_H

#include <map>
#include <systemc.h>

#include "libsc_qemu/libsc_qemu.h"
#include "component.h"

class QemuCpu : public QemuComponent {
protected:
    int m_cpuid;

public:
    SC_HAS_PROCESS(QemuCpu);

    QemuCpu(sc_core::sc_module_name name, ComponentParameters &params)
        : QemuComponent(name, params)
    {
        QemuInstance &inst = QemuInstance::get();

        m_cpuid = inst.get_next_cpuid();
        m_qdev = m_lib.cpu_get_qdev(m_cpuid);

        declare_master("qemu-master", inst.get_master());
    }

    virtual ~QemuCpu() {}
};
#endif
