/*
 * Copyright (c) 2011-2015 Toradex, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/arch/clock.h>
#include <asm/arch/funcmux.h>
#include <asm/arch/pinmux.h>
#include <asm/arch-tegra/ap.h>
#include <asm/arch-tegra/board.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include <i2c.h>
#include <nand.h>

#include "../common/configblock.h"

DECLARE_GLOBAL_DATA_PTR;

#define PMU_I2C_ADDRESS		0x34
#define MAX_I2C_RETRY		3
#define PMU_SUPPLYENE		0x14
#define PMU_SUPPLYENE_SYSINEN	(1<<5)
#define PMU_SUPPLYENE_EXITSLREQ	(1<<1)

u32 get_board_rev(void);

int arch_misc_init(void)
{
	/* Disable PMIC sleep mode on low supply voltage */
	struct udevice *dev;
	u8 addr, data[1];
	int err;

	err = i2c_get_chip_for_busnum(0, PMU_I2C_ADDRESS, 1, &dev);
	if (err) {
		debug("%s: Cannot find PMIC I2C chip\n", __func__);
		return err;
	}

	addr = PMU_SUPPLYENE;

	err = dm_i2c_read(dev, addr, data, 1);
	if (err) {
		debug("failed to get PMU_SUPPLYENE\n");
		return err;
	}

	data[0] &= ~PMU_SUPPLYENE_SYSINEN;
	data[0] |= PMU_SUPPLYENE_EXITSLREQ;

	err = dm_i2c_write(dev, addr, data, 1);
	if (err) {
		debug("failed to set PMU_SUPPLYENE\n");
		return err;
	}

	/* HW version */
	if (!getenv("hw-version")) {
		switch (get_board_rev()) {
		case 0x011b:
		case 0x011c:
			setenv("hw-version", "v11");
			break;
		case 0x012a:
			setenv("hw-version", "v12");
			break;
		default:
			setenv("hw-version", (nand_info[0].erasesize >> 10
					      == 512)?"v11":"v12");
		}
	}

	/* Default memory arguments */
	if (!getenv("memargs")) {
		switch (gd->ram_size) {
		case 0x10000000:
			/* 256 MB */
			setenv("memargs", "mem=148M@0M fbmem=12M@148M "
			       "nvmem=96M@160M");
			setenv("ram-size", "256");
			break;
		case 0x20000000:
			/* 512 MB */
			setenv("memargs", "mem=372M@0M fbmem=12M@372M "
			       "nvmem=128M@384M");
			setenv("ram-size", "512");
			break;
		default:
			printf("Failed detecting RAM size.\n");
		}
	}

	/* NAND parameters */
	if (!getenv("leb-size")) {
		switch (nand_info[0].erasesize >> 10) {
		case 256:
			/* 256 KiB */
			setenv("leb-size", "248KiB");
			break;
		case 512:
			/* 512 KiB */
			setenv("leb-size", "504KiB");
			break;
		default:
			printf("Failed detecting NAND block erase size.\n");
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
	printf("Model: Toradex Colibri T20 %dMB V%s\n",
	       (gd->ram_size == 0x10000000)?256:512, (nand_info[0].erasesize >> 10
		== 512)?((gd->ram_size == 0x10000000)?"1.1B":"1.1C"):"1.2A");

	return 0;
}

#ifdef CONFIG_TEGRA_MMC
/*
 * Routine: pin_mux_mmc
 * Description: setup the pin muxes/tristate values for the SDMMC(s)
 */
void pin_mux_mmc(void)
{
	funcmux_select(PERIPH_ID_SDMMC4, FUNCMUX_SDMMC4_ATB_GMA_4_BIT);
	pinmux_tristate_disable(PMUX_PINGRP_GMB);
}
#endif

#ifdef CONFIG_TEGRA_NAND
void pin_mux_nand(void)
{
	funcmux_select(PERIPH_ID_NDFLASH, FUNCMUX_NDFLASH_KBC_8_BIT);

	/*
	 * configure pingroup ATC to something unrelated to
	 * avoid ATC overriding KBC
	 */
	pinmux_set_func(PMUX_PINGRP_ATC, PMUX_FUNC_GMI);
}
#endif

#ifdef CONFIG_USB_EHCI_TEGRA
void pin_mux_usb(void)
{
	/* module internal USB bus to connect ethernet chipset */
	funcmux_select(PERIPH_ID_USB2, FUNCMUX_USB2_ULPI);

	/* ULPI reference clock output */
	pinmux_set_func(PMUX_PINGRP_CDEV2, PMUX_FUNC_PLLP_OUT4);
	pinmux_tristate_disable(PMUX_PINGRP_CDEV2);

	/* PHY reset GPIO */
	pinmux_tristate_disable(PMUX_PINGRP_UAC);

	/* VBus GPIO */
	pinmux_tristate_disable(PMUX_PINGRP_DTE);

	/* Reset ASIX using LAN_RESET */
	gpio_request(GPIO_PV4, "LAN_RESET");
	gpio_direction_output(GPIO_PV4, 0);
	pinmux_tristate_disable(PMUX_PINGRP_GPV);
	udelay(5);
	gpio_set_value(GPIO_PV4, 1);

	/* USBH_PEN: USB 1 aka Tegra USB port 3 VBus */
	pinmux_tristate_disable(PMUX_PINGRP_SPIG);
}
#endif

#ifdef CONFIG_VIDEO_TEGRA
/*
 * Routine: pin_mux_display
 * Description: setup the pin muxes/tristate values for the LCD interface)
 */
void pin_mux_display(void)
{
	/*
	 * Manually untristate BL_ON (PT4 - SODIMM 71) as specified through
	 * device-tree
	 */
	pinmux_tristate_disable(PMUX_PINGRP_DTA);

	pinmux_set_func(PMUX_PINGRP_SDC, PMUX_FUNC_PWM);
	pinmux_tristate_disable(PMUX_PINGRP_SDC);
}
#endif
