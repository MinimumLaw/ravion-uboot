/*
 * (C) Copyright 2007-2010
 * Marcel Ziswiler, Noser Engineering, marcel.ziswiler@noser.com.
 *
 * (C) Copyright 2006-2008
 * Stefan Roese, DENX Software Engineering, sr@denx.de.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <nand.h>
#include <asm/io.h>

#define CONFIG_SYS_NAND_READ_DELAY \
	{ volatile int dummy; int i; for (i=0; i<10000; i++) dummy = i; }

#if 0
static int nand_ecc_pos[] = CONFIG_SYS_NAND_ECCPOS;
#endif

#if (CONFIG_SYS_NAND_PAGE_SIZE <= 512)
/*
 * NAND command for small page NAND devices (512)
 */
static int nand_command(struct mtd_info *mtd, int block, int page, int offs, u8 cmd)
{
	struct nand_chip *this = mtd->priv;
	int page_addr = page + block * CONFIG_SYS_NAND_PAGE_COUNT;

	if (this->dev_ready)
		while (!this->dev_ready(mtd))
			;
	else
		CONFIG_SYS_NAND_READ_DELAY;

	/* Begin command latch cycle */
	this->cmd_ctrl(mtd, cmd, NAND_CTRL_CLE | NAND_CTRL_CHANGE);
	/* Set ALE and clear CLE to start address cycle */
	/* Column address */
	this->cmd_ctrl(mtd, offs, NAND_CTRL_ALE | NAND_CTRL_CHANGE);
	this->cmd_ctrl(mtd, page_addr & 0xff, NAND_CTRL_ALE); /* A[16:9] */
	this->cmd_ctrl(mtd, (page_addr >> 8) & 0xff,
		       NAND_CTRL_ALE); /* A[24:17] */
#ifdef CONFIG_SYS_NAND_4_ADDR_CYCLE
	/* One more address cycle for devices > 32MiB */
	this->cmd_ctrl(mtd, (page_addr >> 16) & 0x0f,
		       NAND_CTRL_ALE); /* A[28:25] */
#endif
	/* Latch in address */
	this->cmd_ctrl(mtd, NAND_CMD_NONE, NAND_NCE | NAND_CTRL_CHANGE);

	/*
	 * Wait a while for the data to be ready
	 */
	if (this->dev_ready)
		while (!this->dev_ready(mtd))
			;
	else
		CONFIG_SYS_NAND_READ_DELAY;

	return 0;
}
#else
/*
 * NAND command for large page NAND devices (2k)
 */
static int nand_command(struct mtd_info *mtd, int block, int page, int offs, u8 cmd)
{
	struct nand_chip *this = mtd->priv;
	int page_addr = page + block * CONFIG_SYS_NAND_PAGE_COUNT;

#if 1
	this->cmdfunc(mtd, NAND_CMD_READ0, 0, page_addr);
#else
	if (this->dev_ready)
		while (!this->dev_ready(mtd))
			;
	else
		CONFIG_SYS_NAND_READ_DELAY;

	/* Emulate NAND_CMD_READOOB */
	if (cmd == NAND_CMD_READOOB) {
		offs += CONFIG_SYS_NAND_PAGE_SIZE;
		cmd = NAND_CMD_READ0;
	}

	/* Begin command latch cycle */
	this->cmd_ctrl(mtd, cmd, NAND_CTRL_CLE | NAND_CTRL_CHANGE);
	/* Set ALE and clear CLE to start address cycle */
	/* Column address */
	this->cmd_ctrl(mtd, offs & 0xff,
		       NAND_CTRL_ALE | NAND_CTRL_CHANGE); /* A[7:0] */
	this->cmd_ctrl(mtd, (offs >> 8) & 0xff, NAND_CTRL_ALE); /* A[11:9] */
	/* Row address */
	this->cmd_ctrl(mtd, (page_addr & 0xff), NAND_CTRL_ALE); /* A[19:12] */
	this->cmd_ctrl(mtd, ((page_addr >> 8) & 0xff),
		       NAND_CTRL_ALE); /* A[27:20] */
#ifdef CONFIG_SYS_NAND_5_ADDR_CYCLE
	/* One more address cycle for devices > 128MiB */
	this->cmd_ctrl(mtd, (page_addr >> 16) & 0x0f,
		       NAND_CTRL_ALE); /* A[31:28] */
#endif
	/* Latch in address */
	this->cmd_ctrl(mtd, NAND_CMD_READSTART,
		       NAND_CTRL_CLE | NAND_CTRL_CHANGE);
	this->cmd_ctrl(mtd, NAND_CMD_NONE, NAND_NCE | NAND_CTRL_CHANGE);

	/*
	 * Wait a while for the data to be ready
	 */
	if (this->dev_ready)
		while (!this->dev_ready(mtd))
			;
	else
		CONFIG_SYS_NAND_READ_DELAY;
#endif
	return 0;
}
#endif

#if 0
static int nand_is_bad_block(struct mtd_info *mtd, int block)
{
	struct nand_chip *this = mtd->priv;

	nand_command(mtd, block, 0, CONFIG_SYS_NAND_BAD_BLOCK_POS, NAND_CMD_READOOB);

	/*
	 * Read one byte
	 */
	if (readb(this->IO_ADDR_R) != 0xff)
		return 1;

	return 0;
}
#endif

static int nand_read_page(struct mtd_info *mtd, int block, int page, uchar *dst)
{
	struct nand_chip *this = mtd->priv;
	u_char *ecc_calc;
	u_char *ecc_code;
	u_char *oob_data;
	int i;
	int eccsize = CONFIG_SYS_NAND_ECCSIZE;
	int eccbytes = CONFIG_SYS_NAND_ECCBYTES;
	int eccsteps = CONFIG_SYS_NAND_ECCSTEPS;
	uint8_t *p = dst;
#if 0
	int stat;
#endif

	nand_command(mtd, block, page, 0, NAND_CMD_READ0);

	/* No malloc available for now, just use some temporary locations
	 * in SDRAM
	 */
	ecc_calc = (u_char *)(CONFIG_SYS_DRAM_BASE + 0x10000);
	ecc_code = ecc_calc + 0x100;
	oob_data = ecc_calc + 0x200;

	for (i = 0; eccsteps; eccsteps--, i += eccbytes, p += eccsize) {
		this->ecc.hwctl(mtd, NAND_ECC_READ);
		this->read_buf(mtd, p, eccsize);
		this->ecc.calculate(mtd, p, &ecc_calc[i]);
	}
#if 0
	this->read_buf(mtd, oob_data, CONFIG_SYS_NAND_OOBSIZE);

	/* Pick the ECC bytes out of the oob data */
	for (i = 0; i < CONFIG_SYS_NAND_ECCTOTAL; i++)
		ecc_code[i] = oob_data[nand_ecc_pos[i]];

	eccsteps = CONFIG_SYS_NAND_ECCSTEPS;
	p = dst;

	for (i = 0 ; eccsteps; eccsteps--, i += eccbytes, p += eccsize) {
		/* No chance to do something with the possible error message
		 * from correct_data(). We just hope that all possible errors
		 * are corrected by this routine.
		 */
		stat = this->ecc.correct(mtd, p, &ecc_code[i], &ecc_calc[i]);
	}
#endif
	return 0;
}

static uchar *nand_load(struct mtd_info *mtd, int offs, int size, uchar *dst)
{
	int block_count, block_index, block_start;
	int page_count, page_counter, page_index;

	/*
	 * offs has to be aligned to a page address!
	 */
	block_index = 0;
	block_start = offs / CONFIG_SYS_NAND_BLOCK_SIZE;
	page_count = ((size + CONFIG_SYS_NAND_PAGE_SIZE - 1) / CONFIG_SYS_NAND_PAGE_SIZE);
	page_counter = 0;
	page_index = (offs - (block_start * CONFIG_SYS_NAND_BLOCK_SIZE)) /
		     CONFIG_SYS_NAND_PAGE_SIZE;
	block_count = ((size - (page_index * CONFIG_SYS_NAND_PAGE_SIZE)) /
		       CONFIG_SYS_NAND_BLOCK_SIZE) + 1;

	while (block_index < block_count) {
		while ((page_index < CONFIG_SYS_NAND_PAGE_COUNT) &&
		       (page_counter < page_count)) {
			nand_read_page(mtd, block_start + block_index,
				       page_index, dst);
			page_counter++;
			page_index++;
			dst += CONFIG_SYS_NAND_PAGE_SIZE;
		}

		block_index++;
		page_index = 0;
	}

	return dst;
}

/*
 * The main entry for NAND booting. It's necessary that SDRAM is already
 * configured and available since this code loads the main U-Boot image
 * from NAND into SDRAM and starts it from there.
 */
void nand_boot(void)
{
	struct nand_chip nand_chip;
	nand_info_t nand_info;
	uchar *dst;
	__attribute__((noreturn)) void (*uboot)(void);

	/*
	 * Init board specific nand support
	 */
	nand_info.priv = &nand_chip;
	nand_chip.IO_ADDR_R = nand_chip.IO_ADDR_W = (void  __iomem *)CONFIG_SYS_NAND_BASE;
	nand_chip.dev_ready = NULL;	/* preset to NULL */
	board_nand_init(&nand_chip);

	/*
	 * Specify NAND configuration (No autodetection used!)
	 */
	nand_info.writesize = CONFIG_SYS_NAND_PAGE_SIZE;
	nand_info.oobsize = CONFIG_SYS_NAND_OOBSIZE;

	if (nand_chip.select_chip)
		nand_chip.select_chip(&nand_info, 0);

	/*
	 * Load U-Boot image from NAND into RAM
	 * In case of colibri board -- it is split into 3 parts
	 */
	dst = (uchar *)CONFIG_SYS_NAND_U_BOOT_DST;
	dst = nand_load(&nand_info, CONFIG_SYS_NAND_U_BOOT_OFFS,
			CONFIG_SYS_NAND_U_BOOT_SIZE, dst);

#if defined(CONFIG_SYS_NAND_U_BOOT_OFFS2) && defined(CONFIG_SYS_NAND_U_BOOT_SIZE2)
	dst = nand_load(&nand_info, CONFIG_SYS_NAND_U_BOOT_OFFS2,
			CONFIG_SYS_NAND_U_BOOT_SIZE2, dst);
#endif

#if defined(CONFIG_SYS_NAND_U_BOOT_OFFS3) && defined(CONFIG_SYS_NAND_U_BOOT_SIZE3)
	dst = nand_load(&nand_info, CONFIG_SYS_NAND_U_BOOT_OFFS3,
			CONFIG_SYS_NAND_U_BOOT_SIZE3, dst);
#endif


	if (nand_chip.select_chip)
		nand_chip.select_chip(&nand_info, -1);

	/*
	 * Jump to U-Boot image
	 */
	uboot = (void *)CONFIG_SYS_NAND_U_BOOT_START;
	(*uboot)();
}
