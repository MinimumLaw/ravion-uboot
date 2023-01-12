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
#include <fdt_support.h>

#include "rav-cfg-block.h"
#include <asm/setup.h>
#include "rav-common.h"

#ifdef CONFIG_RAVION_CFG_BLOCK

static char rav_serial_str[9];
static char rav_board_rev_str[6];

#if defined(CONFIG_OF_LIBFDT) || defined(CONFIG_SPL_OF_LIBFDT)
int ft_common_board_setup(void *blob, struct bd_info *bd)
{
	int node;
	char *bootargs;
	char *board;
	debug("SPL/U-BOOT FDT patch start.\n");

	bootargs = env_get("bootargs");
	if (bootargs) {
	    debug("bootargs set to %s\n", bootargs);
	    node = fdt_find_or_add_subnode(blob, 0, "chosen");
	    if (node > 0) {
		fdt_setprop_string(blob, node, "bootargs", bootargs);
	    };
	} else {
	    printf("bootargs enverooment not set\n");
	};

	node = fdt_node_offset_by_compatible(blob, 0, "fsl,imx6q-fec");
	if (node > 0) {
		if ( fdt_setprop(blob, node, "local-mac-address", &rav_eth_addr, 6) )
			printf("Ethernet MAC-address not set!\n");
	} else {
		printf("FEC node not found in device-tree!\n");
	};

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

	board = env_get("board");
	if (board)
		fdt_setprop_string(blob, 0, "ravion,board", board);

	debug("SPL/U-BOOT FDT patch end.\n");

	return 0;
}
#endif /* CONFIG_OF_LIBFDT */

int show_board_info(void)
{
	/* check config block present and valid */
	if (read_rav_cfg_block()) {
		printf( "No config block found!\n"
			"Module personalisation required!\n");
		return 0; /* Return 0 - no block personalisation process */
	}

	/* board serial-number */
	sprintf(rav_serial_str, "%08u", rav_serial);
	sprintf(rav_board_rev_str, "V%1d.%1d%c",
		rav_hw_tag.ver_major,
		rav_hw_tag.ver_minor,
		(char)rav_hw_tag.ver_assembly + 'A');

	/* Environment ethaddress setup */
	eth_env_set_enetaddr("ethaddr", (u8 *)&rav_eth_addr);

	/* Show board label */
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
	return -1;
}

#endif /* CONFIG_RAVION_CFG_BLOCK */
