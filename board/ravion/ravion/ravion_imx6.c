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
#include <malloc.h>
#include <micrel.h>
#include <miiphy.h>
#include <netdev.h>
#include <usb.h>

#include "../common/rav-cfg-block.h"

DECLARE_GLOBAL_DATA_PTR;

int dram_init(void)
{
	/* use the DDR controllers configured size */
	gd->ram_size = get_ram_size((void *)CONFIG_SYS_SDRAM_BASE,
				    (ulong)imx_ddr_size());

	return 0;
}

/*
 * Do not overwrite the console
 * Use always serial for U-Boot console
 */
int overwrite_console(void)
{
	return 1;
}

int board_init(void)
{
	/* address of boot parameters */
	gd->bd->bi_boot_params = PHYS_SDRAM + 0x100;

	return 0;
}

#ifdef CONFIG_BOARD_EARLY_INIT_F
#define UART_PAD_CTRL  (PAD_CTL_PUS_100K_UP |	\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm |	\
	PAD_CTL_SRE_FAST  | PAD_CTL_HYS)

static iomux_v3_cfg_t const uart1_pads[] = {
	MX6_PAD_CSI0_DAT10__UART1_TX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX6_PAD_CSI0_DAT11__UART1_RX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL),
};


int board_early_init_f(void)
{
	SETUP_IOMUX_PADS(uart1_pads);
	return 0;
}
#endif /* CONFIG_BOARD_EARLY_INIT_F */

#ifdef CONFIG_BOARD_LATE_INIT
int board_late_init(void)
{
#ifdef CONFIG_FEC_MXC
	/* enable FEC clock */
	struct iomuxc *iomuxc_regs = (struct iomuxc *)IOMUXC_BASE_ADDR;
	enable_fec_anatop_clock(0, ENET_50MHZ);
	setbits_le32(&iomuxc_regs->gpr[1], IOMUXC_GPR1_ENET_CLK_SEL_MASK);
#endif /* CONFIG_FEC_MXC */

#if defined(CONFIG_REVISION_TAG) && \
    defined(CONFIG_ENV_VARS_UBOOT_RUNTIME_CONFIG)
	char env_str[256];
	u32 rev;

	rev = get_board_rev();
	snprintf(env_str, ARRAY_SIZE(env_str), "%.4x", rev);
	env_set("board_rev", env_str);
#endif

#ifdef CONFIG_USB
	usb_init();
#ifdef CONFIG_USB_STORAGE
	/* try to recognize storage devices immediately */
	usb_stor_scan(1);
#endif
#endif
	return 0;
}
#endif /* CONFIG_BOARD_LATE_INIT */

#if defined(CONFIG_OF_LIBFDT) && defined(CONFIG_OF_BOARD_SETUP)
/*
 * ToDo: Place FDT patches here. Applyed for U-Boot FDT
 */
int ft_board_setup(void *blob, bd_t *bd)
{
	return ft_common_board_setup(blob, bd);
}
#endif

int misc_init_r(void)
{
#ifdef CONFIG_CMD_BMODE
	/* no need fixup plaftorm bootmodes */
	add_board_boot_modes(NULL);
#endif
	return 0;
}

#ifdef CONFIG_MULTI_DTB_FIT
int board_fit_config_name_match(const char *name)
{
	if (is_mx6dq()) {
		if (!strcmp(name, "imx6qp-ravion"))
			return 0;
	} else if (is_mx6dl()) {
		if (!strcmp(name, "imx6dl-ravion"))
			return 0;
	}

	return -1;
}
#endif

#ifdef CONFIG_SPL_BUILD
#include <asm/arch/mx6-ddr.h>
#include <spl.h>
#include <linux/libfdt.h>
#include "ravion_qp_2048.h"

#ifdef CONFIG_SPL_OS_BOOT
int spl_start_uboot(void)
{
	return 1;
}
#endif /* CONFIG_SPL_OS_BOOT*/

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
		ddr_init(imx6qp_2048_dcd, ARRAY_SIZE(imx6qp_2048_dcd));
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

#endif /* CONFIG_SPL_BUILD */
