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

#ifndef _QEMU_CONNECTION_STRATEGY_QEMU_OBJECT_LINK_H
#define _QEMU_CONNECTION_STRATEGY_QEMU_OBJECT_LINK_H

#include <utility>

#include <rabbits/component/connection_strategy.h>
#include <rabbits/logger.h>
#include <libsc_qemu/libsc_qemu.h>

class QemuObjectLinkCS : public ConnectionStrategy<QemuObjectLinkCS> {
public:
    using typename ConnectionStrategyBase::BindingResult;
    using typename ConnectionStrategyBase::ConnectionInfo;

private:
    LibScQemu *m_qemu_inst;
    QemuObject *m_src = nullptr;
    QemuObject *m_dst = nullptr;
    const char *m_link_name = nullptr;

public:
    QemuObjectLinkCS(LibScQemu *qemu_inst, QemuObject *src)
        : m_qemu_inst(qemu_inst)
        , m_src(src) {}

    QemuObjectLinkCS(LibScQemu *qemu_inst, QemuObject *dst, const char *link_name)
        : m_qemu_inst(qemu_inst)
        , m_dst(dst)
        , m_link_name(link_name) {}

    virtual ~QemuObjectLinkCS() {}

    virtual BindingResult bind_peer(QemuObjectLinkCS &cs, ConnectionInfo &info, PlatformDescription &d)
    {
        QemuObjectLinkCS *src, *dst;

        if (cs.m_qemu_inst != m_qemu_inst) {
            /* Can't connect two objects from two different QEMU instances. */
            return BindingResult::BINDING_ERROR;
        }

        if (m_src && cs.m_src) {
            LOG(APP, WRN) << "Trying to connect two QEMU object links that "
                             "have same direction (at " << d.origin() << ")\n";
            return BindingResult::BINDING_ERROR;
        }

        if (m_src) {
            src = this;
            dst = &cs;
        } else {
            src = &cs;
            dst = this;
        }

        LOG(APP, DBG) << "Internal QEMU object link\n";
        dst->m_dst->set_prop_link(dst->m_link_name, src->m_src);

        return BindingResult::BINDING_OK;
    }

    virtual BindingResult bind_hierarchical(QemuObjectLinkCS &parent_cs, ConnectionInfo &info)
    {
        /* Not implemented */
        LOG(APP, ERR) << "QEMU object link hierarchical binding not implemented\n";
        return BindingResult::BINDING_ERROR;
    }

    virtual const char * get_typeid() const { return "qemu-object-link"; }
};
#endif

