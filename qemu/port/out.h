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

class QemuOutPort : public OutPort<bool> {
private:
    sc_core::sc_signal<bool> *m_stub_sig = nullptr;

protected:
    int m_idx;

    QemuInternalSignalCS m_cs;

public:
    QemuOutPort(const std::string &name, LibScQemu &lib, sc_qemu_qdev *qdev, int idx)
        : OutPort(name), m_idx(idx), m_cs(&lib, qdev, idx, QemuInternalSignalCS::OUT)
    {
        add_connection_strategy_front(m_cs);
    }

    virtual void selected_strategy(ConnectionStrategyBase &cs) {
        if (&cs != &m_cs) {
            LOG(APP, ERR) << "QEMU to SystemC gpio connection is not implemented\n";
        } else {
            /* Internal QEMU connection. Stub the useless sc_port to make it
             * connected */
            m_stub_sig = new sc_core::sc_signal<bool>;
            sc_p(*m_stub_sig);
        }
    }
};

#endif
