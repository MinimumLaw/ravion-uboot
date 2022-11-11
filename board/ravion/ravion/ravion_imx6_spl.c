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

#include <button.h>
#include <led.h>
#include <dm/uclass-internal.h>

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
	struct udevice *btn_power, *led_reset;
	int ret;

	if (button_get_by_label("Power", &btn_power)) {
	    printf("SPL: Power button not found!\n");
	    return SPL_LOAD_UBOOT;
	};

	if (led_get_by_label("sys::nRESET", &led_reset)) {
	    printf("SPL: system reset not found!\n");
	    return SPL_LOAD_UBOOT;
	};

	if (led_set_state(led_reset, LEDST_ON)) {
	    printf("SPL: reset activate failed!\n");
	    return SPL_LOAD_UBOOT;
	};

#ifdef CONFIG_SPL_ENV_SUPPORT
	env_init();
	env_load();
	if (env_get_yesno("boot_os") != 1) {
		printf("Falcon bootmode not allowed by environment!\n");
		/* Yep, we must wait power button RELEASE and PRESS time */
		udelay(PWRBTN_DETECT_DELAY);
		if (led_set_state(led_reset, LEDST_OFF)) {
			printf("SPL: reset deactivate failed!\n");
		};
		return SPL_LOAD_UBOOT;
	}
#endif

	/* Yep, we must wait power button RELEASE and PRESS again */
	udelay(PWRBTN_DETECT_DELAY);

	ret = button_get_state(btn_power);
	if ( ret >= BUTTON_COUNT) {
		printf("SPL: power button read failed!\n");
		return SPL_LOAD_UBOOT;
	}

	ret = (ret == BUTTON_OFF) ? SPL_LOAD_SYSTEM : SPL_LOAD_UBOOT;
	printf(ret == SPL_LOAD_SYSTEM ? "Falcon mode\n" : "Tortoise mode\n");

	if (led_set_state(led_reset, LEDST_OFF)) {
		printf("SPL: reset deactivate failed!\n");
	};

	return !!ret;
}
#else  /* !CONFIG_SPL_OS_BOOT*/

/* No Falcon mode supported */
void prepare_for_uboot(void)
{
	struct udevice *led_reset;

	if (led_get_by_label("sys::nRESET", &led_reset)) {
	    printf("SPL: system reset not found!\n");
	    return;
	};

	if (led_set_state(led_reset, LEDST_ON)) {
	    printf("SPL: reset activate failed!\n");
	    return;
	};
}
#endif /* CONFIG_SPL_OS_BOOT*/

void board_boot_order(u32 *spl_boot_list)
{
	if (is_boot_from_usb()) {
		puts("Factory recovery boot!\n");
		spl_boot_list[0] = BOOT_DEVICE_BOARD;
		spl_boot_list[1] = BOOT_DEVICE_NONE;
		spl_boot_list[2] = BOOT_DEVICE_NONE;
		spl_boot_list[3] = BOOT_DEVICE_NONE;
	} else {
		spl_boot_list[0] = BOOT_DEVICE_MMC1;
		spl_boot_list[2] = BOOT_DEVICE_BOARD;
		spl_boot_list[1] = BOOT_DEVICE_NONE;
		spl_boot_list[3] = BOOT_DEVICE_NONE;
	};
}

static void ccgr_init(void)
{
	struct mxc_ccm_reg *ccm = (struct mxc_ccm_reg *)CCM_BASE_ADDR;
#if 1
#warning FixMe: Only really required clocks need be enabled here!
	writel( 0xFFFFFFFF, &ccm->CCGR0);
	writel( 0xFFFFFFFF, &ccm->CCGR1);
	writel( 0xFFFFFFFF, &ccm->CCGR2);
	writel( 0xFFFFFFFF, &ccm->CCGR3);
	writel( 0xFFFFFFFF, &ccm->CCGR4);
	writel( 0xFFFFFFFF, &ccm->CCGR5);
	writel( 0xFFFFFFFF, &ccm->CCGR6);
#else
	/*
	 * Bitmask defined clocks from CCM CCGRx registers
	 *  (_NOT_ _WORK_ as expected - need be dzenned)
	 */
	writel(
		(3<<30) |			/* Reserved (1)    31, 30 */
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
	writel(
		(0xF<<30) |			/* Reserved (1)    31..28 */
		MXC_CCM_CCGR1_GPU3D_MASK |			/* 27, 26 */
		MXC_CCM_CCGR1_GPU2D_MASK |			/* 25, 24 */
		MXC_CCM_CCGR1_GPT_SERIAL_MASK |			/* 23, 22 */
		MXC_CCM_CCGR1_GPT_BUS_MASK |			/* 21, 20 */
		(3<<18) |			/* Reserved (1)    19, 18 */
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
	writel(
		(0xF<<28) |			/* Reserved (1)    31..28 */
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
		 (3<<2) |			/* Reserved (1)    03, 02 */
		MXC_CCM_CCGR2_HDMI_TX_IAHBCLK_MASK,		/* 01, 00 */
		&ccm->CCGR2);
	writel(
		(3<<30) |			/* Reserved (1)    31, 30 */
		MXC_CCM_CCGR3_OCRAM_MASK |			/* 29, 28 */
		(3<<26) |			/* Reserved (1)    27, 26 */
		MXC_CCM_CCGR3_MMDC_CORE_IPG_CLK_P0_MASK |	/* 25, 24 */
		(3<<22) |			/* Reserved (1)    23, 22 */
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
	writel(
		MXC_CCM_CCGR4_RAWNAND_U_GPMI_INPUT_APB_MASK |	/* 31, 30 */
		MXC_CCM_CCGR4_RAWNAND_U_GPMI_BCH_INPUT_GPMI_IO_MASK |/* 29, 28 */
		MXC_CCM_CCGR4_RAWNAND_U_GPMI_BCH_INPUT_BCH_MASK|/* 27, 26 */
		MXC_CCM_CCGR4_RAWNAND_U_BCH_INPUT_APB_MASK |	/* 25, 24 */
		MXC_CCM_CCGR4_PWM4_MASK |			/* 23, 22 */
		MXC_CCM_CCGR4_PWM3_MASK |			/* 21, 20 */
		MXC_CCM_CCGR4_PWM2_MASK |			/* 19, 18 */
		MXC_CCM_CCGR4_PWM1_MASK |			/* 17, 16 */
		MXC_CCM_CCGR4_PL301_MX6QPER2_MAINCLK_ENABLE_MASK | /* 15, 14 */
		MXC_CCM_CCGR4_PL301_MX6QPER1_BCH_MASK |		/* 13, 12 */
		(3<<10) |			/* Reserved (1)    11, 10 */
		MXC_CCM_CCGR4_PL301_MX6QFAST1_S133_MASK |	/* 09, 08 */
		(0x3F<<2) |			/* Reserved (1)    07..02 */
		MXC_CCM_CCGR4_PCIE_MASK,			/* 01, 00 */
		&ccm->CCGR4);
	writel(	(0xF<<28) |			/* Reserved (1)    31..28 */
		MXC_CCM_CCGR5_UART_SERIAL_MASK |		/* 27, 26 */
		MXC_CCM_CCGR5_UART_MASK |			/* 25, 24 */
		MXC_CCM_CCGR5_SSI3_MASK |			/*   23, 22 */
		MXC_CCM_CCGR5_SSI2_MASK |			/* 21, 20 */
		MXC_CCM_CCGR5_SSI1_MASK |			/* 19, 18 */
		(3<<16) |			/* Reserved (1)    17, 16 */
		MXC_CCM_CCGR5_SPDIF_MASK |			/* 15, 14 */
		MXC_CCM_CCGR5_SPBA_MASK |			/* 13, 12 */
		(0xF<<8) |			/* Reserved (1)    11..08 */
		MXC_CCM_CCGR5_SDMA_MASK |			/* 07, 06 */
		MXC_CCM_CCGR5_SATA_MASK |			/* 05, 04 */
		(3<<2) |			/* Reserved (1)    03, 02 */
		MXC_CCM_CCGR5_ROM_MASK,				/* 01, 00 */
		&ccm->CCGR5);
	writel(	(0xF<<28) |			/* Reserved (1)    31..28 */
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

struct mx6dq_iomux_ddr_regs qp_iomux_ddr = {
	.dram_cas	= 0x00000030,
	.dram_dqm0	= 0x00000030,
	.dram_dqm1	= 0x00000030,
	.dram_dqm2	= 0x00000030,
	.dram_dqm3	= 0x00000030,
	.dram_dqm4	= 0x00000030,
	.dram_dqm5	= 0x00000030,
	.dram_dqm6	= 0x00000030,
	.dram_dqm7	= 0x00000030,
	.dram_ras	= 0x00000030,
	.dram_reset	= 0x00000030,
	.dram_sdba2	= 0x00000000,
	.dram_sdcke0	= 0x00003000,
	.dram_sdcke1	= 0x00003000,
	.dram_sdclk_0	= 0x00000030,
	.dram_sdclk_1	= 0x00000030,
	.dram_sdodt0	= 0x00000030,
	.dram_sdodt1	= 0x00000030,
	.dram_sdqs0	= 0x00000030,
	.dram_sdqs1	= 0x00000030,
	.dram_sdqs2	= 0x00000030,
	.dram_sdqs3	= 0x00000030,
	.dram_sdqs4	= 0x00000030,
	.dram_sdqs5	= 0x00000030,
	.dram_sdqs6	= 0x00000030,
	.dram_sdqs7	= 0x00000030,
};

struct mx6dq_iomux_grp_regs qp_iomux_grp = {
	.grp_addds	= 0x00000030,
	.grp_b0ds	= 0x00000030,
	.grp_b1ds	= 0x00000030,
	.grp_b2ds	= 0x00000030,
	.grp_b3ds	= 0x00000030,
	.grp_b4ds	= 0x00000030,
	.grp_b5ds	= 0x00000030,
	.grp_b6ds	= 0x00000030,
	.grp_b7ds	= 0x00000030,
	.grp_ctlds	= 0x00000030,
	.grp_ddrmode	= 0x00020000,
	.grp_ddrmode_ctl= 0x00020000,
	.grp_ddrpke	= 0x00000000,
	.grp_ddr_type	= 0x000C0000,
};

struct mx6_ddr3_cfg mt41k256m16ha_125_cfg = {
	.mem_speed	= 1600,	/* ie 1600 for DDR3-1600 (800,1066,1333,1600) */
	.density	= 4,	/* chip density (Gb) (1,2,4,8) */
	.width		= 16,	/* bus width (bits) (4,8,16) */
	.banks		= 8,	/* number of banks */
	.rowaddr	= 15,	/* row address bits (11-16)*/
	.coladdr	= 10,	/* col address bits (9-12) */
	.pagesz		= 2,	/* page size (K) (1-2) */
	.trcd		= 1375,	/* tRCD=tRP=CL (ns*100) */
	.trcmin		= 4875,	/* tRC min (ns*100) */
	.trasmin	= 3500,	/* tRAS min (ns*100) */
	.SRT		= 1,	/* self-refresh temperature: 0=normal, 1=extended */
};

// Table 55: DDR3L-1600 Speed Bins, CL(ralat)=5, CWL(walat)=5 minimal possible
struct mx6_ddr_sysinfo qp_sysinfo = {
	.dsize		= 2,	/* size of bus (in dwords: 0=16bit,1=32bit,2=64bit) */
	.cs_density	= 32,	/* density per chip select (Gb) */
	.ncs		= 1,	/* number chip selects used (1|2) */
	.cs1_mirror	= 0,	/* enable address mirror (0|1) */
	.bi_on		= 1,	/* Bank interleaving enable */
	.rtt_nom	= 2,	/* Rtt_Nom (DDR3_RTT_*) RZQ/2 */
	.rtt_wr		= 1,	/* Rtt_Wr  (DDR3_RTT_*) RZQ/4 */
	.ralat		= 5,	/* Read Additional Latency (0-7) */
	.walat		= 0,	/* Write Additional Latency (0-3) */
	.mif3_mode	= 3,	/* Command prediction working mode */
	.rst_to_cke	= 0x10,	/* Time from SDE enable to CKE rise              - 200us DD3 JEDEC (14CK) */
	.sde_to_rst	= 0x23,	/* Time from SDE enable until DDR reset# is high - 500us DD3 JEDEC (33CK) */
	.pd_fast_exit	= 1,	/* enable precharge powerdown fast-exit */
	.ddr_type	= DDR_TYPE_DDR3,	/* DDR type: DDR3(0) or LPDDR2(1) */
	.refsel		= 1,	/* REF_SEL field of register MDREF - every 32KHz */
	.refr		= 3,	/* REFR field of register MDREF    - do 4 refresh commands  */
};

struct mx6_mmdc_calibration qp_calibration_528 = {
	/* write leveling calibration */
	.p0_mpwldectrl0	= 0x001B0016,
	.p0_mpwldectrl1	= 0x00240017,
	.p1_mpwldectrl0	= 0x00090023,
	.p1_mpwldectrl1	= 0x0006000B,
	/* read DQS gating */
	.p0_mpdgctrl0	= 0x031C0330,
	.p0_mpdgctrl1	= 0x03140310,
	.p1_mpdgctrl0	= 0x03280338,
	.p1_mpdgctrl1	= 0x03140270,
	/* read delay */
	.p0_mprddlctl	= 0x42343C42,
	.p1_mprddlctl	= 0x403C364A,
	/* write delay */
	.p0_mpwrdlctl	= 0x3E3C4240,
	.p1_mpwrdlctl	= 0x4E404C46,
	/* lpddr2 zq hw calibration */
	.mpzqlp2ctl	= 0xA1390003,
};

static void spl_dram_init(void)
{
	if (is_mx6dqp()) {
		mx6dq_dram_iocfg(64, &qp_iomux_ddr, &qp_iomux_grp);
		mx6_dram_cfg(&qp_sysinfo, &qp_calibration_528, &mt41k256m16ha_125_cfg);
		udelay(100);

#ifdef CONFIG_MX6_DDRCAL
		/* Perform DDR DRAM calibration */
		mmdc_do_write_level_calibration(&qp_sysinfo);
		mmdc_do_dqs_calibration(&qp_sysinfo);
#endif
	} else if (is_mx6dq()) {
//		ddr_init(imx6dl_2048_dcd, ARRAY_SIZE(imx6qp_2048_dcd));
	}
}

void board_init_f(ulong dummy)
{
	ccgr_init();			/* Fixup clock troubles in DM code */
	arch_cpu_init();		/* Theory, we not require this code */
	gpr_init();			/* AXI caches also will be inited not here */
	timer_init();			/* gpt-timer later will be in DM */
	spl_early_init();		/* DM devices init...		*/
	preloader_console_init();	/* 	... then start console	*/
#if !defined CONFIG_SPL_OS_BOOT
	/* Falcon mode disabled, just load U-Boot */
	prepare_for_uboot();
#endif
	spl_dram_init();		/* Yeah! We REALLY require this in SPL. */
}
