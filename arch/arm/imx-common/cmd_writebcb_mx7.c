/*
 * Copyright (c) 2013, Toradex AG.  All rights reserved.
 *
 * Derived from downstream U-Boot (drivers/mtd/nand/fsl_nfc.c)
 * and mxsboot
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/* NAND LAYOUT
 *
 * Content	FCB CFG_BLOCK .. FCB     CFG_BLOCK  DBBT .. DBBT
 * Block/Page	0/0 0/1       .. 0/(n-1) 1/(n-1)    0/n  .. 0/n + (n-1)
 *
 * With n = BOOT_SEARCH_COUNT
 *
 * With n = 2 the NAND is used starting at 0 with a length of 0x80000
 *
 * Usually one will use fw_start1 at 0x80000 and fw_start2 at 0x200000
 * allowing for at least 6 bad blocks in either bootloader
 */

#include <common.h>
#include <linux/bch.h>
#include <malloc.h>
#include <nand.h>
#include "rand.h"

#define PAGES_PER_STRIDE	64
#define RAND_16K		(16 * 1024)

#define BOOTLOADER_MAXSIZE	(640 * 1024)
#define BOOT_SEARCH_COUNT	2 /* match with BOOT_CFG_FUSES [6:5] */

extern int raw_access(nand_info_t *nand, ulong addr, loff_t off, ulong count,
			int read);

struct mxs_nand_fcb {
	uint32_t		checksum;
	uint32_t		fingerprint;
	uint32_t		version;
	struct {
		uint8_t			data_setup;
		uint8_t			data_hold;
		uint8_t			address_setup;
		uint8_t			dsample_time;
		uint8_t			nand_timing_state;
		uint8_t			rea;
		uint8_t			rloh;
		uint8_t			rhoh;
	}			timing;
	uint32_t		page_data_size;
	uint32_t		total_page_size;
	uint32_t		sectors_per_block;
	uint32_t		number_of_nands;		/* Ignored */
	uint32_t		total_internal_die;		/* Ignored */
	uint32_t		cell_type;			/* Ignored */
	uint32_t		ecc_block_n_ecc_type;
	uint32_t		ecc_block_0_size;
	uint32_t		ecc_block_n_size;
	uint32_t		ecc_block_0_ecc_type;
	uint32_t		metadata_bytes;
	uint32_t		num_ecc_blocks_per_page;
	uint32_t		ecc_block_n_ecc_level_sdk;	/* Ignored */
	uint32_t		ecc_block_0_size_sdk;		/* Ignored */
	uint32_t		ecc_block_n_size_sdk;		/* Ignored */
	uint32_t		ecc_block_0_ecc_level_sdk;	/* Ignored */
	uint32_t		num_ecc_blocks_per_page_sdk;	/* Ignored */
	uint32_t		metadata_bytes_sdk;		/* Ignored */
	uint32_t		erase_threshold;
	uint32_t		boot_patch;			/* not iMX7 */
	uint32_t		patch_sectors;			/* not iMX7 */
	uint32_t		firmware1_starting_sector;
	uint32_t		firmware2_starting_sector;
	uint32_t		sectors_in_firmware1;
	uint32_t		sectors_in_firmware2;
	uint32_t		dbbt_search_area_start_address;
	uint32_t		badblock_marker_byte;
	uint32_t		badblock_marker_start_bit;
	uint32_t		bb_marker_physical_offset;
	uint32_t		bch_type;			/* only iMX7 */
	uint32_t		tm_timing2_read_latency;	/* only iMX7 */
	uint32_t		tm_timing2_preambel_delay;	/* only iMX7 */
	uint32_t		tm_timing2_ce_delay;		/* only iMX7 */
	uint32_t		tm_timing2_postamble_delay;	/* only iMX7 */
	uint32_t		tm_timing2_cmd_add_pause;	/* only iMX7 */
	uint32_t		tm_timing2_data_pause;		/* only iMX7 */
	uint32_t		tm_speed;			/* only iMX7 */
	uint32_t		tm_timing1_busy_timeout;	/* only iMX7 */
	uint32_t		disbbm;
	uint32_t		bbmark_spare_offset;		/* only iMX7 */
	uint32_t		onfi_sync_enable;		/* only iMX7 */
	uint32_t		onfi_sync_speed;		/* only iMX7 */
	uint32_t		onfi_sync_nand_data;		/* only iMX7 */
	uint32_t		reserved2[6];
	uint32_t		disbbm_search;
	uint32_t		disbbm_search_limit;
	uint32_t		reserved3[15];			/* only iMX7 */
	uint32_t		read_retry_enable;		/* only iMX7 */
	uint32_t		reserved4[1];			/* only iMX7 */
	uint32_t		fill_to_1024[183];
};

struct mxs_nand_dbbt {
	uint32_t		checksum;
	uint32_t		fingerprint;
	uint32_t		version;
	uint32_t		number_bb;
	uint32_t		number_2k_pages_bb;
};

struct mxs_nand_bbt {
	uint32_t		nand;
	uint32_t		number_bb;
	uint32_t		badblock[510];
};


#ifdef DEBUG
#include <linux/ctype.h>
/* static */
void dump(const void *data, int size)
{
	int i, j;
	const uint8_t *s;

	s = data;
	for (i = j = 0; i < size; i += 16) {
		if (i)
			printf("\n");
		printf("[%04x]", i);

		for (j = i; j < i + 16; j++) {
			if (j < size)
				printf(" %02x", s[j]);
			else
				printf("   ");
			if (j == i + 7)
				printf(" ");
		}


		printf(" | ");

		for (j = i; j < i + 16; j ++) {
			if (j < size)
				printf("%c", isprint(s[j]) ? s[j] : '.');
			else
				printf(" ");
			if (j == i + 7)
				printf("-");
		}
	}
	printf("\n");
}
#endif

/*
 * reverse bit for byte
 */
static uint8_t reverse_bit(uint8_t in_byte)
{
	int i;
	uint8_t out_byte = 0;

	for (i = 0; i < 8; i++) {
		if (in_byte & ((0x80) >> i)) {
			out_byte |= 1 << i;
		}
	}

	return out_byte;
}

int encode_bch_ecc(void *source_block, size_t source_size, void *target_block,
		   size_t target_size)
{
	struct bch_control *bch;
	unsigned char *ecc_buf;
	int ecc_buf_size;
	unsigned char *tmp_buf;
	int tmp_buf_size;
	int real_buf_size;
	int i, j;
	int ecc_bit_off;
	int data_ecc_blk_size;
	int low_byte_off, low_bit_off;
	int high_byte_off, high_bit_off;
	unsigned char byte_low, byte_high;

	/* define the variables for bch algorithm*/
	/* m:  METADATABYTE */
	/* b0: BLOCK0BYTE */
	/* e0: BLOCK0ECC */
	/* bn: BLOCKNBYTE */
	/* en: BLOCKNECC */
	/* n : NUMOFBLOCKN */
	/* gf: FCB_GF */
	int m, b0, e0, bn, en, n, gf;

	/* 62 bit BCH, for i.MX6SX and i.MX7D */
	m = 32;
	b0 = 128;
	e0 = 62;
	bn = 128;
	en = 62;
	n = 7;
	gf = 13;

	/* sanity check */
	/* nand data block must be large enough for FCB structure */
	if(source_size > b0 + n * bn)
		return -EINVAL;
	/* nand page need to be large enough to contain Meta, FCB and ECC */
	if(target_size < m + b0 + e0 * gf / 8 + n * bn + n * en * gf / 8)
		return -EINVAL;

	/* init bch, using default polynomial */
	bch = init_bch(gf, en, 0);
	if(!bch)
		return -EINVAL;

	/* buffer for ecc */
	ecc_buf_size = (gf * en + 7) / 8;
	ecc_buf = malloc(ecc_buf_size);
	if(!ecc_buf)
		return -EINVAL;

	/* temp buffer to store data and ecc */
	tmp_buf_size = b0 + (e0 * gf + 7) / 8 + (bn + (en * gf + 7) / 8) * n;
	tmp_buf = malloc(tmp_buf_size);
	if(!tmp_buf)
		return -EINVAL;
	memset(tmp_buf, 0, tmp_buf_size);

	/* generate ecc code for each data block and store in temp buffer */

	for(i = 0; i < n + 1; i++) {
		memset(ecc_buf, 0, ecc_buf_size);
		encode_bch(bch, source_block + i * bn, bn, ecc_buf);

		memcpy(tmp_buf + i * (bn + ecc_buf_size), source_block + i * bn,
				bn);

		/* reverse ecc bit */
		for(j = 0; j < ecc_buf_size; j++) {
			ecc_buf[j] = reverse_bit(ecc_buf[j]);
		}

		memcpy(tmp_buf + (i + 1) * bn + i * ecc_buf_size, ecc_buf,
				ecc_buf_size);
	}

	/* store Metadata for taget block with randomizer*/
	/*memcpy(target_block, RandData, m);*/
	memset(target_block, 0, m);

	/* shift the bit to combine the source data and ecc */
	real_buf_size = (b0 * 8 + gf * e0 + (bn * 8 + gf * en) * n) / 8;

	if(!((gf * en) % 8)) {
		/* ecc data is byte aligned, just copy it. */
		memcpy(target_block + m, tmp_buf, real_buf_size);
	} else {
		/* bit offset for each ecc block */
		ecc_bit_off = 8 - (gf * en) % 8;
		/* size of a data block plus ecc block */
		data_ecc_blk_size = bn + (gf * en + 7) / 8;

		for(i = 0; i < real_buf_size; i++) {
			low_bit_off = ((i / data_ecc_blk_size) * ecc_bit_off)
					% 8;
			low_byte_off = ((i / data_ecc_blk_size) * ecc_bit_off)
					/ 8;
			high_bit_off = (((i + 1) / data_ecc_blk_size)
					* ecc_bit_off) % 8;
			high_byte_off = (((i + 1) / data_ecc_blk_size)
					* ecc_bit_off) / 8;

			byte_low = tmp_buf[i + low_byte_off] >> low_bit_off;
			byte_high = tmp_buf[i + 1 + high_byte_off]
					<< (8 - high_bit_off);

			*(uint8_t *)(target_block + i + m) = (byte_low
					| byte_high);
		}
	}

	free(ecc_buf);
	free(tmp_buf);
	return 0;
}

static void create_fcb(nand_info_t *nand, uint8_t *buf, int fw1_start_address,
		       int fw2_start_address)
{
	int i;
	struct mxs_nand_fcb _fcb;
	struct mxs_nand_fcb *fcb = &_fcb;
	int fw_size = BOOTLOADER_MAXSIZE;
	memset (fcb, 0, sizeof(struct mxs_nand_fcb));

	fcb->fingerprint =		0x20424346;
	fcb->version =			0x01000000;
	fcb->timing.data_setup =	10;
	fcb->timing.data_hold =		7;
	fcb->timing.address_setup =	15;
	fcb->timing.dsample_time =	6;
	
	fcb->page_data_size =		nand->writesize;
	fcb->total_page_size =		nand->writesize + nand->oobsize;
	fcb->sectors_per_block =	nand->erasesize / nand->writesize;

	fcb->ecc_block_n_ecc_type =	4;
	fcb->ecc_block_0_size =		512;
	fcb->ecc_block_n_size =		512;
	fcb->ecc_block_0_ecc_type =	4;
	fcb->metadata_bytes =		10;
	fcb->num_ecc_blocks_per_page =	2;

	fcb->firmware1_starting_sector = fw1_start_address / nand->writesize;
	fcb->firmware2_starting_sector = fw2_start_address / nand->writesize;

	fcb->sectors_in_firmware1 =	DIV_ROUND_UP(fw_size, 3 * 512);
	fcb->sectors_in_firmware2 =	DIV_ROUND_UP(fw_size, 3 * 512);
	fcb->dbbt_search_area_start_address =	256;
	fcb->badblock_marker_byte =	1999;
	fcb->bb_marker_physical_offset =	2048;
	/* This is typically the first byte of the pages OOB area */
	fcb->bb_marker_physical_offset = nand->writesize;
	/* workaround bug in bootrom, see errata */
	fcb->disbbm = 1;
	fcb->disbbm_search = 0;

	/* compute checksum, ~(sum of bytes starting with offset 4) */
	for (i = 4; i < sizeof(struct mxs_nand_fcb); i++)
		fcb->checksum += *((char *) fcb + i);
	fcb->checksum ^= 0xffffffff;
	encode_bch_ecc(fcb, sizeof(struct mxs_nand_fcb), buf,
		       fcb->total_page_size);
#ifdef DEBUG
	printf("FCB\n"); dump(fcb, sizeof(struct mxs_nand_fcb));
	printf("Encoded\n"); dump(buf, 512);
#endif
}

static void create_dbbt(nand_info_t *nand, uint8_t *buf)
{
	int i;
	struct mxs_nand_dbbt *dbbt = (struct mxs_nand_dbbt *)buf;

	memset (buf, 0, sizeof(struct mxs_nand_dbbt));
	dbbt->fingerprint = 0x54424244;
	dbbt->version = 0x01000000;
	dbbt->number_bb = 0;
	dbbt->number_2k_pages_bb = 0;

	/* compute checksum, ~(sum of bytes starting with offset 4) */
	for (i = 4; i < sizeof(struct mxs_nand_dbbt); i++)
		dbbt->checksum += *((char *) dbbt + i);
	dbbt->checksum ^= 0xffffffff;

#ifdef DEBUG
	printf("DBBT\n"); dump(dbbt, sizeof(struct mxs_nand_dbbt));
#endif
}

/* workaround for i.MX 7 errata e9609, use only 3/4
 * of the available chunks in a block and have an
 * FCB with a matching ecc layout and DISBBM set to 1
*/
static void write_bootloader(nand_info_t *nand, uint8_t * addr, loff_t off,
			    ulong fw_size)
{
	int i, j, ret;
	size_t maxsize;
	unsigned used_page_size, used_page_size_tmp;

	ret = 0;
	used_page_size = 3 * nand->writesize / 4;
	maxsize = nand->writesize;
	for (i = 0, j = 0; i < fw_size;
	     i += used_page_size, j += nand->writesize) {
		used_page_size_tmp = used_page_size;
		ret |= nand_write_skip_bad(nand, off + j, &used_page_size_tmp,
					  NULL, maxsize, (u_char *)addr + i,
					  WITH_WR_VERIFY);
	}
	printf("Bootloader %d bytes written to 0x%x: %s\n", (int)fw_size,
		(int) off, ret ? "ERROR" : "OK");

}

static int do_write_bcb(cmd_tbl_t *cmdtp, int flag, int argc,
			char * const argv[])
{
	int j, k, ret;
	uint8_t *buf;
	size_t rwsize, maxsize;
	ulong fw1_off, fw2_off;
	ulong off;
	nand_info_t *nand;

	int dev = nand_curr_device;

	if (argc < 2)
		return -1;

	fw1_off = simple_strtoul(argv[2], NULL, 16);
	if (argc > 2)
		fw2_off = simple_strtoul(argv[3], NULL, 16);
	else
		fw2_off = 0;

	nand = &nand_info[dev];

	/* Allocate one page, should be enought */
	rwsize = nand->writesize;
	buf = malloc(nand->writesize + nand->oobsize);

	/* Set only the first page empty... */
	memset(buf, 0xff, nand->writesize + nand->oobsize);

	create_fcb(nand, buf, fw1_off, fw2_off);

	puts("Write FCB...\n");
	rwsize = maxsize = nand->writesize;

	off = 0;
	for(j = 0; j < BOOT_SEARCH_COUNT; j++) {
		for (k = 0; k < nand->writesize + nand->oobsize; k++) {
			*(uint8_t *)(buf + k) ^=
			RandData[k + ((j * PAGES_PER_STRIDE) % 256)
			/ 64 * RAND_16K];
		}
		/* set BBM to good so we don't need nand scrub */
		*(uint8_t *)(buf + nand->writesize) = 0xff;
#ifdef DEBUG
		printf("Randomized\n"); dump(buf, 512);
#endif
		ret = raw_access(nand, (ulong) buf, off, 1, 0);
		/* revert randomizer */
		for (k = 0; k < nand->writesize + nand->oobsize; k++) {
			*(uint8_t *)(buf + k) ^=
			RandData[k + ((j * PAGES_PER_STRIDE) % 256)
			/ 64 * RAND_16K];
		}

		printf("FCB %d bytes written to 0x%x: %s\n", rwsize,
				(unsigned) off, ret ? "ERROR" : "OK");
		off += nand->erasesize;
	}
	memset (buf, 0xff, nand->writesize + nand->oobsize);
	create_dbbt(nand, buf);

	puts("Write DBBT...\n");
	off = BOOT_SEARCH_COUNT * nand->erasesize;
	for(j = 0; j < BOOT_SEARCH_COUNT; j++) {
		ret = nand_write(nand, off, &rwsize, (unsigned char *)buf);
		printf("DBBT %d bytes written to 0x%x: %s\n", rwsize,
				(unsigned) off, ret ? "ERROR" : "OK");
		off += nand->erasesize;
	}

	return 0;
}

static int do_write_boot(cmd_tbl_t *cmdtp, int flag, int argc,
			char * const argv[])
{
	uint8_t *addr;
	ulong fw1_off, fw2_off, fw_size;
	nand_info_t *nand;

	int dev = nand_curr_device;

	if (argc < 4)
		return -1;

	addr = (uint8_t *)simple_strtoul(argv[1], NULL, 16);
	fw1_off = simple_strtoul(argv[2], NULL, 16);
	if (argc > 3) {
		fw2_off = simple_strtoul(argv[3], NULL, 16);
		fw_size = simple_strtoul(argv[4], NULL, 16);
	} else {
		fw2_off = 0;
		fw_size = simple_strtoul(argv[3], NULL, 16);
	}

	/* The FCB copies BOOTLOADER_MAXSIZE into RAM, so we must not allow
	 * a bigger bootloader */
	if (fw_size > BOOTLOADER_MAXSIZE) {
		printf("ERROR: Only %d bytes are copied by bootrom to RAM, your bootloader is %d\n",
			BOOTLOADER_MAXSIZE, (int) fw_size);
		return 1;
	}
	else
		fw_size = BOOTLOADER_MAXSIZE;

	nand = &nand_info[dev];

	puts("Write bootloader...\n");
	write_bootloader(nand, addr, fw1_off, fw_size);
	if(fw2_off)
		write_bootloader(nand, addr, fw2_off, fw_size);

	return 0;
}

U_BOOT_CMD(
	writebcb, 3, 0, do_write_bcb,
	"Write Boot Control Block (FCB and DBBT)",
	"fw1-off [fw2-off]"
);

U_BOOT_CMD(
	writeboot, 5, 0, do_write_boot,
	"Write bootloadder",
	"addr fw1-off [fw2-off] fw_size"
);
