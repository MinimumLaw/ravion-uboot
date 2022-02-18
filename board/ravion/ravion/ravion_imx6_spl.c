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
#include <init.h>
#include <imx_thermal.h>
#include <linux/delay.h>
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

#include "../common/rav-cfg-block.h"

#ifdef CONFIG_SPL_OS_BOOT
#define SPL_LOAD_SYSTEM	0
#define SPL_LOAD_UBOOT	1

/*
 *   Turn  device on  by pressing  power button,  folowed
 * by quick press and release them switch off FalconMode.
 *   Default  use 3/4sec (750mS or 750000uS) delay before
 * read power button state after system reinit.
 */
#ifndef PWRBTN_DETECT_DELAY
#define PWRBTN_DETECT_DELAY	750000
#endif
static const char POWERBUTTON_GPIO[] = "GPIO2_7";
static const char nRESETOUT_GPIO[] = "GPIO2_5";

/* called BEFORE load system */
void spl_board_prepare_for_linux(void)
{
    show_board_info();
    if (ft_common_board_setup((void *)CONFIG_SYS_SPL_ARGS_ADDR, NULL)) {
	printf("SPL: Device-tree fixup failed.\n");
    };
    debug("Falcon mode: load system...\n");
}

int spl_start_uboot(void)
{
	const char* pwr_gpio = POWERBUTTON_GPIO;
	const char* rst_gpio = nRESETOUT_GPIO;
	struct gpio_desc pwrbtn_gpio;
	struct gpio_desc nrstout_gpio;
	int ret = SPL_LOAD_UBOOT;
	int v;

	if (!!(v=dm_gpio_lookup_name(rst_gpio, &nrstout_gpio)))
		printf("nRESET lookup failed! (err=%d)\n", v);
	if (!!(v=dm_gpio_request(&nrstout_gpio, "nRSTOUT")))
		printf("nRESET request failed! (err=%d)\n", v);
	if (!!(v=dm_gpio_set_dir_flags(&nrstout_gpio, GPIOD_IS_OUT)))
		printf("nRESET configure output failed! (err=%d)\n",v);
	if (!!(v=dm_gpio_set_value(&nrstout_gpio, 0)))
		printf("nRESET deassert failed! (err=%d)\n", v);

#ifdef CONFIG_SPL_ENV_SUPPORT
	env_init();
	env_load();
	if (env_get_yesno("boot_os") != 1) {
		printf("Falcon bootmode not allowed by environment!\n");
		/* Yep, we must wait power button RELEASE and PRESS time */
		udelay(PWRBTN_DETECT_DELAY);
		if (dm_gpio_set_value(&nrstout_gpio, 1))
			printf("nRESET assert failed!\n");
		return SPL_LOAD_UBOOT;
	}
#endif
	ret = dm_gpio_lookup_name(pwr_gpio, &pwrbtn_gpio);
	if (ret) {
		printf("Power button GPIO lookup failed (%d)!\n", ret);
		return SPL_LOAD_UBOOT;
	}

	ret = dm_gpio_request(&pwrbtn_gpio, "PWRBTN");
	if(ret) {
		printf("PWRBTN request failed (%d)!\n", ret);
		return SPL_LOAD_UBOOT;
	};

	dm_gpio_set_dir_flags(&pwrbtn_gpio, GPIOD_IS_IN);

	/* Yep, we must wait power button RELEASE and PRESS again */
	udelay(PWRBTN_DETECT_DELAY);

	ret = dm_gpio_get_value(&pwrbtn_gpio);
	if(ret < 0) {
		printf("PWRBTN get value failed (%d)!\n", ret);
		return SPL_LOAD_UBOOT;
	};

	/* Power button pulled UP, unpressed read "1", pressed "0" */
	ret = ret ? SPL_LOAD_SYSTEM : SPL_LOAD_UBOOT;
	debug(ret == SPL_LOAD_SYSTEM ? "Falcon mode\n" : "Tortoise mode\n");

	if (dm_gpio_set_value(&nrstout_gpio, 1))
		printf("nRESET assert failed!\n");

	return !!ret;
}
#endif /* CONFIG_SPL_OS_BOOT*/

void board_boot_order(u32 *spl_boot_list)
{
	spl_boot_list[0] = BOOT_DEVICE_MMC1;
	spl_boot_list[1] = BOOT_DEVICE_USB;
	spl_boot_list[2] = BOOT_DEVICE_BOARD;
	spl_boot_list[3] = BOOT_DEVICE_NONE;
}

#warning FixMe: Only really required clocks enabled here. ORLY?
static void ccgr_init(void)
{
	struct mxc_ccm_reg *ccm = (struct mxc_ccm_reg *)CCM_BASE_ADDR;
	writel(	MXC_CCM_CCGR0_CHEETAH_DBG_CLK_MASK |
		MXC_CCM_CCGR0_ASRC_MASK |
		MXC_CCM_CCGR0_AIPS_TZ1_MASK |
		MXC_CCM_CCGR0_AIPS_TZ2_MASK |
		MXC_CCM_CCGR0_APBHDMA_MASK,
		&ccm->CCGR0);
	writel( MXC_CCM_CCGR1_ENET_MASK |
		MXC_CCM_CCGR1_GPT_BUS_MASK |
		MXC_CCM_CCGR1_GPT_SERIAL_MASK,
		&ccm->CCGR1);
	writel(	MXC_CCM_CCGR2_IOMUX_IPT_CLK_IO_MASK |
		MXC_CCM_CCGR2_IPMUX1_MASK |
		MXC_CCM_CCGR2_IPMUX2_MASK |
		MXC_CCM_CCGR2_IPMUX3_MASK |
		MXC_CCM_CCGR2_I2C1_SERIAL_MASK,
		&ccm->CCGR2);
	writel(	MXC_CCM_CCGR3_OCRAM_MASK |
		MXC_CCM_CCGR3_MMDC_CORE_ACLK_FAST_CORE_P0_MASK |
		MXC_CCM_CCGR3_MMDC_CORE_IPG_CLK_P0_MASK |
		MXC_CCM_CCGR3_MLB_MASK,
		&ccm->CCGR3);
	writel(	MXC_CCM_CCGR4_PL301_MX6QFAST1_S133_MASK |
		MXC_CCM_CCGR4_PL301_MX6QPER1_BCH_MASK |
		MXC_CCM_CCGR4_PL301_MX6QPER2_MAINCLK_ENABLE_MASK,
		&ccm->CCGR4);
	writel(	MXC_CCM_CCGR5_ROM_MASK |
		MXC_CCM_CCGR5_SATA_MASK |
		MXC_CCM_CCGR5_SDMA_MASK |
		MXC_CCM_CCGR5_UART_MASK |
		MXC_CCM_CCGR5_UART_SERIAL_MASK,
		&ccm->CCGR5);
	writel(	MXC_CCM_CCGR6_USDHC1_MASK |
		MXC_CCM_CCGR6_USDHC2_MASK |
		MXC_CCM_CCGR6_USDHC3_MASK |
		MXC_CCM_CCGR6_USBOH3_MASK,
		&ccm->CCGR6);
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
	spl_dram_init();		/* Yeah! We REALLY require this in SPL. */
	arch_cpu_init();		/* Theory, we not require this code */
	gpr_init();			/* AXI caches also will be inited not here */
	ccgr_init();			/* Fixup clock troubles in DM code */
	timer_init();			/* gpt-timer later will be in DM */
	spl_early_init();		/* DM devices init...		*/
	preloader_console_init();	/* 	... then start console	*/
}
