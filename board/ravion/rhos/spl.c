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

extern struct dram_timing_info dram_timing;

#ifdef CONFIG_SPL_LOAD_FIT
int board_fit_config_name_match(const char *name)
{
	/* Just empty function now - can't decide what to choose */
	printf("%s: %s\n", __func__, name);

	return 0;
}
#endif

void board_boot_order(u32 *spl_boot_list)
{
	puts("Normal boot!\n");
	spl_boot_list[0] = BOOT_DEVICE_MMC1;
	spl_boot_list[1] = BOOT_DEVICE_MMC2;
	spl_boot_list[2] = BOOT_DEVICE_USB;
	spl_boot_list[3] = BOOT_DEVICE_BOARD;
}

void board_init_f(ulong dummy)
{
	int ret;

	/* Clear global data */
	memset((void *)gd, 0, sizeof(gd_t));

	arch_cpu_init();
	clock_init();
	timer_init();

/* FixMe: Really? While this NOT done by CCF?*/
	init_usb_clk();
	init_uart_clk(0);
	init_clk_usdhc(0);
	init_clk_usdhc(1);

	spl_early_init();
	preloader_console_init();

	/* Clear the BSS. */
	memset(__bss_start, 0, __bss_end - __bss_start);

	ret = spl_init();
	if (ret) {
		debug("spl_init() failed: %d\n", ret);
		hang();
	}

	if (IS_ENABLED(CONFIG_FSL_CAAM)) {
		if (sec_init())
			printf("\nsec_init failed!\n");
	}
	enable_tzc380();

	/* DDR initialization */
	ddr_init(&dram_timing);

	board_init_r(NULL, 0);
}
