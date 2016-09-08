/*
 * Copyright (C) 2015
 * Bhuvanchandra DV, Toradex, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <dm.h>
#include <errno.h>
#include <fdtdec.h>
#include <asm/gpio.h>
#include <asm/imx-common/iomux-v3.h>
#include <asm/io.h>
#include <malloc.h>

DECLARE_GLOBAL_DATA_PTR;

struct vybrid_gpios {
	unsigned int chip;
	struct vybrid_gpio_regs *reg;
};

static int vybrid_gpio_direction_input(struct udevice *dev, unsigned gpio)
{
	const struct vybrid_gpios *gpios = dev_get_priv(dev);

	gpio = gpio + (gpios->chip * VYBRID_GPIO_COUNT);
	imx_iomux_gpio_set_direction(gpio, VF610_GPIO_DIRECTION_IN);

	return 0;
}

static int vybrid_gpio_direction_output(struct udevice *dev, unsigned gpio,
					 int value)
{
	const struct vybrid_gpios *gpios = dev_get_priv(dev);

	gpio = gpio + (gpios->chip * VYBRID_GPIO_COUNT);
	gpio_set_value(gpio, value);
	imx_iomux_gpio_set_direction(gpio, VF610_GPIO_DIRECTION_OUT);

	return 0;
}

static int vybrid_gpio_get_value(struct udevice *dev, unsigned gpio)
{
	const struct vybrid_gpios *gpios = dev_get_priv(dev);

	return ((readl(&gpios->reg->gpio_pdir) & (1 << gpio))) ? 1 : 0;
}

static int vybrid_gpio_set_value(struct udevice *dev, unsigned gpio,
				  int value)
{
	const struct vybrid_gpios *gpios = dev_get_priv(dev);
	if (value)
		writel((1 << gpio), &gpios->reg->gpio_psor);
	else
		writel((1 << gpio), &gpios->reg->gpio_pcor);

	return 0;
}

static const struct dm_gpio_ops gpio_vybrid_ops = {
	.direction_input	= vybrid_gpio_direction_input,
	.direction_output	= vybrid_gpio_direction_output,
	.get_value		= vybrid_gpio_get_value,
	.set_value		= vybrid_gpio_set_value,
};

static int vybrid_gpio_probe(struct udevice *dev)
{
	struct vybrid_gpios *gpios = dev_get_priv(dev);
	struct vybrid_gpio_platdata *plat = dev_get_platdata(dev);
	struct gpio_dev_priv *uc_priv = dev->uclass_priv;

	uc_priv->bank_name = "GPIO";
	uc_priv->gpio_count = VYBRID_GPIO_COUNT;
	gpios->reg = (struct vybrid_gpio_regs *)plat->base;
	gpios->chip = plat->chip;

	return 0;
}

static int vybrid_gpio_bind(struct udevice *dev)
{
	struct vybrid_gpio_platdata *plat = dev->platdata;
	fdt_addr_t base_addr;

	if (plat)
		return 0;

	base_addr = dev_get_addr(dev);
	if (base_addr == FDT_ADDR_T_NONE)
		return -ENODEV;

	plat = calloc(1, sizeof(*plat));
	if (!plat)
		return -ENOMEM;

	plat->base = base_addr;
	plat->chip = dev->req_seq;
	dev->platdata = plat;

	return 0;
}

static const struct udevice_id vybrid_gpio_ids[] = {
	{ .compatible = "fsl,vf610-gpio" },
	{ }
};

U_BOOT_DRIVER(gpio_vybrid) = {
	.name	= "gpio_vybrid",
	.id	= UCLASS_GPIO,
	.ops	= &gpio_vybrid_ops,
	.probe	= vybrid_gpio_probe,
	.priv_auto_alloc_size = sizeof(struct vybrid_gpios),
	.of_match = vybrid_gpio_ids,
	.bind	= vybrid_gpio_bind,
};
