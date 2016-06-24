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

#ifndef _QEMU_PORT_IN_H
#define _QEMU_PORT_IN_H

#include <rabbits/component/port/in.h>
#include "qemu/connection_strategy/qemu_internal_signal.h"

class QemuInPort : public InPort<bool> {
private:
    sc_core::sc_signal<bool> *m_stub_sig = nullptr;
    
protected:
    LibScQemu &m_lib;
    sc_qemu_qdev *m_qdev;
    int m_idx;

    QemuInternalSignalCS m_cs;

    void watch_port() {
        const sc_core::sc_event &ev = sc_p.default_event();

        for(;;) {
            sc_core::wait(ev);
            m_lib.qdev_irq_update(m_qdev, m_idx, sc_p.read());
        }
    }

public:
    QemuInPort(const std::string &name, LibScQemu &lib, sc_qemu_qdev *qdev, int idx)
        : InPort(name)
        , m_lib(lib)
        , m_qdev(qdev)
        , m_idx(idx)
        , m_cs(&lib, qdev, idx, QemuInternalSignalCS::IN)
    {
        add_connection_strategy_front(m_cs);
    }

    virtual ~QemuInPort() { delete m_stub_sig; }

    virtual void selected_strategy(ConnectionStrategyBase &cs) {
        if (&cs != &m_cs) {
            /* SystemC to QEMU connection. We must setup the SystemC thread
             * that will watch the gpio and update QEMU accordingly */
            push_thread_to_parent([this]() { this->watch_port(); });
        } else {
            /* Internal QEMU connection. Stub the useless sc_port to make it
             * connected */
            m_stub_sig = new sc_core::sc_signal<bool>;
            sc_p(*m_stub_sig);
        }
    }
};

#endif

