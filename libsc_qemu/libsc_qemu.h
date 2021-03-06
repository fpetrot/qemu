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
#include <unordered_map>

#include <inttypes.h>

struct qemu_import;
struct qemu_context;
struct sc_qemu_object;
struct sc_qemu_io_attr;

class DynLib;
class ConfigManager;

class qemu_io_callbacks {
public:
    virtual uint32_t qemu_io_read(uint32_t addr, uint32_t size) = 0;
    virtual void qemu_io_write(uint32_t addr, uint32_t val, uint32_t size) = 0;
};

class qemu_object_gpio_callbacks {
public:
    virtual void qemu_object_gpio_event(sc_qemu_object *obj, int idx, int level) = 0;
};

class QemuObject;

class LibScQemu {
    friend class QemuObject;

private:
    int m_insn_limit = 0;
    int m_mips_shift = 0;
    bool m_map_whole_as = false;

    ConfigManager &m_config;
    qemu_context *m_qemu_ctx = nullptr;
    qemu_import *m_qemu_import = nullptr;
    DynLib *m_lib = nullptr;
    QemuObject *m_root_mr = nullptr;

    qemu_io_callbacks *m_io_cb = nullptr;
    std::unordered_map<int, qemu_io_callbacks*> m_io_cbs;

    /* Callbacks from sc-qemu lib */
    static uint32_t qemu_sc_read(void *opaque, uint32_t addr,
                                 uint32_t size, const sc_qemu_io_attr *attr);

    static void qemu_sc_write(void *opaque, uint32_t addr,
                              uint32_t val, uint32_t size,
                              const sc_qemu_io_attr *attr);

    static void object_gpio_event(sc_qemu_object *obj, int n, int level, void *opaque);

public:
    LibScQemu(ConfigManager &config);
    ~LibScQemu();

    /* Init data */
    void set_insn_limit(int insn_limit) { m_insn_limit = insn_limit; }
    void set_mips_shift(int mips_shift) { m_mips_shift = mips_shift; }
    void set_map_whole_as(bool map_whole_as) { m_map_whole_as = map_whole_as; }

    void init(std::string lib_path);
    bool is_inited() const { return m_lib != NULL; }

    void map_io(uint32_t base, uint32_t size);
    void map_dmi(uint32_t base, uint32_t size, void *data, bool readonly = false);

    bool cpus_loop(int64_t* elapsed, bool *has_work);

    /* QEMU GDB stub
     * @port: port the gdb server will be listening on. (ex: "tcp::1234") */
    void start_gdb_server(std::string port);

    void register_io_callback(qemu_io_callbacks &, int cpuid);

    QemuObject * object_new(const char *type_name);
    QemuObject * object_get_root_mr();
};


struct QemuGpio {
    const char * name = nullptr;
    int idx;

    QemuGpio(const char * name, int idx) : name(name), idx(idx) {}
    QemuGpio(int idx) : idx(idx) {}
};

class QemuObject {
private:
    sc_qemu_object *m_obj;
    qemu_import &m_import;

    bool m_realized = false;

public:
    QemuObject(sc_qemu_object *obj, qemu_import &import)
        : m_obj(obj)
        , m_import(import)
    {}

    virtual ~QemuObject() {}

    void realize();

    void set_prop_bool(const char *name, bool val);
    void set_prop_int(const char *name, int64_t val);
    void set_prop_str(const char *name, const char *val);
    void set_prop_link(const char *name, QemuObject *link);

    int get_cpu_id();

    void mmio_map(int mmio_id, uint64_t addr);
    void gpio_out_connect(const QemuGpio &gpio_out, QemuObject &in, const QemuGpio &gpio_in);
    void gpio_in_set(const QemuGpio &gpio, int level);
    void gpio_out_register_cb(const QemuGpio &gpio, qemu_object_gpio_callbacks *cb);
};

#endif
