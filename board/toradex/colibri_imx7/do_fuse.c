/*
 * Copyright (C) 2016, Toradex AG
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/*
 * Helpers for i.MX OTP fusing during module production
*/

#include <common.h>
#include <fuse.h>

static int mfgr_fuse(void)
{
	unsigned val;

	fuse_sense(1, 3, &val);
	printf("Fuse 1, 3: %8x\n", val);
	if(val & 0x10000000)
	{
		puts("BT_FUSE_SEL already fused, will do nothing\n");
		return CMD_RET_FAILURE;
	}
	/* BT_FUSE_SEL & boot cfg */
	fuse_prog(1, 3, 0x10003400);
	return CMD_RET_SUCCESS;
}

int do_mfgr_fuse(cmd_tbl_t *cmdtp, int flag, int argc,
		char * const argv[])
{
	int ret;
	puts("Fusing...\n");
	ret = mfgr_fuse();
	if (ret == CMD_RET_SUCCESS)
		puts("done.\n");
	else
		puts("failed.\n");
	return ret;
}

U_BOOT_CMD(
	mfgr_fuse, 1, 0, do_mfgr_fuse,
	"OTP fusing during module production\n",
	""
);
