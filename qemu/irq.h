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

#ifndef _RABBITS_QEMU_IRQ_H
#define _RABBITS_QEMU_IRQ_H

#include <rabbits/component/irq.h>

class QemuComponent;
class QemuIrqIn;
class QemuIrqOut;

class QemuIrqIn : public IrqIn {
    friend class QemuIrqOut;

protected:
    QemuComponent *m_comp;
    int m_qemu_idx;

public:
    QemuIrqIn() {}
    QemuIrqIn(const std::string name, unsigned int idx, QemuComponent *comp, int qemu_idx)
        : IrqIn(name, idx), m_comp(comp), m_qemu_idx(qemu_idx) {}
    QemuIrqIn(const std::string name, QemuComponent *comp, int qemu_idx)
        : IrqIn(name), m_comp(comp), m_qemu_idx(qemu_idx) {}
    QemuIrqIn(const std::string name, unsigned int idx)
        : IrqIn(name, idx) {}
    explicit QemuIrqIn(const std::string name)
        : IrqIn(name) {}

    virtual ~QemuIrqIn() {}

    virtual sc_core::sc_in<bool>& get_port();

    virtual sc_core::sc_signal<bool>* connect(IrqIn &in);
    virtual sc_core::sc_signal<bool>* connect(IrqOut &out);
};

class QemuIrqOut : public IrqOut {
    friend class QemuIrqIn;

protected:
    QemuComponent *m_comp;
    int m_qemu_idx;

public:
    QemuIrqOut() {}
    QemuIrqOut(const std::string name, unsigned int idx, QemuComponent *comp, int qemu_idx)
        : IrqOut(name, idx), m_comp(comp), m_qemu_idx(qemu_idx) {}
    QemuIrqOut(const std::string name, QemuComponent *comp, int qemu_idx)
        : IrqOut(name), m_comp(comp), m_qemu_idx(qemu_idx) {}
    QemuIrqOut(const std::string name, unsigned int idx)
        : IrqOut(name, idx) {}
    explicit QemuIrqOut(const std::string name)
        : IrqOut(name) {}

    virtual ~QemuIrqOut() {}

    virtual sc_core::sc_out<bool>& get_port();

    virtual sc_core::sc_signal<bool>* connect(IrqIn &in);
    virtual sc_core::sc_signal<bool>* connect(IrqOut &out);
};

#endif
