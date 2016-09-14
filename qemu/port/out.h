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

#ifndef _QEMU_PORT_OUT_H
#define _QEMU_PORT_OUT_H

#include <rabbits/component/port/out.h>
#include "qemu/connection_strategy/qemu_internal_signal.h"

class QemuOutPort : public OutPort<bool>, public qemu_object_gpio_callbacks {
private:
    sc_core::sc_signal<bool> *m_stub_sig = nullptr;

protected:
    LibScQemu *m_lib;
    QemuObject *m_obj;
    QemuGpio m_gpio;

    QemuInternalSignalCS m_cs;

public:
    QemuOutPort(const std::string &name, LibScQemu &lib, QemuObject *obj, QemuGpio gpio)
        : OutPort(name), m_gpio(gpio), m_cs(&lib, obj, gpio, QemuInternalSignalCS::OUT)
    {
        m_lib = &lib;
        m_obj = obj;

        add_connection_strategy_front(m_cs);
    }

    void qemu_object_gpio_event(sc_qemu_object *obj, int n, int level)
    {
        sc_p = !!level;
    }

    virtual void selected_strategy(ConnectionStrategyBase &cs) {
        if (&cs != &m_cs) {
            /* QEMU to SystemC signal connection */
            m_obj->gpio_out_register_cb(m_gpio, this);
        } else {
            /* Internal QEMU connection. Stub the useless sc_port to make it
             * connected */
            m_stub_sig = new sc_core::sc_signal<bool>;
            sc_p(*m_stub_sig);
        }
    }
};

#endif
