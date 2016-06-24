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

#ifndef _QEMU_QEMU_SLAVE_H
#define _QEMU_QEMU_SLAVE_H

#include <rabbits/component/port/tlm_target.h>
#include "component.h"

template <unsigned int BUSWIDTH = 32>
class QemuSlave : public QemuComponent
                , public tlm::tlm_fw_transport_if<>
                , public TlmTargetMappedListener {
protected:
    virtual void do_mmio_map(const AddressRange &range) {}

public:
    TlmTargetPort<BUSWIDTH> p_bus;
    QemuSlave(sc_core::sc_module_name n, ComponentParameters &params)
        : QemuComponent(n, params), p_bus("bus", *this)
    {
        p_bus.register_mapped_ev_listener(this);
    }

    virtual ~QemuSlave() {}

    /* tlm::tlm_fw_transport_if<> */
    virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& trans,
                                    tlm::tlm_dmi& dmi_data)
    {
        return false;
    }

    virtual tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& trans,
                                               tlm::tlm_phase& phase,
                                               sc_core::sc_time& t)
    {
        ERR_PRINTF("Non-blocking transport not implemented\n");
        abort();
        return tlm::TLM_COMPLETED;
    }

    virtual void b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& delay)
    {
        ERR_PRINTF("SystemC to QEMU memory request not implemented\n");
        trans.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
    }

    virtual unsigned int transport_dbg(tlm::tlm_generic_payload& trans)
    {
        return 0;
    }

    void tlm_target_mapped_event(const AddressRange &r)
    {
        do_mmio_map(r);
        QemuInstance::get().self_mapping_add(r);
    }
};

#endif
