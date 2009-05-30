/*
 * QEMU/moxiesim emulation
 *
 * Emulates a very simple machine model similiar to the one use by the
 * GDB moxie simulator.
 * 
 * Copyright (c) 2008 Anthony Green
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "hw.h"
#include "pc.h"
#include "isa.h"
#include "net.h"
#include "sysemu.h"
#include "boards.h"

#define PHYS_MEM_BASE 0x80000000

static struct _loaderparams {
    int ram_size;
    const char *kernel_filename;
    const char *kernel_cmdline;
    const char *initrd_filename;
} loaderparams;

static void load_kernel (CPUState *env)
{
    uint64_t entry, kernel_low, kernel_high;
    long kernel_size;
    long initrd_size;
    ram_addr_t initrd_offset;

    kernel_size = load_elf(loaderparams.kernel_filename,  PHYS_MEM_BASE,
                           &entry, &kernel_low, &kernel_high);
    if (kernel_size >= 0)
      env->pc = entry;
    else 
      {
        fprintf(stderr, "qemu: could not load kernel '%s'\n",
                loaderparams.kernel_filename);
        exit(1);
      }

    /* load initrd */
    initrd_size = 0;
    initrd_offset = 0;
    if (loaderparams.initrd_filename) {
        initrd_size = get_image_size (loaderparams.initrd_filename);
        if (initrd_size > 0) {
            initrd_offset = (kernel_high + ~TARGET_PAGE_MASK) & TARGET_PAGE_MASK;
            if (initrd_offset + initrd_size > loaderparams.ram_size) {
                fprintf(stderr,
                        "qemu: memory too small for initial ram disk '%s'\n",
                        loaderparams.initrd_filename);
                exit(1);
            }
            initrd_size = load_image_targphys(loaderparams.initrd_filename,
					      initrd_offset,
					      ram_size);
        }
        if (initrd_size == (target_ulong) -1) {
            fprintf(stderr, "qemu: could not load initial ram disk '%s'\n",
                    loaderparams.initrd_filename);
            exit(1);
        }
    }
}

static void main_cpu_reset(void *opaque)
{
    CPUState *env = opaque;
    cpu_reset(env);

    if (loaderparams.kernel_filename)
        load_kernel (env);
}

static void
moxiesim_init (int ram_size,
	       const char *boot_device,
	       const char *kernel_filename, const char *kernel_cmdline,
	       const char *initrd_filename, const char *cpu_model)
{
    CPUState *env;
    ram_addr_t phys_ram;

    /* Init CPUs. */
    if (cpu_model == NULL) {
        cpu_model = "moxie1";
    }
    env = cpu_init(cpu_model);
    if (!env) {
        fprintf(stderr, "Unable to find CPU definition\n");
        exit(1);
    }
    register_savevm("cpu", 0, 3, cpu_save, cpu_load, env);
    qemu_register_reset(main_cpu_reset, 0, env);

    /* Allocate RAM. */
    phys_ram = qemu_ram_alloc(ram_size);
    cpu_register_physical_memory(PHYS_MEM_BASE, ram_size, phys_ram | IO_MEM_RAM);

    if (kernel_filename) {
        loaderparams.ram_size = ram_size;
        loaderparams.kernel_filename = kernel_filename;
        loaderparams.kernel_cmdline = kernel_cmdline;
        loaderparams.initrd_filename = initrd_filename;
        load_kernel(env);
    }
    /* A single 16450 sits at offset 0x3f8.  */
    if (serial_hds[0])
      serial_mm_init(0x3f8, 0, env->irq[4], 8000000/16, serial_hds[0], 1);
    
    rtc_mm_init(0x400, 0, 0, 0);
}

void DMA_run(void)
{
}

void pic_info(Monitor *mon)
{
}

void irq_info(Monitor *mon)
{
}

QEMUMachine moxiesim_machine = {
    .name = "moxiesim",
    .desc = "Moxie simulator platform",
    .init = moxiesim_init,
    .is_default = 1,
};

static void moxie_machine_init(void)
{
    qemu_register_machine(&moxiesim_machine);
}

machine_init(moxie_machine_init);
