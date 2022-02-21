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

#warning FixMe: Only really required clocks _MUST_BE_ enabled here!
static void ccgr_init(void)
{
	struct mxc_ccm_reg *ccm = (struct mxc_ccm_reg *)CCM_BASE_ADDR;
#if 1
	writel( 0xFFFFFFFF, &ccm->CCGR0);
	writel( 0xFFFFFFFF, &ccm->CCGR1);
	writel( 0xFFFFFFFF, &ccm->CCGR2);
	writel( 0xFFFFFFFF, &ccm->CCGR3);
	writel( 0xFFFFFFFF, &ccm->CCGR4);
	writel( 0xFFFFFFFF, &ccm->CCGR5);
	writel( 0xFFFFFFFF, &ccm->CCGR6);
#else /* this code NOT load Linux, may be reserved bits _MUST_ be 1 at write ??? */
	writel(							/* 31, 30 */
		MXC_CCM_CCGR0_DTCP_MASK |			/* 29, 28 */
		MXC_CCM_CCGR0_DCIC2_MASK |			/* 27, 26 */
		MXC_CCM_CCGR0_DCIC1_MASK |			/* 25, 24 */
		MXC_CCM_CCGR0_CHEETAH_DBG_CLK_MASK |		/* 23, 22 */
		MXC_CCM_CCGR0_CAN2_SERIAL_MASK |		/* 21, 20 */
		MXC_CCM_CCGR0_CAN2_MASK |			/* 19, 18 */
		MXC_CCM_CCGR0_CAN1_SERIAL_MASK |		/* 17, 16 */
		MXC_CCM_CCGR0_CAN1_MASK |			/* 15, 14 */
		MXC_CCM_CCGR0_CAAM_WRAPPER_IPG_MASK |		/* 13, 12 */
		MXC_CCM_CCGR0_CAAM_WRAPPER_ACLK_MASK |		/* 11, 10 */
		MXC_CCM_CCGR0_CAAM_SECURE_MEM_MASK |		/* 09, 08 */
		MXC_CCM_CCGR0_ASRC_MASK |			/* 07, 06 */
		MXC_CCM_CCGR0_APBHDMA_MASK |			/* 05, 04 */
		MXC_CCM_CCGR0_AIPS_TZ1_MASK |			/* 03, 02 */
		MXC_CCM_CCGR0_AIPS_TZ2_MASK |			/* 01, 00 */
		MXC_CCM_CCGR0_APBHDMA_MASK,
		&ccm->CCGR0);
	writel( 						/* 31..28 */
		MXC_CCM_CCGR1_GPU3D_MASK |			/* 27, 26 */
		MXC_CCM_CCGR1_GPU2D_MASK |			/* 25, 24 */
		MXC_CCM_CCGR1_GPT_SERIAL_MASK |			/* 23, 22 */
		MXC_CCM_CCGR1_GPT_BUS_MASK |			/* 21, 20 */
								/* 19, 18 */
		MXC_CCM_CCGR1_ESAIS_MASK |			/* 17, 16 */
		MXC_CCM_CCGR1_EPIT2S_MASK |			/* 15, 14 */
		MXC_CCM_CCGR1_EPIT1S_MASK |			/* 13, 12 */
		MXC_CCM_CCGR1_ENET_MASK |			/* 11, 10 */
		MXC_CCM_CCGR1_ECSPI5S_MASK |			/* 09, 08 */
		MXC_CCM_CCGR1_ECSPI4S_MASK |			/* 07, 06 */
		MXC_CCM_CCGR1_ECSPI3S_MASK |			/* 05, 04 */
		MXC_CCM_CCGR1_ECSPI2S_MASK |			/* 03, 02 */
		MXC_CCM_CCGR1_ECSPI1S_MASK,			/* 01, 00 */
		&ccm->CCGR1);
	writel( 						/* 31..28 */
		MXC_CCM_CCGR2_IPSYNC_VDOA_IPG_MASTER_CLK_MASK | /* 27, 26 */
		MXC_CCM_CCGR2_IPSYNC_IP2APB_TZASC2_IPG_MASK |	/* 25, 24 */
		MXC_CCM_CCGR2_IOMUX_IPT_CLK_IO_MASK |		/* 23, 22 */
		MXC_CCM_CCGR2_IPMUX3_MASK |			/* 21, 20 */
		MXC_CCM_CCGR2_IPMUX2_MASK |			/* 19, 18 */
		MXC_CCM_CCGR2_IPMUX1_MASK |			/* 17, 16 */
		MXC_CCM_CCGR2_IOMUX_IPT_CLK_IO_MASK |		/* 15, 14 */
		MXC_CCM_CCGR2_OCOTP_CTRL_MASK |			/* 13, 12 */
		MXC_CCM_CCGR2_I2C3_SERIAL_MASK |		/* 11, 10 */
		MXC_CCM_CCGR2_I2C2_SERIAL_MASK |		/* 09, 08 */
		MXC_CCM_CCGR2_I2C1_SERIAL_MASK |		/* 07, 06 */
		MXC_CCM_CCGR2_HDMI_TX_ISFRCLK_MASK |		/* 05, 04 */
								/* 03, 02 */
		MXC_CCM_CCGR2_HDMI_TX_IAHBCLK_MASK,		/* 01, 00 */
		&ccm->CCGR2);
	writel(							/* 31, 30 */
		MXC_CCM_CCGR3_OCRAM_MASK |			/* 29, 28 */
								/* 27, 26 */
		MXC_CCM_CCGR3_MMDC_CORE_IPG_CLK_P0_MASK |	/* 25, 24 */
								/* 23, 22 */
		MXC_CCM_CCGR3_MMDC_CORE_ACLK_FAST_CORE_P0_MASK |/* 21, 20 */
		MXC_CCM_CCGR3_MLB_MASK |			/* 19, 18 */
		MXC_CCM_CCGR3_MIPI_CORE_CFG_MASK |		/* 17, 16 */
		MXC_CCM_CCGR3_LDB_DI1_MASK |			/* 15, 14 */
		MXC_CCM_CCGR3_LDB_DI0_MASK |			/* 13, 12 */
		MXC_CCM_CCGR3_IPU2_IPU_DI1_MASK |		/* 11, 10 */
		MXC_CCM_CCGR3_IPU2_IPU_DI0_MASK |		/* 09, 08 */
		MXC_CCM_CCGR3_IPU2_IPU_MASK |			/* 07, 06 */
		MXC_CCM_CCGR3_IPU1_IPU_DI1_MASK |		/* 05, 04 */
		MXC_CCM_CCGR3_IPU1_IPU_DI0_MASK |		/* 03, 02 */
		MXC_CCM_CCGR3_IPU1_IPU_MASK,			/* 01, 00 */
		&ccm->CCGR3);
	writel(	MXC_CCM_CCGR4_RAWNAND_U_GPMI_INPUT_APB_MASK |	/* 31, 30 */
		MXC_CCM_CCGR4_RAWNAND_U_GPMI_BCH_INPUT_GPMI_IO_MASK |	/* 29, 28 */
		MXC_CCM_CCGR4_RAWNAND_U_GPMI_BCH_INPUT_BCH_MASK|/* 27, 26 */
		MXC_CCM_CCGR4_RAWNAND_U_BCH_INPUT_APB_MASK |	/* 25, 24 */
		MXC_CCM_CCGR4_PWM4_MASK |			/* 23, 22 */
		MXC_CCM_CCGR4_PWM3_MASK |			/* 21, 20 */
		MXC_CCM_CCGR4_PWM2_MASK |			/* 19, 18 */
		MXC_CCM_CCGR4_PWM1_MASK |			/* 17, 16 */
		MXC_CCM_CCGR4_PL301_MX6QPER2_MAINCLK_ENABLE_MASK |	/* 15, 14 */
		MXC_CCM_CCGR4_PL301_MX6QPER1_BCH_MASK |		/* 13, 12 */
								/* 11, 10 */
		MXC_CCM_CCGR4_PL301_MX6QFAST1_S133_MASK |	/* 09, 08 */
								/* 07..02 */
		MXC_CCM_CCGR4_PCIE_MASK,			/* 01, 00 */
		&ccm->CCGR4);
	writel(							/* 31..28 */
		MXC_CCM_CCGR5_UART_SERIAL_MASK |		/* 27, 26 */
		MXC_CCM_CCGR5_UART_MASK |			/* 25, 24 */
		MXC_CCM_CCGR5_SSI3_MASK |			/* 23, 22 */
		MXC_CCM_CCGR5_SSI2_MASK |			/* 21, 20 */
		MXC_CCM_CCGR5_SSI1_MASK |			/* 19, 18 */
								/* 17, 16 */
		MXC_CCM_CCGR5_SPDIF_MASK |			/* 15, 14 */
		MXC_CCM_CCGR5_SPBA_MASK |			/* 13, 12 */
								/* 11..08 */
		MXC_CCM_CCGR5_SDMA_MASK |			/* 07, 06 */
		MXC_CCM_CCGR5_SATA_MASK |			/* 05, 04 */
								/* 03, 02 */
		MXC_CCM_CCGR5_ROM_MASK,				/* 01, 00 */
		&ccm->CCGR5);
	writel(							/* 31..28 */
		(MXC_CCM_CCGR_CG_MASK<<26) | 	/* PRG_CLK_1       27, 26 */
		(MXC_CCM_CCGR_CG_MASK<<24) | 	/* PRG_CLK_0       25, 24 */
		(MXC_CCM_CCGR_CG_MASK<<22) | 	/* PRE_CLK_3       23, 21 */
		(MXC_CCM_CCGR_CG_MASK<<20) | 	/* PRE_CLK_2       21, 20 */
		(MXC_CCM_CCGR_CG_MASK<<18) | 	/* PRE_CLK_1       19, 18 */
		(MXC_CCM_CCGR_CG_MASK<<16) | 	/* PRE_CLK_0       17, 16 */
		(MXC_CCM_CCGR_CG_MASK<<14) | 	/* VPU CLOCKS      15, 14 */
		MXC_CCM_CCGR6_VDOAXICLK_MASK |			/* 13, 12 */
		MXC_CCM_CCGR6_EMI_SLOW_MASK |			/* 11, 10 */
		MXC_CCM_CCGR6_USDHC4_MASK |			/* 09, 08 */
		MXC_CCM_CCGR6_USDHC3_MASK |			/* 07, 06 */
		MXC_CCM_CCGR6_USDHC2_MASK |			/* 05, 04 */
		MXC_CCM_CCGR6_USDHC1_MASK |			/* 03, 02 */
		MXC_CCM_CCGR6_USBOH3_MASK,			/* 01, 00 */
		&ccm->CCGR6);
#endif
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
