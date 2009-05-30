/*
 * (C) Copyright 2009 Anthony Green
 * (C) Copyright 2007-2008 Michal Simek
 *
 * Anthony Green <green@moxielogic.com>
 * Michal SIMEK <monstr@monstr.eu>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#define	CONFIG_MOXIE	1	/* Moxie CPU */

/* uart */
/*
 * NS16550 Configuration
 */
#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL

#define CONFIG_CONS_INDEX	1
#define CONFIG_BAUDRATE		38400
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200, 230400 }

#define CONFIG_SYS_NS16550_REG_SIZE	1

#define CONFIG_SYS_NS16550_CLK		get_bus_freq(0)

#define CONFIG_SYS_NS16550_COM1	(0x3f8)

/*
 * memory layout - Example
 * TEXT_BASE = 0x1200_0000;
 * CONFIG_SYS_SRAM_BASE = 0x1000_0000;
 * CONFIG_SYS_SRAM_SIZE = 0x0400_0000;
 *
 * CONFIG_SYS_GBL_DATA_OFFSET = 0x1000_0000 + 0x0400_0000 - 0x1000 = 0x13FF_F000
 * CONFIG_SYS_MONITOR_BASE = 0x13FF_F000 - 0x40000 = 0x13FB_F000
 * CONFIG_SYS_MALLOC_BASE = 0x13FB_F000 - 0x40000 = 0x13F7_F000
 *
 * 0x1000_0000	CONFIG_SYS_SDRAM_BASE
 *					FREE
 * 0x1200_0000	TEXT_BASE
 *		U-BOOT code
 * 0x1202_0000
 *					FREE
 *
 *					STACK
 * 0x13F7_F000	CONFIG_SYS_MALLOC_BASE
 *					MALLOC_AREA	256kB	Alloc
 * 0x11FB_F000	CONFIG_SYS_MONITOR_BASE
 *					MONITOR_CODE	256kB	Env
 * 0x13FF_F000	CONFIG_SYS_GBL_DATA_OFFSET
 *					GLOBAL_DATA	4kB	bd, gd
 * 0x1400_0000	CONFIG_SYS_SDRAM_BASE + CONFIG_SYS_SDRAM_SIZE
 */

#define TEXT_BASE 0x00001000

/* ddr sdram - main memory */
#define	CONFIG_SYS_SDRAM_BASE		0x00001000
#define	CONFIG_SYS_SDRAM_SIZE		(2*1024*1024)
#define	CONFIG_SYS_MEMTEST_START	CONFIG_SYS_SDRAM_BASE
#define	CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_SDRAM_BASE + 0x1000)

/* global pointer */
#define	CONFIG_SYS_GBL_DATA_SIZE	0x1000	/* size of global data */
/* start of global data */
#define	CONFIG_SYS_GBL_DATA_OFFSET	(CONFIG_SYS_SDRAM_BASE + CONFIG_SYS_SDRAM_SIZE - CONFIG_SYS_GBL_DATA_SIZE)

/* monitor code */
#define	SIZE			0x40000
#define	CONFIG_SYS_MONITOR_LEN		SIZE
#define	CONFIG_SYS_MONITOR_BASE	(CONFIG_SYS_GBL_DATA_OFFSET - CONFIG_SYS_MONITOR_LEN)
#define	CONFIG_SYS_MONITOR_END		(CONFIG_SYS_MONITOR_BASE + CONFIG_SYS_MONITOR_LEN)
#define	CONFIG_SYS_MALLOC_LEN		SIZE
#define	CONFIG_SYS_MALLOC_BASE		(CONFIG_SYS_MONITOR_BASE - CONFIG_SYS_MALLOC_LEN)

/* stack */
#define	CONFIG_SYS_INIT_SP_OFFSET	CONFIG_SYS_MONITOR_BASE

/* ENV in RAM */
#define	CONFIG_SYS_NO_FLASH		1
#define	CONFIG_ENV_IS_NOWHERE	1
#define	CONFIG_ENV_SIZE		0x1000
#define	CONFIG_ENV_ADDR		(CONFIG_SYS_MONITOR_BASE - CONFIG_ENV_SIZE)
#define	CONFIG_SYS_FLASH_PROTECTION		/* hardware flash protection */

/*
 * Command line configuration.
 */
#include <config_cmd_default.h>

#define CONFIG_CMD_ASKENV
#define CONFIG_CMD_IRQ
#undef  CONFIG_CMD_MFSL
#define CONFIG_CMD_ECHO
#undef CONFIG_CMD_FLASH
#undef CONFIG_CMD_SAVEENV
#undef CONFIG_CMD_NET

/* Miscellaneous configurable options */
#define	CONFIG_SYS_PROMPT	"=> "
#define	CONFIG_SYS_CBSIZE	512	/* size of console buffer */
#define	CONFIG_SYS_PBSIZE	(CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16) /* print buffer size */
#define	CONFIG_SYS_MAXARGS	15	/* max number of command args */
#define	CONFIG_SYS_LONGHELP
#define	CONFIG_SYS_LOAD_ADDR	CONFIG_SYS_SDRAM_BASE /* default load address */

#define	CONFIG_BOOTDELAY	-1	/* -1 disables auto-boot */
#define	CONFIG_BOOTARGS		"root=romfs"
#define	CONFIG_HOSTNAME		"moxiesim"

/* architecture dependent code */
#define	CONFIG_SYS_USR_EXCEP	/* user exception */
#define CONFIG_SYS_HZ	1000

#define	CONFIG_PREBOOT		"echo U-BOOT for $(hostname);setenv preboot;echo"

#define	CONFIG_EXTRA_ENV_SETTINGS	

#define CONFIG_CMDLINE_EDITING

#endif	/* __CONFIG_H */
