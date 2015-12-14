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

#include "irq.h"
#include "component.h"

#include <rabbits/logger.h>

using namespace sc_core;

sc_signal<bool>* QemuIrqIn::connect(IrqIn &in)
{
    ERR_STREAM("qemu irq in to irq in connection n/i\n");
    abort();
}

sc_signal<bool>* QemuIrqIn::connect(IrqOut &out)
{
    QemuIrqOut *qout = dynamic_cast<QemuIrqOut*>(&out);

    if (qout) {
        /* QEMU internal connection */
        LibScQemu *lib = &(m_comp->m_lib);

        if (lib != &(qout->m_comp->m_lib)) {
            /* Two different QEMU instances */
            return IrqIn::connect(out);
        }

	DBG_STREAM("sc-qemu internal IRQ connection " 
		   << qout->m_comp->name() << ":" << qout->name() << " -> "
		   << m_comp->name() << ":" << name() << "\n");

        lib->qdev_irq_connect(qout->m_comp->m_qdev, qout->m_qemu_idx,  m_comp->m_qdev, m_qemu_idx);

        set_connected();
        out.set_connected();

        return NULL;

    } else {
        /* Classical SystemC connection */
        return IrqIn::connect(out);
    }
}

sc_core::sc_in<bool>& QemuIrqIn::get_port()
{
    return m_comp->create_irq_in_port(m_qemu_idx);
}




sc_core::sc_signal<bool>* QemuIrqOut::connect(IrqIn &in)
{
    QemuIrqIn *qin = dynamic_cast<QemuIrqIn*>(&in);

    if (qin) {
        /* QEMU internal connection */
        LibScQemu *lib = &(m_comp->m_lib);

        if (lib != &(qin->m_comp->m_lib)) {
            /* Two different QEMU instances */
            return IrqOut::connect(in);
        }

	DBG_STREAM("sc-qemu internal IRQ connection " 
		   << m_comp->name() << ":" << name() << " -> "
		   << qin->m_comp->name() << ":" << qin->name() << "\n");

        lib->qdev_irq_connect(m_comp->m_qdev, m_qemu_idx,  qin->m_comp->m_qdev, qin->m_qemu_idx);

        set_connected();
        in.set_connected();

        return NULL;

    } else {
        /* Classical SystemC connection */
        return IrqOut::connect(in);
    }
}

sc_core::sc_signal<bool>* QemuIrqOut::connect(IrqOut &out)
{
    ERR_STREAM("qemu irq out to irq out connection n/i\n");
    abort();
}

sc_core::sc_out<bool>& QemuIrqOut::get_port()
{
    ERR_STREAM("qemu to systemc irq forwarding n/i\n");
    abort();
}
