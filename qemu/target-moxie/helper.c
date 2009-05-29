/*
 *  MOXIE helper routines.
 *
 *  Copyright (c) 2008 Anthony Green
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include <string.h>

#include "config.h"
#include "cpu.h"
#include "mmu.h"
#include "exec-all.h"
#include "host-utils.h"

#if defined(CONFIG_USER_ONLY)

void do_interrupt (CPUState *env)
{
  env->exception_index = -1;
}

int cpu_moxie_handle_mmu_fault(CPUState * env, target_ulong address, int rw,
                             int mmu_idx, int is_softmmu)
{
    env->exception_index = 0xaa;
    env->debug1 = address;
    cpu_dump_state(env, stderr, fprintf, 0);
    printf("%s addr=%x env->pc=%x\n", __func__, address, env->pc);
    return 1;
}

target_phys_addr_t cpu_get_phys_page_debug(CPUState * env, target_ulong addr)
{
    return addr;
}

#else /* !CONFIG_USER_ONLY */

int cpu_moxie_handle_mmu_fault (CPUState *env, target_ulong address, int rw,
                               int mmu_idx, int is_softmmu)
{
	struct moxie_mmu_result_t res;
	int prot, miss;
	target_ulong phy;

	address &= TARGET_PAGE_MASK;
	prot = PAGE_READ | PAGE_WRITE | PAGE_EXEC;
	//	printf ("%s pc=%x %x w=%d smmu=%d\n", __func__, env->pc, address, rw, is_softmmu);
	miss = moxie_mmu_translate(&res, env, address, rw, mmu_idx);
	if (miss)
	{
		/* handle the miss.  */
		phy = 0;
		env->exception_index = EXCP_MMU_MISS;
	}
	else
	{
		phy = res.phy;
	}
	return tlb_set_page(env, address, phy, prot, mmu_idx, is_softmmu);
}


void do_interrupt(CPUState *env)
{
	uint32_t ebp, isr;
	int irqnum;

	fflush(NULL);

#if 0
	printf ("exception index=%d interrupt_req=%d\n",
		env->exception_index,
		env->interrupt_request);
#endif

	switch (env->exception_index)
	{
		case EXCP_BREAK:
		  printf ("BREAK!\n");
		  break;

		case EXCP_MMU_MISS:
		  printf ("MMU miss\n");
		  irqnum = 4;
		  isr = ldl_code(ebp + irqnum * 4);
		  env->pc = isr;
		  break;

		default:
		{
		  printf ("%s ebp=%x %x isr=%x %d"
			  " ir=%x\n",
			  __func__,
			  ebp, ebp + irqnum * 4,
			  isr, env->exception_index,
			  env->interrupt_request);
		}
		break;
	}
}

target_phys_addr_t cpu_get_phys_page_debug(CPUState * env, target_ulong addr)
{
	uint32_t phy = addr;
	struct moxie_mmu_result_t res;
	int miss;
	miss = moxie_mmu_translate(&res, env, addr, 0, 0);
	if (!miss)
		phy = res.phy;
	return phy;
}
#endif
