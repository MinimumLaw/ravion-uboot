/*
 * Copyright (C) 2010-2013 Freescale Semiconductor, Inc.
 * Copyright (C) 2013, Boundary Devices <info@boundarydevices.com>
 * Copyright (C) 2014-2016, Toradex AG
 * copied from nitrogen6x
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <dm.h>
#include <asm/arch/clock.h>
#include <asm/arch/crm_regs.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/iomux.h>
#include <asm/arch/mx6-pins.h>
#include <asm/arch/mx6-ddr.h>
#include <asm/arch/mxc_hdmi.h>
#include <asm/arch/sys_proto.h>
#include <asm/bootm.h>
#include <asm/gpio.h>
#include <asm/mach-imx/iomux-v3.h>
#include <asm/mach-imx/boot_mode.h>
#include <asm/mach-imx/video.h>
#include <asm/io.h>
#include <dm/platdata.h>
#include <input.h>
#include <imx_thermal.h>
#include <linux/errno.h>
#include <linux/libfdt.h>
#include <malloc.h>
#include <micrel.h>
#include <miiphy.h>
#include <netdev.h>
#include <usb.h>
#include <spl.h>

#include "ravion_qp_2048.h"
#include "ravion_dl_2048.h"

#define KITSBIMX6_BUILD

void reset_cpu(ulong addr)
{
}

#define UART_PAD_CTRL  (PAD_CTL_PUS_100K_UP |	\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm | \
	PAD_CTL_SRE_FAST  | PAD_CTL_HYS)

static iomux_v3_cfg_t const ravion_pads[] = {
	/* UART */
	MX6_PAD_CSI0_DAT10__UART1_TX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX6_PAD_CSI0_DAT11__UART1_RX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL),
	/* PWRBTN */
};

int board_early_init_f(void)
{
	SETUP_IOMUX_PADS(ravion_pads);
	return 0;
}

#ifdef CONFIG_SPL_OS_BOOT
#define SPL_LOAD_SYSTEM	0
#define SPL_LOAD_UBOOT	1

/* called BEFORE load system */
void spl_board_prepare_for_linux(void)
{
	printf("T-50 mode: load system...\n");
}

int spl_start_uboot(void)
{
	struct gpio_desc pwrbtn_gpio;
	int ret = SPL_LOAD_UBOOT;

#ifdef CONFIG_SPL_ENV_SUPPORT
	env_init();
	env_load();
	if (env_get_yesno("boot_os") != 1) {
		printf("Enverooment disable T50 boot mode!\n");
		return SPL_LOAD_UBOOT;
	}
#endif

#ifdef KITSBIMX6_BUILD
	ret = dm_gpio_lookup_name("GPIO2_8", &pwrbtn_gpio);
	if (ret) {
		printf("GPIO2_8 lookup failed (%d)!\n", ret);
		return SPL_LOAD_UBOOT;
	}
#else
	ret = dm_gpio_lookup_name("GPIO2_7", &pwrbtn_gpio);
	if (ret) {
		printf("GPIO2_7 lookup failed (%d)!\n", ret);
		return SPL_LOAD_UBOOT;
	}
#endif
	ret = dm_gpio_request(&pwrbtn_gpio, "PWRBTN");
	if(ret) {
		printf("PWRBTN request failed (%d)!\n", ret);
		return SPL_LOAD_UBOOT;
	};

	dm_gpio_set_dir_flags(&pwrbtn_gpio, GPIOD_IS_IN);

	ret = dm_gpio_get_value(&pwrbtn_gpio);
	if(ret < 0) {
		printf("PWRBTN get value failed (%d)!\n", ret);
		return SPL_LOAD_UBOOT;
	};

	/* Power button pulled UP, unpressed read "1", pressed "0" */
	ret = ret ? SPL_LOAD_SYSTEM : SPL_LOAD_UBOOT;
	printf(ret == SPL_LOAD_SYSTEM ? "Nice, T-50 mode started...\n" : "OK, tortoise mode started...\n");

	return !!ret;
}
#endif /* CONFIG_SPL_OS_BOOT*/

void board_boot_order(u32 *spl_boot_list)
{
	spl_boot_list[0] = BOOT_DEVICE_USB;
	spl_boot_list[1] = BOOT_DEVICE_MMC1;
	spl_boot_list[2] = BOOT_DEVICE_BOARD;
	spl_boot_list[3] = BOOT_DEVICE_NONE;
}

static void ccgr_init(void)
{
	struct mxc_ccm_reg *ccm = (struct mxc_ccm_reg *)CCM_BASE_ADDR;
#warning Only really required clocks must be enabled here
	writel(0xFFFFFFFF, &ccm->CCGR0);
	writel(0xFFFFFFFF, &ccm->CCGR1);
	writel(0xFFFFFFFF, &ccm->CCGR2);
	writel(0xFFFFFFFF, &ccm->CCGR3);
	writel(0xFFFFFFFF, &ccm->CCGR4);
	writel(0xFFFFFFFF, &ccm->CCGR5);
	writel(0xFFFFFFFF, &ccm->CCGR6);
}

static int imx6qp_2048_dcd[] = RAVION_QP_2048;
static int imx6dl_2048_dcd[] = RAVION_DL_2048;

static void ddr_init(int *table, int size) 
{
	int i;

	for (i = 0; i < size / 2 ; i++)
		writel(table[2 * i + 1], table[2 * i]);
}

static void spl_dram_init(void)
{
	if (is_mx6dqp())
		ddr_init(imx6qp_2048_dcd, ARRAY_SIZE(imx6qp_2048_dcd));
	else if (is_mx6dq())
		ddr_init(imx6dl_2048_dcd, ARRAY_SIZE(imx6qp_2048_dcd));
}

void board_init_f(ulong dummy)
{
	/* DDR initialization */
	spl_dram_init();

	/* setup AIPS and disable watchdog */
	arch_cpu_init();

	ccgr_init();
	gpr_init();

	/* iomux and setup of i2c */
	board_early_init_f();

	/* setup GP timer */
	timer_init();

	/* UART clocks enabled and gd valid - init serial console */
	preloader_console_init();

	/* Clear the BSS. */
	memset(__bss_start, 0, __bss_end - __bss_start);

	/* load/boot image from boot device */
	board_init_r(NULL, 0);
}
