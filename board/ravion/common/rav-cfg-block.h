/*
 * Modification: Radioavionica Corp
 * Copyright (c) 2016 Toradex, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _RAVION_CFG_BLOCK_H
#define _RAVION_CFG_BLOCK_H

#include "rav-common.h"

struct ravion_hw {
	u16 ver_major;
	u16 ver_minor;
	u16 ver_assembly;
	u16 prodid;
};

struct ravion_eth_addr {
	u32 oui:24;
	u32 nic:24;
} __attribute__((__packed__));

enum {
	RAVION_IMX6QP_2GB_IT = 1,
	RAVION_IMX6DL_2GB_IT,
	RAVION_IMX6DL_512MB_IT,
};

extern const char * const ravion_modules[];
extern bool valid_cfgblock;
extern struct ravion_hw rav_hw_tag;
extern struct ravion_eth_addr rav_eth_addr;
extern u32 rav_serial;

int read_rav_cfg_block(void);

#endif /* _RAVION_CFG_BLOCK_H */
