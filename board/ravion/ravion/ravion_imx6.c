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

#include "../common/rav-cfg-block.h"

DECLARE_GLOBAL_DATA_PTR;

int dram_init(void)
{
	/* use the DDR controllers configured size */
	gd->ram_size = get_ram_size((void *)CONFIG_SYS_SDRAM_BASE,
				    (ulong)imx_ddr_size());

	return 0;
}

static iomux_v3_cfg_t const pwr_intb_pads[] = {
#define EXT_NRESETOUTPUT_GP IMX_GPIO_NR(5, 7)
	MX6_PAD_DISP0_DAT13__GPIO5_IO07 | MUX_PAD_CTRL(NO_PAD_CTRL),
};

int board_early_init_f(void)
{
	imx_iomux_v3_setup_multiple_pads(pwr_intb_pads,
					 ARRAY_SIZE(pwr_intb_pads));
	/* Set nRESETOUT to 100mS, then set them into ACTIVE state */
	gpio_direction_output(EXT_NRESETOUTPUT_GP, 1);

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

#ifdef CONFIG_BOARD_LATE_INIT
int board_late_init(void)
{
	/* enable FEC clock */
	struct iomuxc *iomuxc_regs = (struct iomuxc *)IOMUXC_BASE_ADDR;
	enable_fec_anatop_clock(0, ENET_50MHZ);
	setbits_le32(&iomuxc_regs->gpr[1], IOMUXC_GPR1_ENET_CLK_SEL_MASK);
#if defined(CONFIG_REVISION_TAG) && \
    defined(CONFIG_ENV_VARS_UBOOT_RUNTIME_CONFIG)
	char env_str[256];
	u32 rev;

	rev = get_board_rev();
	snprintf(env_str, ARRAY_SIZE(env_str), "%.4x", rev);
	env_set("board_rev", env_str);
#endif
	return 0;
}
#endif /* CONFIG_BOARD_LATE_INIT */

#if defined(CONFIG_OF_LIBFDT) && defined(CONFIG_OF_SYSTEM_SETUP)
int ft_system_setup(void *blob, bd_t *bd)
{
	return 0;
}
#endif

int checkboard(void)
{
	char it[] = " IT";
	int minc, maxc;

	switch (get_cpu_temp_grade(&minc, &maxc)) {
	case TEMP_AUTOMOTIVE:
	case TEMP_INDUSTRIAL:
		break;
	case TEMP_EXTCOMMERCIAL:
	default:
		it[0] = 0;
	};
	printf("CPU Module: Ravion iMX6 %s %sMB%s\n",
	       is_cpu_type(MXC_CPU_MX6QP) ? "QuadPlus" : \
			is_cpu_type(MXC_CPU_MX6Q) ? "Quad" : \
	    		is_cpu_type(MXC_CPU_MX6DL) ? "DualLite" : "Solo",
	       (gd->ram_size == 0x80000000) ? "2048" : \
			(gd->ram_size == 0x40000000) ? "1024" : \
			(gd->ram_size == 0x20000000) ? "512" : "256", it);
	return 0;
}

#if defined(CONFIG_OF_LIBFDT) && defined(CONFIG_OF_BOARD_SETUP)
int ft_board_setup(void *blob, bd_t *bd)
{
	return ft_common_board_setup(blob, bd);
}
#endif

#ifdef CONFIG_CMD_BMODE
static const struct boot_mode board_boot_modes[] = {
	{"mmc",	MAKE_CFGVAL(0x40, 0x20, 0x00, 0x00)},
	{NULL,	0},
};
#endif

int misc_init_r(void)
{
#ifdef CONFIG_CMD_BMODE
	add_board_boot_modes(board_boot_modes);
#endif
	return 0;
}

#ifdef CONFIG_LDO_BYPASS_CHECK
/* TODO, use external pmic, for now always ldo_enable */
void ldo_mode_set(int ldo_bypass)
{
	return;
}
#endif
