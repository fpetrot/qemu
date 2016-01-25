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

#ifndef _QEMU_WAPPER_QEMU_LIB_WRAPPER_H
#define _QEMU_WAPPER_QEMU_LIB_WRAPPER_H

#include <string>

/* XXX rabbits.h ? */
#include <inttypes.h>

struct qemu_import;
struct qemu_context;
struct sc_qemu_qdev;
struct sc_qemu_char_dev;

class DynLib;

class qemu_io_callbacks {
public:
    virtual uint32_t qemu_io_read(uint32_t addr, uint32_t size) = 0;
    virtual void qemu_io_write(uint32_t addr, uint32_t val, uint32_t size) = 0;
};

class qemu_char_dev_callbacks {
public:
    virtual void qemu_char_dev_read(const uint8_t *data, int len) = 0;
};

class LibScQemu {
private:
    int m_insn_limit;
    int m_mips_shift;

    qemu_context *m_qemu_ctx;
    qemu_import *m_qemu_import;
    DynLib *m_lib;

    qemu_io_callbacks *m_io_cb;

    /* Callbacks from sc-qemu lib */
    static uint32_t qemu_sc_read(void *opaque, uint32_t addr, uint32_t size);
    static void qemu_sc_write(void *opaque, uint32_t addr,
                              uint32_t val, uint32_t size);
    static void char_dev_read(void *opaque, const uint8_t *data, int len);

public:
    LibScQemu();
    ~LibScQemu();

    /* Init data */
    void set_insn_limit(int insn_limit) { m_insn_limit = insn_limit; }
    void set_mips_shift(int mips_shift) { m_mips_shift = mips_shift; }

    void init(std::string lib_path, int num_cpu, std::string cpu_model);
    bool is_inited() const { return m_lib != NULL; }

    void map_io(uint32_t base, uint32_t size);
    void map_dmi(uint32_t base, uint32_t size, void *data);

    bool cpus_loop(int64_t* elapsed);

    /* Return the qdev associated to CPU cpu_idx */
    sc_qemu_qdev * cpu_get_qdev(int cpu_idx);

    /* QEMU GDB stub
     * @port: port the gdb server will be listening on. (ex: "tcp::1234") */
    void start_gdb_server(std::string port);

    /* QEMU device creation and handling  */

    /* Cortex-A15 private devices. Contains a GIC (interrupt controller) and
     * generic timers.
     * @num_irq: Number of irq lines in the GIC that is part of A15 private devices */
    sc_qemu_qdev * qdev_create_cortex_a15_priv(int num_irq);

    /* ARM GIC interrupt controller
     * @num_irq: Number of irq lines in the GIC */
    sc_qemu_qdev * qdev_create_arm_gic(int num_irq);

    /* 16550 compatible UART
     * @reg_shift: right shift applied on register access (2 -> 1 reg every 4 bytes)
     * @baudbase:  main oscillator frequency */
    sc_qemu_qdev * qdev_create_uart_16550(int reg_shift, int baudbase);

    /* SP804 timer */
    sc_qemu_qdev * qdev_create_sp804();

    void qdev_destroy(sc_qemu_qdev *);
    void qdev_mmio_map(sc_qemu_qdev *, int mmio_id, uint32_t addr);
    void qdev_irq_connect(sc_qemu_qdev *src, int src_idx, sc_qemu_qdev *dst, int dst_idx);
    void qdev_irq_update(sc_qemu_qdev *, int irq_idx, int level);

    void register_io_callback(qemu_io_callbacks &);

    sc_qemu_char_dev *char_dev_create();
    int char_dev_write(sc_qemu_char_dev *, const uint8_t *data, int len);
    void char_dev_register_callbacks(sc_qemu_char_dev *, qemu_char_dev_callbacks *);

};

#endif
