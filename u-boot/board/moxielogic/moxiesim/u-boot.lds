/*
 * Copyrigth (c) 2007,2008
 * Nobuhiro Iwamatsu <iwamatsu@nigauri.org>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

OUTPUT_FORMAT("elf32-moxie", "elf32-moxie", "elf32-moxie")
OUTPUT_ARCH(moxie)
ENTRY(_start)

SECTIONS
{
	/*
	   Base address of internal SDRAM is 0x1000.
	*/
	. = 0x00001000;

	PROVIDE (reloc_dst = .);

	PROVIDE (_ftext = .);
	PROVIDE (_fcode = .);
	PROVIDE (_start = .);

	.text :
	{
		cpu/moxie/start.o		(.text)
		. = ALIGN(8192);
		*(.text)
		. = ALIGN(4);
	} =0xFF
	PROVIDE (_ecode = .);
	.rodata :
	{
		*(.rodata)
		. = ALIGN(4);
	}
	PROVIDE (_etext = .);


	PROVIDE (_fdata = .);
	.data :
	{
		*(.data)
		. = ALIGN(4);
	}
	PROVIDE (_edata = .);

	PROVIDE (_fgot = .);
	.got :
	{
		*(.got)
		. = ALIGN(4);
	}
	PROVIDE (_egot = .);

	PROVIDE (__u_boot_cmd_start = .);
	.u_boot_cmd :
	{
		*(.u_boot_cmd)
		. = ALIGN(4);
	}
	PROVIDE (__u_boot_cmd_end = .);

	PROVIDE (reloc_dst_end = .);
	/* _reloc_dst_end = .; */

	PROVIDE (bss_start = .);
	PROVIDE (__bss_start = .);
	.bss :
	{
		*(.bss)
		. = ALIGN(4);
	}
	PROVIDE (bss_end = .);

	PROVIDE (_end = .);

	.stack 0x80000 : { _stack = .; *(.stack) }
}
