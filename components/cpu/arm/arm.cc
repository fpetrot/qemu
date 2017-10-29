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

#include <string>
#include <rabbits/logger.h>

#include "arm.h"
#include "libsc_qemu/libsc_qemu.h"

using std::string;

QemuCpuArm::QemuCpuArm(sc_core::sc_module_name name, const Parameters &params,
                       ConfigManager &c, const std::string & model)
    : QemuCpu<32>(name, params, c, model + "-arm-cpu")
    , p_in_irq("irq", m_lib, m_obj, 0)
    , p_in_fiq("fiq", m_lib, m_obj, 1)
{
}

void QemuCpuArm::end_of_elaboration()
{
    QemuCpu<32>::end_of_elaboration();

    m_external_ev |= p_in_irq.sc_p.default_event();
    m_external_ev |= p_in_fiq.sc_p.default_event();
}
