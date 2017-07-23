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

#ifndef _RABBITS_QEMU_COMPONENT_H
#define _RABBITS_QEMU_COMPONENT_H

#include <systemc>

#include <rabbits/component/component.h>

#include "instance.h"
#include "libsc_qemu/libsc_qemu.h"

class QemuComponent : public Component {
protected:
    LibScQemu &m_lib;
    QemuObject *m_obj = nullptr;

public:

    SC_HAS_PROCESS(QemuComponent);

    QemuComponent(sc_core::sc_module_name name, const Parameters &params, ConfigManager &c)
        : Component(name, params, c), m_lib(QemuInstance::get(get_config()).get_lib())
    {}

    virtual ~QemuComponent()
    {
        if (m_obj) {
            delete m_obj;
            m_obj = nullptr;
        }
    }
};

#endif
