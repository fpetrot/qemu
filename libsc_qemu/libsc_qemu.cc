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
#include <rabbits/dynloader/dynloader.h>
#include <rabbits/dynloader/dynlib.h>
#include <rabbits/rabbits_exception.h>

#include <cassert>
#include <sc-qemu/sc_qemu.h>


LibScQemu::LibScQemu()
    : m_insn_limit(0)
    , m_mips_shift(0)
    , m_qemu_ctx(NULL)
    , m_qemu_import(NULL)
    , m_lib(NULL)
    , m_io_cb(NULL)
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
                                        uint32_t size)
{
    LibScQemu *w = (LibScQemu *) opaque;

    return w->m_io_cb->qemu_io_read(addr, size);
}

void LibScQemu::qemu_sc_write(void *opaque, uint32_t addr,
                                     uint32_t val, uint32_t size)
{
    LibScQemu *w = (LibScQemu *) opaque;

    w->m_io_cb->qemu_io_write(addr, val, size);
}
/* ---------------------------- */

void LibScQemu::init(std::string libname, int num_cpu, std::string cpu_model)
{
    sc_qemu_init_struct s;
    sc_qemu_init_fn qemu_init = NULL;
    DynamicLoader &dyn = DynamicLoader::get();

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

    //assert(m_io_cb != NULL);

    s.sc_import.write = qemu_sc_write;
    s.sc_import.read = qemu_sc_read;

    s.q_import = m_qemu_import;
    s.cpu_model = cpu_model.c_str();
    s.num_cpu = num_cpu;
    s.opaque = this;
    s.max_run_time = m_insn_limit;
    s.cpu_mips_shift = m_mips_shift;

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

sc_qemu_qdev * LibScQemu::cpu_get_qdev(int cpu_idx)
{
    assert(m_qemu_import);
    return m_qemu_import->cpu_get_qdev(m_qemu_ctx, cpu_idx);
}

void LibScQemu::start_gdb_server(std::string port)
{
    assert(m_qemu_import);
    m_qemu_import->start_gdbserver(m_qemu_ctx, port.c_str());
}

/* 
 * QEMU devices 
 */
sc_qemu_qdev * LibScQemu::qdev_create_cortex_a15_priv(int num_int)
{
    assert(m_qemu_import);
    uint32_t i = num_int;
    return m_qemu_import->qdev_create(m_qemu_ctx, SC_QDEV_ARM_A15PRIV, i);
}

sc_qemu_qdev * LibScQemu::qdev_create_arm_gic(int num_int)
{
    assert(m_qemu_import);
    uint32_t i = num_int;
    return m_qemu_import->qdev_create(m_qemu_ctx, SC_QDEV_ARM_GIC, i);
}

sc_qemu_qdev * LibScQemu::qdev_create_uart_16550(int reg_shift, int baudbase)
{
    assert(m_qemu_import);
    uint32_t r = reg_shift;
    uint32_t b = baudbase;
    return m_qemu_import->qdev_create(m_qemu_ctx, SC_QDEV_16550, r, b);
}

sc_qemu_qdev * LibScQemu::qdev_create_sp804()
{
    assert(m_qemu_import);
    return m_qemu_import->qdev_create(m_qemu_ctx, SC_QDEV_SP804);
}

void LibScQemu::qdev_destroy(sc_qemu_qdev *dev)
{}

void LibScQemu::qdev_mmio_map(sc_qemu_qdev *dev, int mmio_id, uint32_t addr)
{
    assert(m_qemu_import);
    m_qemu_import->qdev_mmio_map(dev, mmio_id, addr);
}

void LibScQemu::qdev_irq_connect(sc_qemu_qdev *src, int src_idx,
                                        sc_qemu_qdev *dst, int dst_idx)
{
    assert(m_qemu_import);
    m_qemu_import->qdev_irq_connect(src, src_idx, dst, dst_idx);
}

void LibScQemu::qdev_irq_update(sc_qemu_qdev *dev, int irq_idx, int level)
{
    assert(m_qemu_import);
    m_qemu_import->qdev_irq_update(dev, irq_idx, level);
}

void LibScQemu::register_io_callback(qemu_io_callbacks &cb)
{
    m_io_cb = &cb;
}

sc_qemu_char_dev * LibScQemu::char_dev_create()
{
    assert(m_qemu_import);
    sc_qemu_char_dev * ret;
    ret = m_qemu_import->char_dev_create(m_qemu_ctx);

    return ret;
}

int LibScQemu::char_dev_write(sc_qemu_char_dev *dev, const uint8_t *data, int len)
{
    assert(m_qemu_import);
    return m_qemu_import->char_dev_write(dev, data, len);
}

void LibScQemu::char_dev_read(void *opaque, const uint8_t *data, int len)
{
    qemu_char_dev_callbacks *cb = (qemu_char_dev_callbacks *) opaque;

    cb->qemu_char_dev_read(data, len);
}

void LibScQemu::char_dev_register_callbacks(sc_qemu_char_dev *dev, qemu_char_dev_callbacks *cb)
{
    assert(m_qemu_import);
    m_qemu_import->char_dev_register_read(dev, char_dev_read, cb);
}
