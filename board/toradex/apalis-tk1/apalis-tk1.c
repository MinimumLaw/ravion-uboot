/*
 * Copyright (c) 2012-2016 Toradex, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/arch-tegra/ap.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include <netdev.h>
#include <power/as3722.h>

#include <asm/arch/gpio.h>
#include <asm/arch/pinmux.h>

#include "pinmux-config-apalis-tk1.h"

#define LAN_RESET_N GPIO_PS2

DECLARE_GLOBAL_DATA_PTR;

int arch_misc_init(void)
{
	if (readl(NV_PA_BASE_SRAM + NVBOOTINFOTABLE_BOOTTYPE) ==
	    NVBOOTTYPE_RECOVERY) {
		printf("USB recovery mode, disabled autoboot\n");
		setenv("bootdelay", "-1");
	}

	return 0;
}

int checkboard_fallback(void)
{
	printf("Model: Toradex Apalis TK1 2GB\n");

	return 0;
}

/*
 * Routine: pinmux_init
 * Description: Do individual peripheral pinmux configs
 */
void pinmux_init(void)
{
	pinmux_clear_tristate_input_clamping();

	gpio_config_table(apalis_tk1_gpio_inits,
			  ARRAY_SIZE(apalis_tk1_gpio_inits));

	pinmux_config_pingrp_table(apalis_tk1_pingrps,
				   ARRAY_SIZE(apalis_tk1_pingrps));

	pinmux_config_drvgrp_table(apalis_tk1_drvgrps,
				   ARRAY_SIZE(apalis_tk1_drvgrps));
}

#ifdef CONFIG_PCI_TEGRA
int tegra_pcie_board_init(void)
{
	struct udevice *pmic;
	int err;

	err = as3722_init(&pmic);
	if (err) {
		error("failed to initialize AS3722 PMIC: %d\n", err);
		return err;
	}

	err = as3722_sd_enable(pmic, 4);
	if (err < 0) {
		error("failed to enable SD4: %d\n", err);
		return err;
	}

	err = as3722_sd_set_voltage(pmic, 4, 0x24);
	if (err < 0) {
		error("failed to set SD4 voltage: %d\n", err);
		return err;
	}

	err = as3722_gpio_configure(pmic, 1, AS3722_GPIO_OUTPUT_VDDH |
					     AS3722_GPIO_INVERT);
	if (err < 0) {
		error("failed to configure GPIO#1 as output: %d\n", err);
		return err;
	}

	err = as3722_gpio_direction_output(pmic, 2, 1);
	if (err < 0) {
		error("failed to set GPIO#2 high: %d\n", err);
		return err;
	}

	/* Reset I210 Gigabit Ethernet Controller */
	gpio_request(LAN_RESET_N, "LAN_RESET_N");
	gpio_direction_output(LAN_RESET_N, 0);

	/*
	 * Make sure we don't get any back feeding from LAN_WAKE_N resp.
	 * DEV_OFF_N
	 */
	gpio_request(GPIO_PO5, "LAN_WAKE_N");
	gpio_direction_output(GPIO_PO5, 0);

	gpio_request(GPIO_PO6, "LAN_DEV_OFF_N");
	gpio_direction_output(GPIO_PO6, 0);

	/* Make sure LDO9 and LDO10 are initially enabled @ 0V */
	err = as3722_ldo_enable(pmic, 9);
	if (err < 0) {
		error("failed to enable LDO9: %d\n", err);
		return err;
	}
	err = as3722_ldo_enable(pmic, 10);
	if (err < 0) {
		error("failed to enable LDO10: %d\n", err);
		return err;
	}
	err = as3722_ldo_set_voltage(pmic, 9, 0x80);
	if (err < 0) {
		error("failed to set LDO9 voltage: %d\n", err);
		return err;
	}
	err = as3722_ldo_set_voltage(pmic, 10, 0x80);
	if (err < 0) {
		error("failed to set LDO10 voltage: %d\n", err);
		return err;
	}

	mdelay(100);

	/* Make sure controller gets enabled by disabling DEV_OFF_N */
	gpio_set_value(GPIO_PO6, 1);

	/* Enable LDO9 and LDO10 for +V3.3_ETH on patched prototypes */
	err = as3722_ldo_set_voltage(pmic, 9, 0xff);
	if (err < 0) {
		error("failed to set LDO9 voltage: %d\n", err);
		return err;
	}
	err = as3722_ldo_set_voltage(pmic, 10, 0xff);
	if (err < 0) {
		error("failed to set LDO10 voltage: %d\n", err);
		return err;
	}

	mdelay(100);
	gpio_set_value(LAN_RESET_N, 1);

	return 0;
}

int board_eth_init(bd_t *bis)
{
	return pci_eth_init(bis);
}
#endif /* CONFIG_PCI_TEGRA */
