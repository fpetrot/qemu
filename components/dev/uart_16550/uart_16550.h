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

#ifndef _QEMU_WRAPPER_DEV_UART_16550_H
#define _QEMU_WRAPPER_DEV_UART_16550_H

#include "qemu/component.h"

class QemuUart16550 : public QemuComponent
{
protected:
    uint32_t m_base_addr;
    int m_baudbase;
    int m_irq_idx;
    int m_regshift;


public:
    QemuUart16550(std::string name, ComponentParameters &params);
    virtual ~QemuUart16550() {}
};

#endif
