/*
 * Modification: Radioavionica Corp
 * Copyright (c) 2016 Toradex, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <g_dnl.h>
#include <linux/libfdt.h>
#include <init.h>
#include <env.h>

#include "rav-cfg-block.h"
#include <asm/setup.h>
#include "rav-common.h"

#ifdef CONFIG_RAVION_CFG_BLOCK

static char rav_serial_str[9];
static char rav_board_rev_str[6];

#if defined(CONFIG_OF_LIBFDT) || defined(CONFIG_SPL_OF_LIBFDT)
int ft_common_board_setup(void *blob, struct bd_info *bd)
{
	printf("SPL/U-BOOT FDT patch start.\n");
	/* ToDo: need setup:
	    {
		ravion,serial-number; string: done;
		ravion,product-id; string: done;
		ravion,board-rev; string: done;

		chosen {
			bootards; string
		};

		fec {
			local-mac-address; 6 byte array
		};
	 */
	if (rav_serial) {
		fdt_setprop(blob, 0, "ravion,serial-number", rav_serial_str,
			    strlen(rav_serial_str) + 1);
	}

	if (rav_hw_tag.ver_major) {
		char prod_id[5];

		sprintf(prod_id, "%04u", rav_hw_tag.prodid);
		fdt_setprop(blob, 0, "ravion,product-id", prod_id, 5);

		fdt_setprop(blob, 0, "ravion,board-rev", rav_board_rev_str,
			    strlen(rav_board_rev_str) + 1);
	}
	printf("SPL/U-BOOT FDT patch end.\n");

	return 0;
}
#endif /* CONFIG_OF_LIBFDT */

int show_board_info(void)
{
	unsigned char ethaddr[6];

	if (read_rav_cfg_block()) {
		printf("Missing Ravion config block\n");
		checkboard();
		return 0;
	}

	/* board serial-number */
	sprintf(rav_serial_str, "%08u", rav_serial);
	sprintf(rav_board_rev_str, "V%1d.%1d%c",
		rav_hw_tag.ver_major,
		rav_hw_tag.ver_minor,
		(char)rav_hw_tag.ver_assembly + 'A');

	/*
	 * Check if environment contains a valid MAC address,
	 * set the one from config block if not
	 */
	if (!eth_env_get_enetaddr("ethaddr", ethaddr))
		eth_env_set_enetaddr("ethaddr", (u8 *)&rav_eth_addr);

	printf("Model: %s %s, Serial# %s\n",
	       ravion_modules[rav_hw_tag.prodid],
	       rav_board_rev_str,
	       rav_serial_str);

	return 0;
}

#else /* CONFIG_RAVION_CFG_BLOCK */

#if defined(CONFIG_OF_LIBFDT) || defined(CONFIG_SPL_OF_LIBFDT)
int ft_common_board_setup(void *blob, struct bd_info *bd)
{
	return 0;
}
#endif /* CONFIG_OF_LIBFDT */

int show_board_info(void)
{
	printf("Bootloader not report board info!\n");
	return 0;
}

#endif /* CONFIG_RAVION_CFG_BLOCK */
