/*
 * Generic GPIO API implementation for PowerPC.
 *
 * Copyright (c) 2009 Anthony Green <moxielogic.com>
 * Copyright (c) 2007-2008  MontaVista Software, Inc.
 *
 * Author: Anton Vorontsov <avorontsov@ru.mvista.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef _ASM_MOXIE_GPIO_H
#define _ASM_MOXIE_GPIO_H

#include <linux/errno.h>

#ifndef CONFIG_GPIOLIB
extern int gpio_get_value(unsigned int gpio);
extern void gpio_set_value(unsigned int gpio, int value);
#endif

#include <asm-generic/gpio.h>

#ifdef CONFIG_GPIOLIB

/*
 * We don't (yet) implement inlined/rapid versions for on-chip gpios.
 * Just call gpiolib.
 */
static inline int gpio_get_value(unsigned int gpio)
{
	return __gpio_get_value(gpio);
}

static inline void gpio_set_value(unsigned int gpio, int value)
{
	__gpio_set_value(gpio, value);
}

static inline int gpio_cansleep(unsigned int gpio)
{
	return __gpio_cansleep(gpio);
}

/*
 * Not implemented, yet.
 */
static inline int gpio_to_irq(unsigned int gpio)
{
	return -ENOSYS;
}

static inline int irq_to_gpio(unsigned int irq)
{
	return -EINVAL;
}

#endif /* CONFIG_GPIOLIB */

static inline int gpio_request(unsigned gpio, const char *label)
{
	return -ENOSYS;
}

static inline void gpio_free(unsigned gpio)
{
	might_sleep();

	/* GPIO can never have been requested */
	WARN_ON(1);
}

static inline int gpio_direction_input(unsigned gpio)
{
	return -ENOSYS;
}

static inline int gpio_direction_output(unsigned gpio, int value)
{
	return -ENOSYS;
}

#endif /* _ASM_MOXIE_GPIO_H */
