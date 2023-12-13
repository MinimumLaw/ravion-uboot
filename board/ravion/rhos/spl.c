// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2018, 2021 NXP
 *
 */

#include <common.h>
#include <hang.h>
#include <image.h>
#include <init.h>
#include <log.h>
#include <asm/global_data.h>
#include <asm/io.h>
#include <errno.h>
#include <asm/io.h>
#include <asm/arch/ddr.h>
#include <asm/arch/imx8mq_pins.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/clock.h>
#include <asm/mach-imx/iomux-v3.h>
#include <asm/mach-imx/gpio.h>
#include <asm/mach-imx/mxc_i2c.h>
#include <fsl_esdhc_imx.h>
#include <fsl_sec.h>
#include <mmc.h>
#include <linux/delay.h>
#include <spl.h>

DECLARE_GLOBAL_DATA_PTR;

void spl_board_init(void)
{
	if (IS_ENABLED(CONFIG_FSL_CAAM)) {
		if (sec_init())
			printf("\nsec_init failed!\n");
	}
	puts("Normal Boot\n");
}

void board_init_f(ulong dummy)
{
	arch_cpu_init();
	clock_init();
	init_wdog_clk();
	ddr_init(&dram_timing);

	/* Clear global data */
	memset((void *)gd, 0, sizeof(gd_t));
	/* Clear the BSS. */
	memset(__bss_start, 0, __bss_end - __bss_start);

	enable_tzc380();

	init_uart_clk(0);
	board_early_init_f();
	preloader_console_init();

	init_usb_clk();
	init_clk_usdhc(0);
	init_clk_usdhc(1);
}
