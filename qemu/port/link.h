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

#pragma once

#include <rabbits/component/port.h>

#include "qemu/connection_strategy/qemu_object_link.h"

class QemuLinkSrcPort : public Port {
private:
    LibScQemu &m_lib;
    QemuObject *m_obj;

    QemuObjectLinkCS m_cs;

public:
    QemuLinkSrcPort(const std::string &name, sc_core::sc_module *parent,
                    LibScQemu &lib, QemuObject *obj)
        : Port(name)
        , m_lib(lib)
        , m_obj(obj)
        , m_cs(&lib, obj)
    {
        add_connection_strategy(m_cs);
        declare_parent(parent);
    }

    const char * get_typeid() const { return "qemu-link-src"; }
};

class QemuLinkDstPort : public Port {
private:
    LibScQemu &m_lib;
    QemuObject *m_obj;

    QemuObjectLinkCS m_cs;

public:
    QemuLinkDstPort(const std::string &name, sc_core::sc_module *parent,
                    LibScQemu &lib, QemuObject *obj, const char *link_name)
        : Port(name)
        , m_lib(lib)
        , m_obj(obj)
        , m_cs(&lib, obj, link_name)
    {
        add_connection_strategy(m_cs);
        declare_parent(parent);
    }

    const char * get_typeid() const { return "qemu-link-dst"; }
};
