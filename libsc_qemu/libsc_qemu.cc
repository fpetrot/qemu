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

#include "libsc_qemu.h"

#include <rabbits/logger.h>
#include <rabbits/config/manager.h>
#include <rabbits/dynloader/dynloader.h>
#include <rabbits/dynloader/dynlib.h>
#include <rabbits/rabbits_exception.h>

#include <cassert>
#include <sc-qemu/sc-qemu.h>


LibScQemu::LibScQemu(ConfigManager &config)
    : m_config(config)
{
}

LibScQemu::~LibScQemu()
{
    /* TODO */
}

/* ----------------------------
 * Callbacks from sc-qemu libs
 * ---------------------------- */
uint32_t LibScQemu::qemu_sc_read(void *opaque, uint32_t addr,
                                 uint32_t size, const sc_qemu_io_attr *attr)
{
    LibScQemu *w = reinterpret_cast<LibScQemu*>(opaque);
    const int i = attr->cpuid;

    return w->m_io_cbs[i]->qemu_io_read(addr, size);
}

void LibScQemu::qemu_sc_write(void *opaque, uint32_t addr,
                              uint32_t val, uint32_t size,
                              const sc_qemu_io_attr *attr)
{
    LibScQemu *w = reinterpret_cast<LibScQemu*>(opaque);
    const int i = attr->cpuid;

    w->m_io_cbs[i]->qemu_io_write(addr, val, size);
}

void LibScQemu::object_gpio_event(sc_qemu_object *obj, int n, int level, void *opaque)
{
    qemu_object_gpio_callbacks *cb = reinterpret_cast<qemu_object_gpio_callbacks*>(opaque);

    cb->qemu_object_gpio_event(obj, n, level);
}

/* ---------------------------- */

void LibScQemu::init(std::string libname)
{
    sc_qemu_init_struct s;
    sc_qemu_init_fn qemu_init = NULL;
    DynamicLoader &dyn = m_config.get_dynloader();

    m_lib = dyn.search_and_load_library(libname);
    if (m_lib == NULL) {
        throw RabbitsException("sc-qemu library `"
                               + libname + "." + DynLib::get_lib_extension() + "` not found.");
    }

    if (!m_lib->check_symbol(SC_QEMU_INIT_SYM_STR)) {
        throw RabbitsException("Invalid sc-qemu library `" + libname
                               + "`. (Symbol " + SC_QEMU_INIT_SYM_STR + " not found).");
    }

    m_qemu_import = new qemu_import;

    qemu_init = (sc_qemu_init_fn) m_lib->get_symbol(SC_QEMU_INIT_SYM_STR);

    s.sc_import.write = qemu_sc_write;
    s.sc_import.read = qemu_sc_read;

    s.q_import = m_qemu_import;
    s.opaque = this;
    s.max_run_time = m_insn_limit;
    s.cpu_mips_shift = m_mips_shift;
    s.map_whole_as = m_map_whole_as;

    m_qemu_ctx = qemu_init(&s);
}

void LibScQemu::map_io(uint32_t base, uint32_t size)
{
    assert(m_qemu_import);
    m_qemu_import->map_io(m_qemu_ctx, base, size);
}

void LibScQemu::map_dmi(uint32_t base, uint32_t size, void *data, bool readonly)
{
    assert(m_qemu_import);
    m_qemu_import->map_dmi(m_qemu_ctx, base, size, data, readonly);
}

bool LibScQemu::cpus_loop(int64_t *elapsed)
{
    assert(m_qemu_import);
    return m_qemu_import->cpu_loop(m_qemu_ctx, elapsed);
}

void LibScQemu::start_gdb_server(std::string port)
{
    assert(m_qemu_import);
    m_qemu_import->start_gdbserver(m_qemu_ctx, port.c_str());
}

void LibScQemu::register_io_callback(qemu_io_callbacks &cb, int cpuid)
{
    m_io_cbs[cpuid] = &cb;
}

QemuObject * LibScQemu::object_new(const char *type_name)
{
    sc_qemu_object *obj =  m_qemu_import->object_new(m_qemu_ctx, type_name);
    QemuObject *ret = new QemuObject(obj, *m_qemu_import);

    return ret;
}


/* ---------------------------- */


void QemuObject::realize()
{
    m_import.object_property_set_bool(m_obj, true, "realized");
    m_realized = true;
}

void QemuObject::set_prop_bool(const char *name, bool val)
{
    m_import.object_property_set_bool(m_obj, val, name);
}

void QemuObject::set_prop_int(const char *name, int64_t val)
{
    m_import.object_property_set_int(m_obj, val, name);
}

void QemuObject::set_prop_str(const char *name, const char *val)
{
    m_import.object_property_set_str(m_obj, val, name);
}

int QemuObject::get_cpu_id()
{
    return m_import.cpu_get_id(m_obj);
}

void QemuObject::mmio_map(int mmio_id, uint64_t addr)
{
    m_import.object_mmio_map(m_obj, mmio_id, addr);
}

void QemuObject::gpio_out_connect(const QemuGpio &gpio_out, QemuObject &in, const QemuGpio &gpio_in)
{
    m_import.object_gpio_connect(m_obj, gpio_out.name, gpio_out.idx,
                                 in.m_obj, gpio_in.name, gpio_in.idx);
}

void QemuObject::gpio_in_set(const QemuGpio &gpio, int level)
{
    m_import.object_gpio_update(m_obj, gpio.name, gpio.idx, level);
}

void QemuObject::gpio_out_register_cb(const QemuGpio &gpio, qemu_object_gpio_callbacks *cb)
{
    m_import.object_gpio_register_cb(m_obj, gpio.name, gpio.idx,
                                     LibScQemu::object_gpio_event, cb);
}
