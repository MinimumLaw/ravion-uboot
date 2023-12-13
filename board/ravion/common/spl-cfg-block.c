/*
 * Modification: Radioavionica Corp
 * Copyright (c) 2016 Toradex, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <command.h>
#include "rav-cfg-block.h"

#if defined(CONFIG_TARGET_RAVION_IMX6)
#include <asm/arch/sys_proto.h>
#else
#define is_cpu_type(cpu) (0)
#endif
#include <cli.h>
#include <console.h>
#include <malloc.h>
#include <mmc.h>
#include <nand.h>
#include <asm/mach-types.h>
#include <asm/cache.h>

DECLARE_GLOBAL_DATA_PTR;

#define TAG_VALID	0xcf01
#define TAG_MAC		0x0000
#define TAG_HW		0x0008
#define TAG_INVALID	0xffff

#define TAG_FLAG_VALID	0x1

#if defined(CONFIG_RAVION_CFG_BLOCK_IS_IN_MMC)
#define RAVION_CFG_BLOCK_MAX_SIZE 512
#else
#error Ravion config block location not set
#endif

typedef struct cmd_tbl cmd_tbl_t;

struct ravion_tag {
	u32 len:14;
	u32 flags:2;
	u32 id:16;
};

bool valid_cfgblock;
struct ravion_hw rav_hw_tag;
struct ravion_eth_addr rav_eth_addr;
u32 rav_serial;

const char * const ravion_modules[] = {
	 [0] = "UNKNOWN MODULE",
	 [1] = "Ravion-v2 iMX6 QuadPlus 2Gb IT",
	 [2] = "Ravion-v2 iMX6 DualLite 2Gb IT",
	 [3] = "Ravion-v2 iMX6 DualLite 512Mb IT",
};

#ifdef CONFIG_RAVION_CFG_BLOCK_IS_IN_MMC
static int rav_cfg_block_mmc_storage(u8 *config_block, int write)
{
	struct mmc *mmc;
	int dev = CONFIG_RAVION_CFG_BLOCK_DEV;
	int offset = CONFIG_RAVION_CFG_BLOCK_OFFSET;
	uint part = CONFIG_RAVION_CFG_BLOCK_PART;
	uint blk_start;
	int ret = 0;

	/* Read production parameter config block from eMMC */
	mmc = find_mmc_device(dev);
	if (!mmc) {
		puts("No MMC card found\n");
		ret = -ENODEV;
		goto out;
	}
	if (part != mmc_get_blk_desc(mmc)->hwpart) {
		if (blk_select_hwpart_devnum(UCLASS_MMC, dev, part)) {
			puts("MMC partition switch failed\n");
			ret = -ENODEV;
			goto out;
		}
	}
	if (offset < 0)
		offset += mmc->capacity;
	blk_start = ALIGN(offset, mmc->read_bl_len) / mmc->read_bl_len;

	if (!write) {
		/* Careful reads a whole block of 512 bytes into config_block */
		if (blk_dread(mmc_get_blk_desc(mmc), blk_start, 1,
			      (unsigned char *)config_block) != 1) {
			ret = -EIO;
			goto out;
		}
	} else {
		/* Just writing one 512 byte block */
		if (blk_dwrite(mmc_get_blk_desc(mmc), blk_start, 1,
			       (unsigned char *)config_block) != 1) {
			ret = -EIO;
			goto out;
		}
	}

out:
	/* Switch back to regular eMMC user partition */
	blk_select_hwpart_devnum(UCLASS_MMC, 0, 0);

	return ret;
}
#endif

int read_rav_cfg_block(void)
{
	int ret = 0;
	u8 *config_block = NULL;
	struct ravion_tag *tag;
	size_t size = RAVION_CFG_BLOCK_MAX_SIZE;
	int offset;

	/* Allocate RAM area for config block */
	config_block = memalign(ARCH_DMA_MINALIGN, size);
	if (!config_block) {
		printf("Not enough malloc space available!\n");
		return -ENOMEM;
	}

	memset(config_block, 0, size);

#if defined(CONFIG_RAVION_CFG_BLOCK_IS_IN_MMC)
	ret = rav_cfg_block_mmc_storage(config_block, 0);
#else
	ret = -EINVAL;
#endif
	if (ret)
		goto out;

	/* Expect a valid tag first */
	tag = (struct ravion_tag *)config_block;
	if (tag->flags != TAG_FLAG_VALID || tag->id != TAG_VALID) {
		valid_cfgblock = false;
		ret = -EINVAL;
		goto out;
	}
	valid_cfgblock = true;
	offset = 4;

	while (offset < RAVION_CFG_BLOCK_MAX_SIZE) {
		tag = (struct ravion_tag *)(config_block + offset);
		offset += 4;
		if (tag->id == TAG_INVALID)
			break;

		if (tag->flags == TAG_FLAG_VALID) {
			switch (tag->id) {
			case TAG_MAC:
				memcpy(&rav_eth_addr, config_block + offset,
				       6);

				/* NIC part of MAC address is serial number */
				rav_serial = ntohl(rav_eth_addr.nic) >> 8;
				break;
			case TAG_HW:
				memcpy(&rav_hw_tag, config_block + offset, 8);
				break;
			}
		}

		/* Get to next tag according to current tags length */
		offset += tag->len * 4;
	}

	/* Cap product id to avoid issues with a yet unknown one */
	if (rav_hw_tag.prodid > (sizeof(ravion_modules) /
				  sizeof(ravion_modules[0])))
		rav_hw_tag.prodid = 0;

out:
	free(config_block);
	return ret;
}
