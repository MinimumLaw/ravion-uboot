/*
 * Copyright (c) 2012-2015 Toradex, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/arch/gp_padctrl.h>
#include <asm/arch/pinmux.h>
#include <asm/arch-tegra/ap.h>
#include <asm/arch-tegra/tegra.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include <dm.h>
#include <i2c.h>
#include <netdev.h>

#include "pinmux-config-apalis_t30.h"
#include "../common/configblock.h"

DECLARE_GLOBAL_DATA_PTR;

#define PMU_I2C_ADDRESS		0x2D
#define MAX_I2C_RETRY		3

int arch_misc_init(void)
{
	/* Default memory arguments */
	if (!getenv("memargs")) {
		switch (gd->ram_size) {
		case 0x40000000:
			/* 1 GB */
			setenv("memargs", "vmalloc=128M mem=1012M@2048M "
					  "fbmem=12M@3060M");
			break;
		case 0x7ff00000:
		case 0x80000000:
			/* 2 GB */
			setenv("memargs", "vmalloc=256M mem=2035M@2048M "
					  "fbmem=12M@4083M");
			break;
		default:
			printf("Failed detecting RAM size.\n");
		}
	}

	if (readl(NV_PA_BASE_SRAM + NVBOOTINFOTABLE_BOOTTYPE) ==
	    NVBOOTTYPE_RECOVERY) {
		printf("USB recovery mode, disabled autoboot\n");
		setenv("bootdelay", "-1");
	}

	return 0;
}

int checkboard_fallback(void)
{
	printf("Model: Toradex Apalis T30 %dGB\n", (gd->ram_size == 0x40000000)?1:2);

	return 0;
}

/*
 * Routine: pinmux_init
 * Description: Do individual peripheral pinmux configs
 */
void pinmux_init(void)
{
	pinmux_config_pingrp_table(tegra3_pinmux_common,
				   ARRAY_SIZE(tegra3_pinmux_common));

	pinmux_config_pingrp_table(unused_pins_lowpower,
				   ARRAY_SIZE(unused_pins_lowpower));

	/* Initialize any non-default pad configs (APB_MISC_GP regs) */
	pinmux_config_drvgrp_table(apalis_t30_padctrl,
				   ARRAY_SIZE(apalis_t30_padctrl));
}

#ifdef CONFIG_PCI_TEGRA
int tegra_pcie_board_init(void)
{
	struct udevice *dev;
	u8 addr, data[1];
	int err;

	err = i2c_get_chip_for_busnum(0, PMU_I2C_ADDRESS, 1, &dev);
	if (err) {
		debug("%s: Cannot find PMIC I2C chip\n", __func__);
		return err;
	}

	/* TPS659110: VDD2_OP_REG = 1.05V */
	data[0] = 0x27;
	addr = 0x25;

	err = dm_i2c_write(dev, addr, data, 1);
	if (err) {
		debug("failed to set VDD supply\n");
		return err;
	}

	/* TPS659110: VDD2_REG 7.5 mV/us, ACTIVE */
	data[0] = 0x0D;
	addr = 0x24;

	err = dm_i2c_write(dev, addr, data, 1);
	if (err) {
		debug("failed to enable VDD supply\n");
		return err;
	}

	/* TPS659110: LDO6_REG = 1.1V, ACTIVE */
	data[0] = 0x0D;
	addr = 0x35;

	err = dm_i2c_write(dev, addr, data, 1);
	if (err) {
		debug("failed to set AVDD supply\n");
		return err;
	}

#ifdef APALIS_T30_PCIE_EVALBOARD_INIT
#define PEX_PERST_N     GPIO_PS7 /* Apalis GPIO7 */
#define RESET_MOCI_N    GPIO_PI4

	/* Reset PLX PEX 8605 PCIe Switch plus PCIe devices on Apalis Evaluation
	   Board */
	gpio_request(PEX_PERST_N, "PEX_PERST_N");
	gpio_request(RESET_MOCI_N, "RESET_MOCI_N");
	gpio_direction_output(PEX_PERST_N, 0);
	gpio_direction_output(RESET_MOCI_N, 0);
	/* Must be asserted for 100 ms after power and clocks are stable */
	mdelay(100);
	gpio_set_value(PEX_PERST_N, 1);
	/* Err_5: PEX_REFCLK_OUTpx/nx Clock Outputs is not Guaranteed Until
	   900 us After PEX_PERST# De-assertion */
	mdelay(1);
	gpio_set_value(RESET_MOCI_N, 1);
#endif /* APALIS_T30_PCIE_EVALBOARD_INIT */

	return 0;
}

int board_eth_init(bd_t *bis)
{
	return pci_eth_init(bis);
}
#endif /* CONFIG_PCI_TEGRA */
