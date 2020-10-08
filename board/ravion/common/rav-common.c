/*
 * Modification: Radioavionica Corp
 * Copyright (c) 2016 Toradex, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <g_dnl.h>
#include <linux/libfdt.h>
#include <env.h>

#include "rav-cfg-block.h"
#include <asm/setup.h>
#include "rav-common.h"

#ifdef CONFIG_RAVION_CFG_BLOCK
static char rav_serial_str[9];
static char rav_board_rev_str[6];

#ifdef CONFIG_REVISION_TAG
u32 get_board_rev(void)
{
	/* Check validity */
	if (!rav_hw_tag.ver_major)
		return 0;

	return ((rav_hw_tag.ver_major & 0xff) << 8) |
		((rav_hw_tag.ver_minor & 0xf) << 4) |
		((rav_hw_tag.ver_assembly & 0xf) + 0xa);
}
#endif /* CONFIG_RAVION_CFG_BLOCK */

#ifdef CONFIG_SERIAL_TAG
void get_board_serial(struct tag_serialnr *serialnr)
{
	int array[8];
	unsigned int serial = rav_serial;
	int i;

	serialnr->low = 0;
	serialnr->high = 0;

	/* Check validity */
	if (serial) {
		/*
		 * Convert to Linux serial number format (hexadecimal coded
		 * decimal)
		 */
		i = 7;
		while (serial) {
			array[i--] = serial % 10;
			serial /= 10;
		}
		while (i >= 0)
			array[i--] = 0;
		serial = array[0];
		for (i = 1; i < 8; i++) {
			serial *= 16;
			serial += array[i];
		}

		serialnr->low = serial;
	}
}
#endif /* CONFIG_SERIAL_TAG */

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

	env_set("serial#", rav_serial_str);

	/*
	 * Check if environment contains a valid MAC address,
	 * set the one from config block if not
	 */
	if (!eth_env_get_enetaddr("ethaddr", ethaddr))
		eth_env_set_enetaddr("ethaddr", (u8 *)&rav_eth_addr);

#ifdef CONFIG_RAVION_CFG_BLOCK_2ND_ETHADDR
	if (!eth_env_get_enetaddr("eth1addr", ethaddr)) {
		/*
		 * Secondary MAC address is allocated from block
		 * 0x100000 higher then the first MAC address
		 */
		memcpy(ethaddr, &rav_eth_addr, 6);
		ethaddr[3] += 0x10;
		eth_env_set_enetaddr("eth1addr", ethaddr);
	}
#endif

	printf("Model: Ravion %s %s, Serial# %s\n",
	       ravion_modules[rav_hw_tag.prodid],
	       rav_board_rev_str,
	       rav_serial_str);

	return 0;
}

#ifdef CONFIG_USB_GADGET_DOWNLOAD
int g_dnl_bind_fixup(struct usb_device_descriptor *dev, const char *name)
{
	unsigned short usb_pid;

	usb_pid = TORADEX_USB_PRODUCT_NUM_OFFSET + rav_hw_tag.prodid;
	put_unaligned(usb_pid, &dev->idProduct);

	return 0;
}
#endif

#if defined(CONFIG_OF_LIBFDT)
int ft_common_board_setup(void *blob, struct bd_info *bd)
{
	if (rav_serial) {
		fdt_setprop(blob, 0, "serial-number", rav_serial_str,
			    strlen(rav_serial_str) + 1);
	}

	if (rav_hw_tag.ver_major) {
		char prod_id[5];

		sprintf(prod_id, "%04u", rav_hw_tag.prodid);
		fdt_setprop(blob, 0, "ravion,product-id", prod_id, 5);

		fdt_setprop(blob, 0, "ravion,board-rev", rav_board_rev_str,
			    strlen(rav_board_rev_str) + 1);
	}

	return 0;
}
#endif

#else /* CONFIG_RAVION_CFG_BLOCK */

#ifdef CONFIG_REVISION_TAG
u32 get_board_rev(void)
{
	return 0;
}
#endif /* CONFIG_REVISION_TAG */

#ifdef CONFIG_SERIAL_TAG
u32 get_board_serial(void)
{
	return 0;
}
#endif /* CONFIG_SERIAL_TAG */

int ft_common_board_setup(void *blob, bd_t *bd)
{
	return 0;
}

#endif /* CONFIG_RAVION_CFG_BLOCK */
