/*
 * Cache control for Moxie cache memories
 *
 * Copyright (C) 2009 Anthony Green <green@moxielogic.com>
 * Copyright (C) 2007-2009 Michal Simek <monstr@monstr.eu>
 * Copyright (C) 2007-2009 PetaLogix
 * Copyright (C) 2007 John Williams <john.williams@petalogix.com>
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License. See the file COPYING in the main directory of this
 * archive for more details.
 */

#include <asm/cacheflush.h>
#include <linux/cache.h>
#include <asm/cpuinfo.h>

/* Exported functions */

void _enable_icache(void)
{
  /* Do nothing for now.  */
}

void _disable_icache(void)
{
  /* Do nothing for now.  */
}

void _invalidate_icache(unsigned int addr)
{
  /* Do nothing for now.  */
}

void _enable_dcache(void)
{
  /* Do nothing for now.  */
}

void _disable_dcache(void)
{
  /* Do nothing for now.  */
}

void _invalidate_dcache(unsigned int addr)
{
  /* Do nothing for now.  */
}

void __invalidate_icache_all(void)
{
  /* Do nothing for now.  */
}

void __invalidate_icache_range(unsigned long start, unsigned long end)
{
  /* Do nothing for now.  */
}

void __invalidate_icache_page(struct vm_area_struct *vma, struct page *page)
{
	__invalidate_icache_all();
}

void __invalidate_icache_user_range(struct vm_area_struct *vma,
				struct page *page, unsigned long adr,
				int len)
{
	__invalidate_icache_all();
}

void __invalidate_cache_sigtramp(unsigned long addr)
{
	__invalidate_icache_range(addr, addr + 8);
}

void __invalidate_dcache_all(void)
{
	unsigned int i;
	unsigned flags;

	if (cpuinfo.use_dcache) {
		local_irq_save(flags);
		__disable_dcache();

		/*
		 * Just loop through cache size and invalidate,
		 * no need to add CACHE_BASE address
		 */
		for (i = 0; i < cpuinfo.dcache_size;
			i += cpuinfo.dcache_line)
				__invalidate_dcache(i);

		__enable_dcache();
		local_irq_restore(flags);
	}
}

void __invalidate_dcache_range(unsigned long start, unsigned long end)
{
	unsigned int i;
	unsigned flags;
	unsigned int align;

	if (cpuinfo.use_dcache) {
		/*
		 * No need to cover entire cache range,
		 * just cover cache footprint
		 */
		end = min(start + cpuinfo.dcache_size, end);
		align = ~(cpuinfo.dcache_line - 1);
		start &= align; /* Make sure we are aligned */
		/* Push end up to the next cache line */
		end = ((end & align) + cpuinfo.dcache_line);
		local_irq_save(flags);
		__disable_dcache();

		for (i = start; i < end; i += cpuinfo.dcache_line)
			__invalidate_dcache(i);

		__enable_dcache();
		local_irq_restore(flags);
	}
}

void __invalidate_dcache_page(struct vm_area_struct *vma, struct page *page)
{
	__invalidate_dcache_all();
}

void __invalidate_dcache_user_range(struct vm_area_struct *vma,
				struct page *page, unsigned long adr,
				int len)
{
	__invalidate_dcache_all();
}
