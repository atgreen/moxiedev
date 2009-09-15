/*
 *  MOXIE mmu emulation.
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
#include <stdlib.h>

#include "config.h"
#include "cpu.h"
#include "mmu.h"
#include "exec-all.h"

int moxie_mmu_translate(struct moxie_mmu_result_t *res,
		       CPUState *env, uint32_t vaddr,
		       int rw, int mmu_idx)
{
  if (vaddr >= 0x1000)
    res->phy = vaddr + 0x80000000;
  else
    {
      res->phy = vaddr;
    }
  return 0;
}
