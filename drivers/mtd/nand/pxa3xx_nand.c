/*
 * drivers/mtd/nand/pxa3xx_nand.c
 *
 * Copyright © 2005 Intel Corporation
 * Copyright © 2006 Marvell International Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <common.h>

#if defined(CONFIG_CMD_NAND)
#ifdef CONFIG_NEW_NAND_CODE

#include <nand.h>
#include <asm/arch/pxa-regs.h>

#include <linux/mtd/compat.h>
#include <linux/types.h>
#include <asm/io.h>
#include <asm/errno.h>

#define NDCR_RD_ID_CNT_MASK	(0x7 << 16)
#define NDCR_RD_ID_CNT(x)	(((x) << 16) & NDCR_RD_ID_CNT_MASK)

#define NDSR_MASK		(0xfff)

#define NDCB0_CMD_TYPE_MASK	(0x7 << 21)
#define NDCB0_CMD_TYPE(x)	(((x) << 21) & NDCB0_CMD_TYPE_MASK)

#define NDCB0_ADDR_CYC_MASK	(0x7 << 16)
#define NDCB0_ADDR_CYC(x)	(((x) << 16) & NDCB0_ADDR_CYC_MASK)
#define NDCB0_CMD2_MASK		(0xff << 8)
#define NDCB0_CMD1_MASK		(0xff)
#define NDCB0_ADDR_CYC_SHIFT	(16)

#define msleep		mdelay
#define mdelay(n)	({unsigned long msec = (n); while (msec--) udelay(1000); })

/* error code and state */
enum {
	ERR_NONE	= 0,
	ERR_SENDCMD	= -2,
	ERR_DBERR	= -3,
	ERR_BBERR	= -4,
};

enum {
	STATE_READY	= 0,
	STATE_CMD_HANDLE,
	STATE_PIO_READING,
	STATE_PIO_WRITING,
};

/* the maximum possible buffer size for large page with OOB data
 * is: 2048 + 64 = 2112 bytes, allocate a page here for both the
 * data buffer and the DMA descriptor
 */
#define MAX_BUFF_SIZE	PAGE_SIZE

struct pxa3xx_nand_cmdset {
	uint16_t	read1;
	uint16_t	read2;
	uint16_t	program;
	uint16_t	read_status;
	uint16_t	read_id;
	uint16_t	erase;
	uint16_t	reset;
	uint16_t	lock;
	uint16_t	unlock;
	uint16_t	lock_status;
};

struct pxa3xx_nand_flash {
	const struct pxa3xx_nand_cmdset *cmdset;

	uint32_t page_per_block;/* Pages per block (PG_PER_BLK) */
	uint32_t page_size;	/* Page size in bytes (PAGE_SZ) */
	uint32_t flash_width;	/* Width of Flash memory (DWIDTH_M) */
	uint32_t dfc_width;	/* Width of flash controller(DWIDTH_C) */
	uint32_t num_blocks;	/* Number of physical blocks in Flash */
	uint32_t chip_id;
};

struct pxa3xx_nand_info {
	const struct pxa3xx_nand_flash *flash_info;

	unsigned int 		buf_start;
	unsigned int		buf_count;

	unsigned char		data_buff[MAX_BUFF_SIZE];
	size_t			data_buff_size;
	uint32_t		reg_ndcr;

	/* saved column/page_addr during CMD_SEQIN */
	int			seqin_column;
	int			seqin_page_addr;

	/* relate to the command */
	unsigned int		state;

	int			use_ecc;	/* use HW ECC ? */
	size_t			data_size;	/* data size in FIFO */
	int 			retcode;

	/* generated NDCBx register values */
	uint32_t		ndcb0;
	uint32_t		ndcb1;
	uint32_t		ndcb2;

	/* calculated from pxa3xx_nand_flash data */
	size_t		oob_size;
	size_t		read_id_bytes;

	unsigned int	col_addr_cycles;
	unsigned int	row_addr_cycles;
};

static struct pxa3xx_nand_info cmx3xx_nand_info = {
	.data_buff_size		= MAX_BUFF_SIZE,
};

static struct pxa3xx_nand_flash default_flash;

static struct pxa3xx_nand_cmdset smallpage_cmdset = {
	.read1		= 0x0000,
	.read2		= 0x0050,
	.program	= 0x1080,
	.read_status	= 0x0070,
	.read_id	= 0x0090,
	.erase		= 0xD060,
	.reset		= 0x00FF,
	.lock		= 0x002A,
	.unlock		= 0x2423,
	.lock_status	= 0x007A,
};

static struct pxa3xx_nand_cmdset largepage_cmdset = {
	.read1		= 0x3000,
	.read2		= 0x0050,
	.program	= 0x1080,
	.read_status	= 0x0070,
	.read_id	= 0x0090,
	.erase		= 0xD060,
	.reset		= 0x00FF,
	.lock		= 0x002A,
	.unlock		= 0x2423,
	.lock_status	= 0x007A,
};

#define WAIT_EVENT_TIMEOUT	(2 * CONFIG_SYS_HZ/10)

static int wait_for_event(struct pxa3xx_nand_info *info, uint32_t event)
{
	int timeout = WAIT_EVENT_TIMEOUT;
	uint32_t ndsr;

	while (timeout--) {
		ndsr = NDSR & NDSR_MASK;
		if (ndsr & event) {
			NDSR = ndsr;
			return 0;
		}
		udelay(10);
	}

	return -ETIMEDOUT;
}

static int prepare_read_prog_cmd(struct pxa3xx_nand_info *info,
			uint16_t cmd, int column, int page_addr)
{
	const struct pxa3xx_nand_flash *f = info->flash_info;
	const struct pxa3xx_nand_cmdset *cmdset = f->cmdset;

	/* calculate data size */
	switch (f->page_size) {
	case 2048:
		info->data_size = (info->use_ecc) ? 2088 : 2112;
		break;
	case 512:
		info->data_size = (info->use_ecc) ? 520 : 528;
		break;
	default:
		return -EINVAL;
	}

	/* generate values for NDCBx registers */
	info->ndcb0 = cmd | ((cmd & 0xff00) ? NDCB0_DBC : 0);
	info->ndcb1 = 0;
	info->ndcb2 = 0;
	info->ndcb0 |= NDCB0_ADDR_CYC(info->row_addr_cycles + info->col_addr_cycles);

	if (info->col_addr_cycles == 2) {
		/* large block, 2 cycles for column address
		 * row address starts from 3rd cycle
		 */
		info->ndcb1 |= page_addr << 16;
		if (info->row_addr_cycles == 3)
			info->ndcb2 = (page_addr >> 16) & 0xff;
	} else
		/* small block, 1 cycles for column address
		 * row address starts from 2nd cycle
		 */
		info->ndcb1 = page_addr << 8;

	if (cmd == cmdset->program)
		info->ndcb0 |= NDCB0_CMD_TYPE(1) | NDCB0_AUTO_RS;

	return 0;
}

static int prepare_erase_cmd(struct pxa3xx_nand_info *info,
			uint16_t cmd, int page_addr)
{
	info->ndcb0 = cmd | ((cmd & 0xff00) ? NDCB0_DBC : 0);
	info->ndcb0 |= NDCB0_CMD_TYPE(2) | NDCB0_AUTO_RS | NDCB0_ADDR_CYC(3);
	info->ndcb1 = page_addr;
	info->ndcb2 = 0;
	return 0;
}

static int prepare_other_cmd(struct pxa3xx_nand_info *info, uint16_t cmd)
{
	const struct pxa3xx_nand_cmdset *cmdset = info->flash_info->cmdset;
	info->ndcb0 = cmd | ((cmd & 0xff00) ? NDCB0_DBC : 0);
	info->ndcb1 = 0;
	info->ndcb2 = 0;

	if (cmd == cmdset->read_id) {
		info->ndcb0 |= NDCB0_CMD_TYPE(3);
		info->data_size = 8;
	} else if (cmd == cmdset->read_status) {
		info->ndcb0 |= NDCB0_CMD_TYPE(4);
		info->data_size = 8;
	} else if (cmd == cmdset->reset || cmd == cmdset->lock ||
		   cmd == cmdset->unlock) {
		info->ndcb0 |= NDCB0_CMD_TYPE(5);
	} else
		return -EINVAL;

	return 0;
}

/* calculate delta between OSCR values start and now  */
static unsigned long get_delta(unsigned long start)
{
	unsigned long cur = OSCR;

	if(cur < start) /* OSCR overflowed */
		return (cur + (start^0xffffffff));
	else
		return (cur - start);
}

/* wait_event with timeout */
static unsigned int wait_event(unsigned long event)
{
	unsigned long ndsr, timeout, start = OSCR;

	if(!event)
		return 0xff000000;

	if(event & (NDSR_CS0_CMDD | NDSR_CS0_BBD))
		timeout = CONFIG_SYS_NAND_PROG_ERASE_TO * OSCR_CLK_FREQ;
	else
		timeout = CONFIG_SYS_NAND_OTHER_TO * OSCR_CLK_FREQ;

	while(1) {
		ndsr = NDSR;
		if(ndsr & event) {
			NDSR |= event;
			break;
		}
		if(get_delta(start) > timeout) {
			printf("wait_event: TIMEOUT waiting for event: 0x%lx.\n", event);
			return 0xff000000;
		}
	}
	return ndsr;
}

/* NOTE: it is a must to set ND_RUN firstly, then write command buffer
 * otherwise, it does not work
 */
static int write_cmd(struct pxa3xx_nand_info *info)
{
	uint32_t ndcr;

	/* clear status bits and run */
	NDSR = NDSR_MASK;

	ndcr = info->reg_ndcr;
	ndcr |= info->use_ecc ? NDCR_ECC_EN : 0;
	ndcr |= NDCR_ND_RUN;

	NDCR = ndcr;

	if (wait_for_event(info, NDSR_WRCMDREQ)) {
		printk(KERN_ERR "timed out writing command\n");
		return -ETIMEDOUT;
	}

	NDCB0 = info->ndcb0;
	NDCB0 = info->ndcb1;
	NDCB0 = info->ndcb2;
	return 0;
}

static int handle_data_pio(struct pxa3xx_nand_info *info)
{
	int i = 0, j, ret;
	unsigned int aligned_data_size = info->data_size & 0xfffffffc;
	unsigned int rest_data_size = info->data_size & 0x3;
	unsigned long *long_buf;

	switch (info->state) {
	case STATE_PIO_WRITING:
		if (aligned_data_size) {
			for (i = 0; i < aligned_data_size; i += 4) {
				long_buf = (unsigned long *) &info->data_buff[i];
				NDDB = *long_buf;
			}
		}
		if (rest_data_size)
			printf("handle_data_pio: ERROR, writing non 4-byte aligned data.\n");

		if(wait_for_event(info, NDSR_CS0_CMDD)) {
			printk(KERN_ERR "program command time out\n");
			return -1;
		}

		break;
	case STATE_PIO_READING:
		if (aligned_data_size) {
			for (i = 0; i < aligned_data_size; i += 4) {
				long_buf = (unsigned long *) &info->data_buff[i];
				*long_buf = NDDB;
			}
		}
		if (rest_data_size) {
			unsigned int rest_data = NDDB;
			for(j = 0; j < rest_data_size; j++)
				info->data_buff[i + j] = (u_char) ((rest_data >> j) & 0xff);
		}

		break;
	default:
		printk(KERN_ERR "handle_data_pio: invalid state %d\n", info->state);
		return -EINVAL;
	}

	info->state = STATE_READY;
	return 0;
}

static int pxa3xx_nand_do_cmd(struct pxa3xx_nand_info *info, uint32_t event)
{
	unsigned int status;

	info->retcode = ERR_NONE;

	if (write_cmd(info)) {
		info->retcode = ERR_SENDCMD;
		printf("FAIL1\n");
		goto fail_stop;
	}

	info->state = STATE_CMD_HANDLE;

	status = wait_event(event);
	if (status & (NDSR_RDDREQ | NDSR_DBERR)) {
		if (status & NDSR_DBERR)
			info->retcode = ERR_DBERR;

		info->state = STATE_PIO_READING;
	} else if (status & NDSR_WRDREQ) {
		info->state = STATE_PIO_WRITING;
	} else if (status & (NDSR_CS0_BBD | NDSR_CS0_CMDD)) {
		if (status & NDSR_CS0_BBD)
			info->retcode = ERR_BBERR;

		info->state = STATE_READY;
	}
/*	NDSR = status; */

	if (info->data_size > 0)
		if (handle_data_pio(info)) {
			printf("FAIL2\n");
			goto fail_stop;
		}

	return 0;

fail_stop:
	NDCR &= ~NDCR_ND_RUN;
	udelay(10);
	return -ETIMEDOUT;
}

static int pxa3xx_nand_dev_ready(struct mtd_info *mtd)
{
	return NDSR & NDSR_RDY ? 1 : 0;
}

static inline int is_buf_blank(uint8_t *buf, size_t len)
{
	for (; len > 0; len--)
		if (*buf++ != 0xff)
			return 0;
	return 1;
}

static void pxa3xx_nand_cmdfunc(struct mtd_info *mtd, unsigned command,
				int column, int page_addr)
{
	struct pxa3xx_nand_info *info = &cmx3xx_nand_info;
	const struct pxa3xx_nand_cmdset *cmdset = info->flash_info->cmdset;
	int ret;

	info->use_ecc = 0;
	info->data_size = 0;
	info->state = STATE_READY;

	switch (command) {
	case NAND_CMD_READOOB:
		/* disable HW ECC to get all the OOB data */
		info->buf_count = mtd->writesize + mtd->oobsize;
		info->buf_start = mtd->writesize + column;

		if (prepare_read_prog_cmd(info, cmdset->read1, column, page_addr))
			break;

		pxa3xx_nand_do_cmd(info, NDSR_RDDREQ | NDSR_DBERR);

		/* We only are OOB, so if the data has error, does not matter */
		if (info->retcode == ERR_DBERR)
			info->retcode = ERR_NONE;
		break;

	case NAND_CMD_READ0:
		info->use_ecc = 1;
		info->retcode = ERR_NONE;
		info->buf_start = column;
		info->buf_count = mtd->writesize + mtd->oobsize;
		memset(info->data_buff, 0xFF, info->buf_count);

		if (prepare_read_prog_cmd(info, cmdset->read1, column, page_addr))
			break;

		pxa3xx_nand_do_cmd(info, NDSR_RDDREQ | NDSR_DBERR);

		if (info->retcode == ERR_DBERR) {
			/* for blank page (all 0xff), HW will calculate its ECC as
			 * 0, which is different from the ECC information within
			 * OOB, ignore such double bit errors
			 */
			if (is_buf_blank(info->data_buff, mtd->writesize))
				info->retcode = ERR_NONE;
		}
		break;
	case NAND_CMD_SEQIN:
		info->buf_start = column;
		info->buf_count = mtd->writesize + mtd->oobsize;
		memset(info->data_buff, 0xff, info->buf_count);

		/* save column/page_addr for next CMD_PAGEPROG */
		info->seqin_column = column;
		info->seqin_page_addr = page_addr;
		break;
	case NAND_CMD_PAGEPROG:
		info->use_ecc = (info->seqin_column >= mtd->writesize) ? 0 : 1;

		if (prepare_read_prog_cmd(info, cmdset->program,
				info->seqin_column, info->seqin_page_addr))
			break;

		pxa3xx_nand_do_cmd(info, NDSR_WRDREQ);
		break;
	case NAND_CMD_ERASE1:
		if (prepare_erase_cmd(info, cmdset->erase, page_addr))
			break;

		pxa3xx_nand_do_cmd(info, NDSR_CS0_BBD | NDSR_CS0_CMDD);
		break;
	case NAND_CMD_ERASE2:
		break;
	case NAND_CMD_READID:
	case NAND_CMD_STATUS:
		info->buf_start = 0;
		info->buf_count = (command == NAND_CMD_READID) ?
				info->read_id_bytes : 1;

		if (prepare_other_cmd(info, (command == NAND_CMD_READID) ?
				cmdset->read_id : cmdset->read_status))
			break;

		pxa3xx_nand_do_cmd(info, NDSR_RDDREQ);
		break;
	case NAND_CMD_RESET:
		if (prepare_other_cmd(info, cmdset->reset))
			break;

		ret = pxa3xx_nand_do_cmd(info, NDSR_CS0_CMDD);
		if (ret == 0) {
			int timeout = 2;

			while (timeout--) {
				if (NDSR & NDSR_RDY)
					break;
				msleep(10);
			}
			NDCR &= ~NDCR_ND_RUN;
		}
		break;
	default:
		printk(KERN_ERR "non-supported command.\n");
		break;
	}

	if (info->retcode == ERR_DBERR) {
		printk(KERN_ERR "double bit error @ page %08x\n", page_addr);
		info->retcode = ERR_NONE;
	}
}

static uint8_t pxa3xx_nand_read_byte(struct mtd_info *mtd)
{
	struct pxa3xx_nand_info *info = &cmx3xx_nand_info;
	char retval = 0xFF;

	if (info->buf_start < info->buf_count)
		/* Has just send a new command? */
		retval = info->data_buff[info->buf_start++];

	return retval;
}

static u16 pxa3xx_nand_read_word(struct mtd_info *mtd)
{
	struct pxa3xx_nand_info *info = &cmx3xx_nand_info;
	u16 retval = 0xFFFF;

	if (!(info->buf_start & 0x01) && info->buf_start < info->buf_count) {
		retval = *((u16 *)(info->data_buff+info->buf_start));
		info->buf_start += 2;
	}
	return retval;
}

static void pxa3xx_nand_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
{
	struct pxa3xx_nand_info *info = &cmx3xx_nand_info;
	int real_len = min_t(size_t, len, info->buf_count - info->buf_start);

	memcpy(buf, info->data_buff + info->buf_start, real_len);
	info->buf_start += real_len;
}

static void pxa3xx_nand_write_buf(struct mtd_info *mtd,
		const uint8_t *buf, int len)
{
	struct pxa3xx_nand_info *info = &cmx3xx_nand_info;
	int real_len = min_t(size_t, len, info->buf_count - info->buf_start);

	memcpy(info->data_buff + info->buf_start, buf, real_len);
	info->buf_start += real_len;
}

static int pxa3xx_nand_verify_buf(struct mtd_info *mtd,
		const uint8_t *buf, int len)
{
	return 0;
}

static void pxa3xx_nand_select_chip(struct mtd_info *mtd, int chip)
{
	return;
}

/*
 * not required for Monahans DFC
 */
static void pxa3xx_nand_cmd_ctrl(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
	return;
}

static int pxa3xx_nand_waitfunc(struct mtd_info *mtd, struct nand_chip *this)
{
	struct pxa3xx_nand_info *info = &cmx3xx_nand_info;

	/* pxa3xx_nand_send_command has waited for command complete */
	if (this->state == FL_WRITING || this->state == FL_ERASING) {
		if (info->retcode == ERR_NONE)
			return 0;
		else {
			/*
			 * any error make it return 0x01 which will tell
			 * the caller the erase and write fail
			 */
			return 0x01;
		}
	}

	return 0;
}

static void pxa3xx_nand_ecc_hwctl(struct mtd_info *mtd, int mode)
{
	return;
}

static int pxa3xx_nand_ecc_calculate(struct mtd_info *mtd,
		const uint8_t *dat, uint8_t *ecc_code)
{
	return 0;
}

static int pxa3xx_nand_ecc_correct(struct mtd_info *mtd,
		uint8_t *dat, uint8_t *read_ecc, uint8_t *calc_ecc)
{
	struct pxa3xx_nand_info *info = &cmx3xx_nand_info;
	/*
	 * Any error include ERR_SEND_CMD, ERR_DBERR, ERR_BUSERR, we
	 * consider it as a ecc error which will tell the caller the
	 * read fail We have distinguish all the errors, but the
	 * nand_read_ecc only check this function return value
	 */
	if (info->retcode != ERR_NONE)
		return -1;

	return 0;
}

static int __readid(struct pxa3xx_nand_info *info, uint32_t *id)
{
	const struct pxa3xx_nand_flash *f = info->flash_info;
	const struct pxa3xx_nand_cmdset *cmdset = f->cmdset;
	uint8_t  id_buff[8];
	int i;
	unsigned long *long_buf;

	if (prepare_other_cmd(info, cmdset->read_id)) {
		printk(KERN_ERR "failed to prepare command\n");
		return -EINVAL;
	}

	/* Send command */
	if (write_cmd(info))
		goto fail_timeout;

	/* Wait for CMDDM(command done successfully) */
	if (wait_for_event(info, NDSR_RDDREQ))
		goto fail_timeout;

	for (i = 0; i < 8; i += 4) {
		long_buf = (unsigned long *) &id_buff[i];
		*long_buf = NDDB;
	}
	*id = id_buff[0] | (id_buff[1] << 8);
	return 0;

fail_timeout:
	NDCR &= ~NDCR_ND_RUN;
	udelay(10);
	return -ETIMEDOUT;
}

static int pxa3xx_nand_config_flash(struct pxa3xx_nand_info *info,
				    const struct pxa3xx_nand_flash *f)
{
	uint32_t ndcr = 0x00000FFF; /* disable all interrupts */

	if (f->page_size != 2048 && f->page_size != 512)
		return -EINVAL;

	if (f->flash_width != 16 && f->flash_width != 8)
		return -EINVAL;

	/* calculate flash information */
	info->oob_size = (f->page_size == 2048) ? 64 : 16;
	info->read_id_bytes = (f->page_size == 2048) ? 4 : 2;

	/* calculate addressing information */
	info->col_addr_cycles = (f->page_size == 2048) ? 2 : 1;

	if (f->num_blocks * f->page_per_block > 65536)
		info->row_addr_cycles = 3;
	else
		info->row_addr_cycles = 2;

	ndcr |= NDCR_ND_ARB_EN;
	ndcr |= (info->col_addr_cycles == 2) ? NDCR_RA_START : 0;
	ndcr |= (f->page_per_block == 64) ? NDCR_PG_PER_BLK : 0;
	ndcr |= (f->page_size == 2048) ? NDCR_PAGE_SZ : 0;
	ndcr |= (f->flash_width == 16) ? NDCR_DWIDTH_M : 0;
	ndcr |= (f->dfc_width == 16) ? NDCR_DWIDTH_C : 0;

	ndcr |= NDCR_RD_ID_CNT(info->read_id_bytes);
	ndcr |= NDCR_SPARE_EN; /* enable spare by default */

	info->reg_ndcr = ndcr;

	return 0;
}

static int pxa3xx_nand_detect_config(struct pxa3xx_nand_info *info)
{
	uint32_t ndcr = NDCR;
	struct nand_flash_dev *type = NULL;
	uint32_t id = -1;
	int i;

	default_flash.page_per_block = ndcr & NDCR_PG_PER_BLK ? 64 : 32;
	default_flash.page_size = ndcr & NDCR_PAGE_SZ ? 2048 : 512;
	default_flash.flash_width = ndcr & NDCR_DWIDTH_M ? 16 : 8;
	default_flash.dfc_width = ndcr & NDCR_DWIDTH_C ? 16 : 8;

	if (default_flash.page_size == 2048)
		default_flash.cmdset = &largepage_cmdset;
	else
		default_flash.cmdset = &smallpage_cmdset;

	/* set info fields needed to __readid */
	info->flash_info = &default_flash;
	info->read_id_bytes = (default_flash.page_size == 2048) ? 4 : 2;
	info->reg_ndcr = ndcr;

	if (__readid(info, &id))
		return -ENODEV;

	/* Lookup the flash id */
	id = (id >> 8) & 0xff;		/* device id is byte 2 */

	for (i = 0; nand_flash_ids[i].name != NULL; i++) {
		if (id == nand_flash_ids[i].id) {
			type =  &nand_flash_ids[i];
			break;
		}
	}

	if (!type)
		return -ENODEV;

	/* fill the missing flash information */
	i = ffs(default_flash.page_per_block * default_flash.page_size) - 1;

	default_flash.num_blocks = type->chipsize << (20 - i);

	info->oob_size = (default_flash.page_size == 2048) ? 64 : 16;

	/* calculate addressing information */
	info->col_addr_cycles = (default_flash.page_size == 2048) ? 2 : 1;

	if (default_flash.num_blocks * default_flash.page_per_block > 65536)
		info->row_addr_cycles = 3;
	else
		info->row_addr_cycles = 2;

	return 0;
}

static int pxa3xx_nand_detect_flash(struct pxa3xx_nand_info *info)
{
	uint32_t id = -1;
	int i;

	if (pxa3xx_nand_detect_config(info) == 0)
		return 0;

	for (i = 0; i < 1; ++i) {
		if (pxa3xx_nand_config_flash(info, info->flash_info))
			continue;

		if (__readid(info, &id))
			continue;

		return 0;
	}

	printf("failed to detect configured nand flash; found %04x instead of\n", id);

	return -ENODEV;
}

static struct nand_ecclayout hw_smallpage_ecclayout = {
	.eccbytes = 6,
	.eccpos = {8, 9, 10, 11, 12, 13 },
	.oobfree = { {2, 6} }
};

static struct nand_ecclayout hw_largepage_ecclayout = {
	.eccbytes = 24,
	.eccpos = {
		40, 41, 42, 43, 44, 45, 46, 47,
		48, 49, 50, 51, 52, 53, 54, 55,
		56, 57, 58, 59, 60, 61, 62, 63},
	.oobfree = { {2, 38} }
};

int board_nand_init(struct nand_chip *this)
{
	int ret;
	struct pxa3xx_nand_info *info = &cmx3xx_nand_info;
	const struct pxa3xx_nand_flash *f;

	/* Enable NAND flash clock */
	CKENA |= CKENA_4_NAND;

	ret = pxa3xx_nand_detect_flash(info);
	if (ret) {
		printf("board_nand_init: failed to detect flash\n");
		return -ENODEV;
	}

	f = info->flash_info;

	this->options = (f->flash_width == 16) ? NAND_BUSWIDTH_16 : 0;

	this->cmd_ctrl		= pxa3xx_nand_cmd_ctrl;
	this->waitfunc		= pxa3xx_nand_waitfunc;
	this->select_chip	= pxa3xx_nand_select_chip;
	this->dev_ready		= pxa3xx_nand_dev_ready;
	this->cmdfunc		= pxa3xx_nand_cmdfunc;
	this->read_word		= pxa3xx_nand_read_word;
	this->read_byte		= pxa3xx_nand_read_byte;
	this->read_buf		= pxa3xx_nand_read_buf;
	this->write_buf		= pxa3xx_nand_write_buf;
	this->verify_buf	= pxa3xx_nand_verify_buf;

	this->ecc.mode		= NAND_ECC_HW;
	this->ecc.hwctl		= pxa3xx_nand_ecc_hwctl;
	this->ecc.calculate	= pxa3xx_nand_ecc_calculate;
	this->ecc.correct	= pxa3xx_nand_ecc_correct;
	this->ecc.size		= f->page_size;

	if (f->page_size == 2048)
		this->ecc.layout = &hw_largepage_ecclayout;
	else
		this->ecc.layout = &hw_smallpage_ecclayout;

	this->chip_delay = 25;

	return 0;
}

#else
#error "U-Boot legacy NAND support not available for Monahans DFC."
#endif
#endif
