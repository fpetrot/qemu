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
    sc_qemu_qdev *m_qdev;

#if 0
    void irq_in_thread() 
    {
        std::map<int, sc_core::sc_in<bool>* >::iterator it;

        if (!m_irq_in_ports.size()) {
            return;
        }

        for (;;) {
            wait(m_irq_in_ports_events);

            for (it = m_irq_in_ports.begin(); it != m_irq_in_ports.end(); it++) {
                int irq_idx = it->first;
                sc_core::sc_in<bool> * irq_line = it->second;
                
                if (irq_line->posedge()) {
                    m_lib.qdev_irq_update(m_qdev, irq_idx, 1);
                } else if (irq_line->negedge()) {
                    m_lib.qdev_irq_update(m_qdev, irq_idx, 0);
                }
            }
        }
    }

    sc_core::sc_in<bool> & create_irq_in_port(int idx)
    {
        if (m_irq_in_ports.find(idx) == m_irq_in_ports.end()) {
            m_irq_in_ports[idx] = new sc_core::sc_in<bool>;
        }

        return *(m_irq_in_ports[idx]);
    }

    virtual void end_of_elaboration() {
        ComponentBase::end_of_elaboration();

        std::map<int, sc_core::sc_in<bool>* >::iterator it;

        for (it = m_irq_in_ports.begin(); it != m_irq_in_ports.end(); it++) {
            sc_core::sc_in<bool> * irq_line = it->second;

            m_irq_in_ports_events |= irq_line->default_event();
        }
    }
#endif

public:

    SC_HAS_PROCESS(QemuComponent);

    QemuComponent(sc_core::sc_module_name name, ComponentParameters &params)
        : Component(name, params), m_lib(QemuInstance::get().get_lib()), m_qdev(NULL) 
    {
        //SC_THREAD(irq_in_thread);
    }

    virtual ~QemuComponent() {}
};

#endif
