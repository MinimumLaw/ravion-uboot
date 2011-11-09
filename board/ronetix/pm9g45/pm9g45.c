/*
 * (C) Copyright 2010,2011
 * Ilko Iliev <iliev@ronetix.at>
 * Asen Dimov <dimov@ronetix.at>
 * Ronetix GmbH <www.ronetix.at>
 *
 * (C) Copyright 2007-2008
 * Stelian Pop <stelian.pop@leadtechdesign.com>
 * Lead Tech Design <www.leadtechdesign.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/sizes.h>
#include <asm/arch/at91sam9g45.h>
#include <asm/arch/at91sam9_smc.h>
#include <asm/arch/at91_common.h>
#include <asm/arch/at91_pmc.h>
#include <asm/arch/at91_rstc.h>
#include <asm/arch/at91_matrix.h>
#include <asm/arch/at91_pio.h>
#include <asm/arch/clk.h>
#include <asm/arch/io.h>
#include <asm/arch/hardware.h>
#ifdef CONFIG_HAS_DATAFLASH
#include <dataflash.h>
#endif
#ifdef CONFIG_LCD
#include <lcd.h>
#include <atmel_lcdc.h>
#endif
#ifdef CONFIG_LCD_INFO
#include <nand.h>
#include <version.h>
#endif
#if defined(CONFIG_RESET_PHY_R) && defined(CONFIG_MACB)
#include <net.h>
#endif
#include <netdev.h>
#include <ds2401.h>

DECLARE_GLOBAL_DATA_PTR;

/*
 * Miscelaneous platform dependent initialisations
 */
#ifdef CONFIG_HAS_DATAFLASH
AT91S_DATAFLASH_INFO dataflash_info[CONFIG_SYS_MAX_DATAFLASH_BANKS];

struct dataflash_addr cs[CONFIG_SYS_MAX_DATAFLASH_BANKS] = {
       {CONFIG_SYS_DATAFLASH_LOGIC_ADDR_CS0, 0},       /* Logical adress, CS */
};

/*define the area offsets*/
#ifdef CONFIG_SYS_USE_DATAFLASH
dataflash_protect_t area_list[NB_DATAFLASH_AREA] = {
       {0x00000000, 0x000041FF, FLAG_PROTECT_SET,   0, "Bootstrap"},
       {0x00004200, 0x000083FF, FLAG_PROTECT_CLEAR, 0, "Environment"},
       {0x00008400, 0x00041FFF, FLAG_PROTECT_SET,   0, "U-Boot"},
       {0x00042000, 0x00251FFF, FLAG_PROTECT_CLEAR, 0, "Kernel"},
       {0x00252000, 0xFFFFFFFF, FLAG_PROTECT_CLEAR, 0, "FS"},
};
#else
dataflash_protect_t area_list[NB_DATAFLASH_AREA] = {
       {0x00000000, 0xFFFFFFFF, FLAG_PROTECT_CLEAR,   0, ""},
};
#endif
#endif /* CONFIG_HAS_DATAFLASH */

#ifdef CONFIG_DS2401

static void ds2401_hw_init(void)
{
	at91_set_pio_output(CONFIG_DS2401_PIN, 1); /* output */
	at91_set_pio_multi_drive(CONFIG_DS2401_PIN, 1); /* open drain */
}

static void ds2401_set(int value)
{
	at91_set_pio_value(CONFIG_DS2401_PIN, value);
}

static int ds2401_get(void)
{
	return at91_get_pio_value(CONFIG_DS2401_PIN);
}

DS2401_FUNCS DS2401_funcs = {
	.set = ds2401_set,
	.get = ds2401_get
};
#endif

#ifdef CONFIG_CMD_NAND
static void pm9g45_nand_hw_init(void)
{
	unsigned long csa;
	at91_smc_t	*smc	= (at91_smc_t *) AT91_SMC_BASE;
	at91_matrix_t	*matrix = (at91_matrix_t *) AT91_MATRIX_BASE;
	at91_pmc_t	*pmc	= (at91_pmc_t *) AT91_PMC_BASE;

	/* Enable CS3 */
	csa = readl(&matrix->ccr[6]) | AT91_MATRIX_CSA_EBI_CS3A;
	writel(csa, &matrix->ccr[6]);

	/* Configure SMC CS3 for NAND/SmartMedia */
	writel(AT91_SMC_SETUP_NWE(1) | AT91_SMC_SETUP_NCS_WR(0) |
		AT91_SMC_SETUP_NRD(1) | AT91_SMC_SETUP_NCS_RD(0),
		&smc->cs[3].setup);

	writel(AT91_SMC_PULSE_NWE(4) | AT91_SMC_PULSE_NCS_WR(3) |
		AT91_SMC_PULSE_NRD(3) | AT91_SMC_PULSE_NCS_RD(2),
		&smc->cs[3].pulse);

	writel(AT91_SMC_CYCLE_NWE(7) | AT91_SMC_CYCLE_NRD(4),
		&smc->cs[3].cycle);

	writel(AT91_SMC_MODE_RM_NRD | AT91_SMC_MODE_WM_NWE |
		AT91_SMC_MODE_EXNW_DISABLE |
		AT91_SMC_MODE_DBW_8 |
		AT91_SMC_MODE_TDF_CYCLE(3),
		&smc->cs[3].mode);

	writel(1 << AT91SAM9G45_ID_PIOC, &pmc->pcer);

#ifdef CONFIG_SYS_NAND_READY_PIN
	/* Configure RDY/BSY */
	at91_set_pio_input(CONFIG_SYS_NAND_READY_PIN, 1);
#endif

	/* Enable NandFlash */
	at91_set_pio_output(CONFIG_SYS_NAND_ENABLE_PIN, 1);
}
#endif

#ifdef CONFIG_MACB
static void pm9g45_macb_hw_init(void)
{
	at91_pmc_t	*pmc	= (at91_pmc_t *) AT91_PMC_BASE;
	at91_pio_t	*pio	= (at91_pio_t *) AT91_PIO_BASE;
	at91_rstc_t	*rstc	= (at91_rstc_t *) AT91_RSTC_BASE;
	unsigned long	erstl;

	/*
	 * PD2 enables the 50MHz oscillator for Ethernet PHY
	 * 1 - enable
	 * 0 - disable
	 */
	at91_set_pio_output(AT91_PIO_PORTD, 2, 1);
	at91_set_pio_value(AT91_PIO_PORTD, 2, 1); /* 1- enable, 0 - disable */

	/* Enable clock */
	writel(1 << AT91SAM9G45_ID_EMAC, &pmc->pcer);

/* On BaseBoard BB9G45 no additional reset is needed,
 * but on PM9263 it is needed. if CONFIG_RESET_PHY_R
 * is defined an reset will accrue.
 */
#ifdef CONFIG_RESET_PHY_R
	/*
	 * Disable pull-up on:
	 *	RXDV (PA15) => PHY normal mode (not Test mode)
	 *	ERX0 (PA12) => PHY ADDR0
	 *	ERX1 (PA13) => PHY ADDR1 => PHYADDR = 0x0
	 *
	 * PHY has internal pull-down
	 */
	at91_set_pio_pullup(AT91_PIO_PORTA, 15, 0);
	at91_set_pio_pullup(AT91_PIO_PORTA, 12, 0);
	at91_set_pio_pullup(AT91_PIO_PORTA, 13, 0);

	erstl = readl(&rstc->mr) & AT91_RSTC_MR_ERSTL_MASK;

	/* Need to reset PHY -> 500ms reset */
	writel(AT91_RSTC_KEY | AT91_RSTC_MR_ERSTL(0x0D) |
		AT91_RSTC_MR_URSTEN, &rstc->mr);

	writel(AT91_RSTC_KEY | AT91_RSTC_CR_EXTRST, &rstc->cr);
	/* Wait for end hardware reset */
	while (!(readl(&rstc->sr) & AT91_RSTC_SR_NRSTL));

	/* Restore NRST value and enable user reset */
	writel(AT91_RSTC_KEY | erstl | AT91_RSTC_MR_URSTEN, &rstc->mr);
	/* Re-enable pull-up */
	at91_set_pio_pullup(AT91_PIO_PORTA, 15, 1);
	at91_set_pio_pullup(AT91_PIO_PORTA, 12, 1);
	at91_set_pio_pullup(AT91_PIO_PORTA, 13, 1);

#else
	/* Enable user reset */
	erstl = readl(&rstc->mr);
	writel(AT91_RSTC_KEY | erstl | AT91_RSTC_MR_URSTEN, &rstc->mr);
#endif

	at91_macb_hw_init();
}
#endif

#ifdef CONFIG_LCD
#if defined(CONFIG_BB9263)
/*
 * LCD name TX09D50VM1CCA
 */
vidinfo_t panel_info = {
	vl_col:		240,
	vl_row:		320,
	vl_clk:		4965000,
	vl_sync:	ATMEL_LCDC_INVLINE_NORMAL |
			ATMEL_LCDC_INVFRAME_NORMAL,
	vl_bpix:	3,
	vl_tft:		1,
	vl_hsync_len:	5,
	vl_left_margin:	1,
	vl_right_margin:33,
	vl_vsync_len:	1,
	vl_upper_margin:1,
	vl_lower_margin:0,
	mmio:		AT91SAM9G45_LCDC_BASE,
};
#elif defined(CONFIG_BB9G45)
/*
 * LCD name GATW70SN8H1E0
 */
vidinfo_t panel_info = {
	vl_col:		800,
	vl_row:		480,
	vl_clk:		25000000,
	vl_sync:	ATMEL_LCDC_INVLINE_NORMAL |
			ATMEL_LCDC_INVFRAME_NORMAL,
	vl_bpix:	3,
	vl_tft:		1,
	vl_hsync_len:	48,
	vl_left_margin:	1,
	vl_right_margin:33,
	vl_vsync_len:	3,
	vl_upper_margin:29,
	vl_lower_margin:13,
	mmio:		AT91SAM9G45_LCDC_BASE,
};
#endif

void lcd_enable(void)
{
	at91_set_a_periph(AT91_PIO_PORTE, 6, 1);	/* power up */
}

void lcd_disable(void)
{
	at91_set_a_periph(AT91_PIO_PORTE, 6, 0);	/* power down */
}

static void pm9g45_lcd_hw_init(void)
{
	at91_pmc_t	*pmc	= (at91_pmc_t *) AT91_PMC_BASE;

	at91_set_a_periph(AT91_PIO_PORTE, 0, 0);	/* LCDDPWR */
	at91_set_a_periph(AT91_PIO_PORTE, 2, 0);	/* LCDCC */
	at91_set_a_periph(AT91_PIO_PORTE, 3, 0);	/* LCDVSYNC */
	at91_set_a_periph(AT91_PIO_PORTE, 4, 0);	/* LCDHSYNC */
	at91_set_a_periph(AT91_PIO_PORTE, 5, 0);	/* LCDDOTCK */

	at91_set_a_periph(AT91_PIO_PORTE, 7, 0);	/* LCDD0 */
	at91_set_a_periph(AT91_PIO_PORTE, 8, 0);	/* LCDD1 */
	at91_set_a_periph(AT91_PIO_PORTE, 9, 0);	/* LCDD2 */
	at91_set_a_periph(AT91_PIO_PORTE, 10, 0);	/* LCDD3 */
	at91_set_a_periph(AT91_PIO_PORTE, 11, 0);	/* LCDD4 */
	at91_set_a_periph(AT91_PIO_PORTE, 12, 0);	/* LCDD5 */
	at91_set_a_periph(AT91_PIO_PORTE, 13, 0);	/* LCDD6 */
	at91_set_a_periph(AT91_PIO_PORTE, 14, 0);	/* LCDD7 */
	at91_set_a_periph(AT91_PIO_PORTE, 15, 0);	/* LCDD8 */
	at91_set_a_periph(AT91_PIO_PORTE, 16, 0);	/* LCDD9 */
	at91_set_a_periph(AT91_PIO_PORTE, 17, 0);	/* LCDD10 */
	at91_set_a_periph(AT91_PIO_PORTE, 18, 0);	/* LCDD11 */
	at91_set_a_periph(AT91_PIO_PORTE, 19, 0);	/* LCDD12 */
	at91_set_b_periph(AT91_PIO_PORTE, 20, 0);	/* LCDD13 */
	at91_set_a_periph(AT91_PIO_PORTE, 21, 0);	/* LCDD14 */
	at91_set_a_periph(AT91_PIO_PORTE, 22, 0);	/* LCDD15 */
	at91_set_a_periph(AT91_PIO_PORTE, 23, 0);	/* LCDD16 */
	at91_set_a_periph(AT91_PIO_PORTE, 24, 0);	/* LCDD17 */
	at91_set_a_periph(AT91_PIO_PORTE, 25, 0);	/* LCDD18 */
	at91_set_a_periph(AT91_PIO_PORTE, 26, 0);	/* LCDD19 */
	at91_set_a_periph(AT91_PIO_PORTE, 27, 0);	/* LCDD20 */
	at91_set_b_periph(AT91_PIO_PORTE, 28, 0);	/* LCDD21 */
	at91_set_a_periph(AT91_PIO_PORTE, 29, 0);	/* LCDD22 */
	at91_set_a_periph(AT91_PIO_PORTE, 30, 0);	/* LCDD23 */

	writel(1 << AT91SAM9G45_ID_LCDC, &pmc->pcer);

	gd->fb_base = CONFIG_AT91SAM9G45_LCD_BASE;
}

#ifdef CONFIG_LCD_INFO

void lcd_show_board_info(void)
{
	ulong dram_size, nand_size;
#ifdef CONFIG_HAS_DATAFLASH
	ulong dataflash_size;
#endif
	int i;
	char temp[32];

	lcd_printf ("%s\n", U_BOOT_VERSION);
	lcd_printf ("(C) 2010 Ronetix GmbH\n");
	lcd_printf ("support@ronetix.at\n");
	lcd_printf ("%s CPU at %s MHz\n",
		CONFIG_SYS_AT91_CPU_NAME,
		strmhz(temp, get_cpu_clk_rate()));

	dram_size = 0;
	for (i = 0; i < CONFIG_NR_DRAM_BANKS; i++)
		dram_size += gd->bd->bi_dram[i].size;

	nand_size = 0;
	for (i = 0; i < CONFIG_SYS_MAX_NAND_DEVICE; i++)
		nand_size += nand_info[i].size;

#ifdef CONFIG_HAS_DATAFLASH
	dataflash_size = 0;
	for (i = 0; i < CONFIG_SYS_MAX_DATAFLASH_BANKS; i++) {
		dataflash_size += (unsigned int)
				dataflash_info[i].Device.pages_number *
				dataflash_info[i].Device.pages_size;
	}
#endif

	lcd_printf ("%ld MB DDR2 SDRAM\n%ld MB NAND\n",
		dram_size >> 20,
		nand_size >> 20);

#ifdef CONFIG_HAS_DATAFLASH
	if ((dataflash_size >> 20) > 0)
		lcd_printf ("%ld MB DataFlash\n",
			dataflash_size >> 20);
#endif
}
#endif /* CONFIG_LCD_INFO */
#endif

int board_init(void)
{
	at91_pmc_t	*pmc	= (at91_pmc_t *) AT91_PMC_BASE;

	/* Enable Ctrlc */
	console_init_f();

	writel((1 << AT91SAM9G45_ID_PIOA) |
		(1 << AT91SAM9G45_ID_PIOB) |
		(1 << AT91SAM9G45_ID_PIOC) |
		(1 << AT91SAM9G45_ID_PIODE), &pmc->pcer);

	/* arch number of AT91SAM9M10G45EK-Board */
	gd->bd->bi_arch_number = MACH_TYPE_PM9G45;
	/* adress of boot parameters */
	gd->bd->bi_boot_params = PHYS_SDRAM + 0x100;

	at91_serial_hw_init();
#ifdef CONFIG_CMD_NAND
	pm9g45_nand_hw_init();
#endif

#ifdef CONFIG_HAS_DATAFLASH
	at91_spi0_hw_init(1 << 0);
#endif
#ifdef CONFIG_ATMEL_SPI
	at91_spi0_hw_init(1 << 0);
#endif

#ifdef CONFIG_DS2401
	ds2401_hw_init();
	ds2401_init(&DS2401_funcs);
#endif

#ifdef CONFIG_MACB
	pm9g45_macb_hw_init();
#endif

#ifdef CONFIG_LCD
	pm9g45_lcd_hw_init();
#endif
	return 0;
}

int dram_init(void)
{
	gd->bd->bi_dram[0].start = PHYS_SDRAM;
	gd->bd->bi_dram[0].size = PHYS_SDRAM_SIZE;
	return 0;
}

#ifdef CONFIG_RESET_PHY_R
void reset_phy(void)
{
#ifdef CONFIG_MACB
	/*
	 * Initialize ethernet HW addr prior to starting Linux,
	 * needed for nfsroot
	 */
	eth_init(gd->bd);
#endif
}
#endif

int board_eth_init(bd_t *bis)
{
	int rc = 0;
#ifdef CONFIG_MACB
	/*
	 * Setup the MAC address. If you want to setup the MAC address
	 * using the 'onewire' console command comment out the following
	 * line
	 */
	do_ds2401(NULL, 0, 0, NULL);

	rc = macb_eth_initialize(0, (void *)AT91_EMAC_BASE, 0x01);
#endif
	return rc;
}

/* SPI chip select control */
#ifdef CONFIG_ATMEL_SPI
#include <spi.h>

int spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
	return bus == 0 && cs < 2;
}

void spi_cs_activate(struct spi_slave *slave)
{
	switch(slave->cs) {
		case 1:
			at91_set_pio_output(AT91_PIO_PORTB, 18, 0);
			at91_set_pio_value(AT91_PIO_PORTB, 18, 0);
			break;
		case 0:
		default:
			at91_set_pio_output(AT91_PIO_PORTB, 3, 0);
			at91_set_pio_value(AT91_PIO_PORTB, 3, 0);
			break;
	}
}

void spi_cs_deactivate(struct spi_slave *slave)
{
	switch(slave->cs) {
		case 1:
			at91_set_pio_output(AT91_PIO_PORTB, 18, 1);
			at91_set_pio_value(AT91_PIO_PORTB, 18, 1);
			break;
		case 0:
		default:
			at91_set_pio_output(AT91_PIO_PORTB, 3, 1);
			at91_set_pio_value(AT91_PIO_PORTB, 3, 1);
		break;
	}
}
#endif /* CONFIG_ATMEL_SPI */
