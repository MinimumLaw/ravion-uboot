/*
 * Copyright (C) 2010-2013 Freescale Semiconductor, Inc.
 * Copyright (C) 2013, Boundary Devices <info@boundarydevices.com>
 * Copyright (C) 2014-2016, Toradex AG
 * copied from nitrogen6x
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

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
#include <init.h>
#include <input.h>
#include <imx_thermal.h>
#include <linux/errno.h>
#include <malloc.h>
#include <micrel.h>
#include <miiphy.h>
#include <netdev.h>
#include <power/regulator.h>
#include <usb.h>

#include "../common/rav-cfg-block.h"

DECLARE_GLOBAL_DATA_PTR;

#if defined(CONFIG_OF_LIBFDT) && defined(CONFIG_OF_BOARD_SETUP)
int ft_board_setup(void *blob, struct bd_info *bd)
{
	return ft_common_board_setup(blob, bd);
}
#endif

int dram_init(void)
{
	/* use the DDR controllers configured size */
	gd->ram_size = get_ram_size((void *)CFG_SYS_SDRAM_BASE,
				    (ulong)imx_ddr_size());
	return 0;
}

int board_init(void)
{
	/* address of boot parameters */
	gd->bd->bi_boot_params = PHYS_SDRAM + 0x100;
#ifdef CONFIG_FEC_MXC /* enable FEC clock */
	struct iomuxc *iomuxc_regs = (struct iomuxc *)IOMUXC_BASE_ADDR;
	enable_fec_anatop_clock(0, ENET_50MHZ);
	setbits_le32(&iomuxc_regs->gpr[1], IOMUXC_GPR1_ENET_CLK_SEL_MASK);
#endif /* CONFIG_FEC_MXC */

	struct udevice *udev;
	if (!uclass_get_device_by_name(UCLASS_REGULATOR, "vgen4", &udev)) /* VDDHI_IN */
		regulator_set_enable(udev, true);
	if (!uclass_get_device_by_name(UCLASS_REGULATOR, "vgen6", &udev)) /* NVCC_LCD, NVCC_EIM, NVCC_SD1, NVCC_SD2, NVCC_NANDF */
		regulator_set_enable(udev, true);

	struct mxc_ccm_reg *ccm = (struct mxc_ccm_reg *)CCM_BASE_ADDR;
#warning FixMe: Only really required clocks need be enabled here!
	ccm->CCGR0 = 0xFFFFFFFF;
	ccm->CCGR1 = 0xFFFFFFFF;
	ccm->CCGR2 = 0xFFFFFFFF;
	ccm->CCGR3 = 0xFFFFFFFF;
	ccm->CCGR4 = 0xFFFFFFFF;
	ccm->CCGR5 = 0xFFFFFFFF;
	ccm->CCGR6 = 0xFFFFFFFF;

	/* USB required LDO 1P1, 2P5, 3P0 and VBUS power supply (defined in DTB) */
	ccm->pmu_reg_3p0_set = 0x3;
	ccm->pmu_reg_1p1_set = 0x3;
	ccm->pmu_reg_2p5_set = 0x3;

	return 0;
}

int board_late_init(void)
{
	/* restore critical boot env */
	env_set("bootdelay","-2");
	env_set("boot_syslinux_conf","extlinux/extlinux.conf");

	/* check boot from USB */
	if (is_boot_from_usb()) {
		puts("Factory mode enabled!\n");
		env_set("preboot","");
		env_set("bootdelay","0");
		env_set("bootcmd", "fastboot usb 0");
	}
	return 0;
}
