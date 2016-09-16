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

#ifndef _QEMU_CONNECTION_STRATEGY_QEMU_INTERNAL_SIGNAL_H
#define _QEMU_CONNECTION_STRATEGY_QEMU_INTERNAL_SIGNAL_H

#include <utility>

#include <rabbits/component/connection_strategy.h>
#include <rabbits/logger.h>
#include <libsc_qemu/libsc_qemu.h>

class QemuInternalSignalCS : public ConnectionStrategy<QemuInternalSignalCS> {
public:
    using typename ConnectionStrategyBase::BindingResult;
    enum Direction { IN, OUT };

private:
    typedef std::pair<QemuObject*, QemuGpio> QemuSignal;

    LibScQemu *m_qemu_inst;

    QemuSignal m_sig;
    Direction m_dir;

public:
    QemuInternalSignalCS(LibScQemu *qemu_inst, QemuObject *obj, QemuGpio gpio, Direction dir)
        : m_qemu_inst(qemu_inst), m_sig(std::make_pair(obj, gpio)), m_dir(dir) {}

    virtual ~QemuInternalSignalCS() {}

    virtual BindingResult bind_peer(QemuInternalSignalCS &cs, PlatformDescription &d)
    {
        QemuSignal *in, *out;

        if (cs.m_qemu_inst != m_qemu_inst) {
            /* Can't connect two signals from two different QEMU instances.
             * Let the SystemC world CS connect them. */
            return BindingResult::BINDING_TRY_NEXT;
        }

        if (cs.m_dir == m_dir) {
            LOG(APP, WRN) << "Trying to connect two QEMU signals that have same direction (at " << d.origin() << ")\n";
            return BindingResult::BINDING_ERROR;
        }

        if (m_dir == IN) {
            in = &m_sig;
            out = &(cs.m_sig);
        } else {
            in = &(cs.m_sig);
            out = &m_sig;
        }

        LOG(APP, DBG) << "Internal QEMU gpio connection\n";
        out->first->gpio_out_connect(out->second, *(in->first), in->second);

        return BindingResult::BINDING_OK;
    }

    virtual BindingResult bind_hierarchical(QemuInternalSignalCS &parent_cs)
    {
        /* Not implemented */
        LOG(APP, ERR) << "QEMU signals hierarchical binding not implemented\n";
        return BindingResult::BINDING_ERROR;
    }
};
#endif
