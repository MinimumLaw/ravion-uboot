// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2009 Freescale Semiconductor, Inc.
 */

#include <common.h>
#include <init.h>
#include <asm/global_data.h>
#include <asm/io.h>
#include <asm/gpio.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/iomux-mx51.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/crm_regs.h>
#include <asm/arch/clock.h>
#include <asm/mach-imx/mx5_video.h>
#include <i2c.h>
#include <input.h>
#include <mmc.h>
#include <fsl_esdhc_imx.h>
#include <mc13892.h>
#include <usb/ehci-ci.h>

DECLARE_GLOBAL_DATA_PTR;

int dram_init(void)
{
	/* dram_init must store complete ramsize in gd->ram_size */
	gd->ram_size = get_ram_size((void *)CONFIG_SYS_SDRAM_BASE,
				PHYS_SDRAM_1_SIZE);
	return 0;
}

u32 get_board_rev(void)
{
	u32 rev = get_cpu_rev();
	if (!gpio_get_value(IMX_GPIO_NR(1, 22)))
		rev |= BOARD_REV_2_0 << BOARD_VER_OFFSET;
	return rev;
}

#define UART_PAD_CTRL	(PAD_CTL_HYS | PAD_CTL_PUS_100K_DOWN | PAD_CTL_DSE_HIGH)

static void setup_iomux_uart(void)
{
	static const iomux_v3_cfg_t uart_pads[] = {
		MX51_PAD_UART1_RXD__UART1_RXD_DTE,
		MX51_PAD_UART1_TXD__UART1_TXD,
		NEW_PAD_CTRL(MX51_PAD_UART1_RTS__UART1_RTS_DTE, UART_PAD_CTRL),
		NEW_PAD_CTRL(MX51_PAD_UART1_CTS__UART1_CTS, UART_PAD_CTRL),
	};

	imx_iomux_v3_setup_multiple_pads(uart_pads, ARRAY_SIZE(uart_pads));
}


int board_early_init_f(void)
{
	setup_iomux_uart();
	setup_iomux_lcd();

	return 0;
}

int board_init(void)
{
	/* address of boot parameters */
	gd->bd->bi_boot_params = PHYS_SDRAM_1 + 0x100;

	return 0;
}

#ifdef CONFIG_BOARD_LATE_INIT
int board_late_init(void)
{
	return 0;
}
#endif

/*
 * Do not overwrite the console
 * Use always serial for U-Boot console
 */
int overwrite_console(void)
{
	return 1;
}

int checkboard(void)
{
	puts("Board: MX511UTSVU\n");

	return 0;
}
