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

#ifndef _QEMU_WRAPPER_H
#define _QEMU_WRAPPER_H

#include <vector>
#include <cassert>

#include "component.h"

#include <rabbits/component/port/tlm_initiator.h>

template <unsigned int BUSWIDTH = 32>
class QemuMaster : public QemuComponent, public tlm::tlm_bw_transport_if<>
{
#if 0
    void add_map(uint32_t base_address, uint32_t size) {
        m_lib.map_io(base_address, size);
    }

    void add_map_dmi(uint32_t base_address, uint32_t size, void *data) {
        m_lib.map_dmi(base_address, size, data);
    }

    virtual void dmi_hint_cb(uint64_t start, uint64_t size, void *data,
                             sc_core::sc_time read_latency, sc_core::sc_time write_latency)
    {
        if (data != NULL) {
            DBG_PRINTF("DMI mapping from %" PRIx64 " to %" PRIx64 "\n", start, start+size);
            add_map_dmi(start, size, data);
        } else {
            DBG_PRINTF("I/O mapping from %" PRIx64 " to %" PRIx64 "\n", start, start+size);
            add_map(start, size);
        }
    }
#endif

public:
    TlmInitiatorPort<BUSWIDTH> p_bus;

    QemuMaster(sc_core::sc_module_name name, ComponentParameters &params)
        : QemuComponent(name, params)
	, p_bus("bus", *this) {}

    virtual ~QemuMaster() {}


    /* tlm::tlm_bw_transport_if */
    virtual tlm::tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload& trans,
                                               tlm::tlm_phase& phase,
                                               sc_core::sc_time& t)
    {
        ERR_PRINTF("Non-blocking transport not implemented\n");
        abort();
        return tlm::TLM_COMPLETED;
    }

    virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range,
                                           sc_dt::uint64 end_range)
    {
        ERR_PRINTF("DMI memory invalidation not implemented\n");
        abort();
    }
};

#endif
