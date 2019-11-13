/*
 * Copyright (C) 2010-2013 Freescale Semiconductor, Inc.
 * Copyright (C) 2013, Boundary Devices <info@boundarydevices.com>
 * Copyright (C) 2014-2016, Toradex AG
 * copied from nitrogen6x
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <dm.h>
#include <asm/arch/clock.h>
#include <asm/arch/crm_regs.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/iomux.h>
#include <asm/arch/mx6-pins.h>
#include <asm/arch/mx6-ddr.h>
#include <asm/arch/mxc_hdmi.h>
#include <asm/arch/sys_proto.h>
#include <asm/bootm.h>
#include <asm/gpio.h>
#include <asm/mach-imx/iomux-v3.h>
#include <asm/mach-imx/boot_mode.h>
#include <asm/mach-imx/video.h>
#include <asm/io.h>
#include <dm/platdata.h>
#include <input.h>
#include <imx_thermal.h>
#include <linux/errno.h>
#include <malloc.h>
#include <micrel.h>
#include <miiphy.h>
#include <netdev.h>

#include "../common/rav-cfg-block.h"

DECLARE_GLOBAL_DATA_PTR;

#define ENET_PAD_CTRL  (PAD_CTL_PUS_100K_UP |			\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm | PAD_CTL_HYS)

#define SPI_PAD_CTRL (PAD_CTL_HYS | PAD_CTL_SPEED_MED |		\
	PAD_CTL_DSE_40ohm     | PAD_CTL_SRE_FAST)

#define BUTTON_PAD_CTRL (PAD_CTL_PUS_100K_UP |			\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm | PAD_CTL_HYS)

#define WEAK_PULLUP	(PAD_CTL_PUS_100K_UP |			\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm | PAD_CTL_HYS |	\
	PAD_CTL_SRE_SLOW)

#define NO_PULLUP	(					\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm | PAD_CTL_HYS |	\
	PAD_CTL_SRE_SLOW)

#define WEAK_PULLDOWN	(PAD_CTL_PUS_100K_DOWN |		\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm |			\
	PAD_CTL_HYS | PAD_CTL_SRE_SLOW)

#define OUTPUT_40OHM (PAD_CTL_SPEED_MED|PAD_CTL_DSE_40ohm)

#define OUTPUT_RGB (PAD_CTL_SPEED_MED|PAD_CTL_DSE_60ohm|PAD_CTL_SRE_FAST)

int dram_init(void)
{
	/* use the DDR controllers configured size */
	gd->ram_size = get_ram_size((void *)CONFIG_SYS_SDRAM_BASE,
				    (ulong)imx_ddr_size());

	return 0;
}

#if defined(CONFIG_VIDEO_IPUV3)

static void do_enable_hdmi(struct display_info_t const *dev)
{
	imx_setup_hdmi();
	imx_enable_hdmi_phy();
}

static iomux_v3_cfg_t const disp_ctrl_pads[] = {
	MX6_PAD_EIM_OE__GPIO2_IO25 | MUX_PAD_CTRL(NO_PAD_CTRL),		/* soDimm110 */
#define PKK_LVDS_SHDN_GP IMX_GPIO_NR(2, 25)
	MX6_PAD_KEY_COL0__GPIO4_IO06 | MUX_PAD_CTRL(NO_PAD_CTRL),	/* soDimm124 */
#define DISP_EN_GP IMX_GPIO_NR(4, 6)
	MX6_PAD_DISP0_DAT6__GPIO4_IO27 | MUX_PAD_CTRL(NO_PAD_CTRL),	/* soDimm184 */
#define DISP_SD_GP IMX_GPIO_NR(4, 27)
};

static void mtu_display_init(void)
{
	imx_iomux_v3_setup_multiple_pads(disp_ctrl_pads,
		ARRAY_SIZE(disp_ctrl_pads));
	gpio_direction_output(DISP_SD_GP, 1);
	gpio_direction_output(DISP_EN_GP, 1);
	debug("MTU control!\n");
}

static void pkk_m7_display_init(void)
{
	imx_iomux_v3_setup_multiple_pads(disp_ctrl_pads,
		ARRAY_SIZE(disp_ctrl_pads));
	gpio_direction_output(PKK_LVDS_SHDN_GP, 1);
	gpio_direction_output(DISP_SD_GP, 0);
	gpio_direction_output(DISP_EN_GP, 1);
	debug("PKK-M7 control!\n");
}

static void pkk_m10_display_init(void)
{
	imx_iomux_v3_setup_multiple_pads(disp_ctrl_pads,
		ARRAY_SIZE(disp_ctrl_pads));
	gpio_direction_output(PKK_LVDS_SHDN_GP, 1);
	gpio_direction_output(DISP_SD_GP, 0);
	gpio_direction_output(DISP_EN_GP, 1);
	debug("PKK-M10 control!\n");
}

static iomux_v3_cfg_t const backlight_pads[] = {
	MX6_PAD_NANDF_D4__GPIO2_IO04 | MUX_PAD_CTRL(NO_PAD_CTRL),	/* soDimm119 */
#define BACKLIGHT_GP IMX_GPIO_NR(2, 4)
	MX6_PAD_SD4_DAT1__GPIO2_IO09 | MUX_PAD_CTRL(NO_PAD_CTRL),	/* soDimm28 */
#define BACKLIGHTPWM_GP IMX_GPIO_NR(2, 9)
};

static void common_backlight_init(void)
{
	imx_iomux_v3_setup_multiple_pads(backlight_pads,
		ARRAY_SIZE(backlight_pads));
	gpio_direction_output(BACKLIGHT_GP, 0);
	gpio_direction_output(BACKLIGHTPWM_GP, 1);
	debug("Backlight!\n");
}

static iomux_v3_cfg_t const led_pads[] = {
	/* Status Blue Led on Router */
	MX6_PAD_KEY_ROW0__GPIO4_IO07 | MUX_PAD_CTRL(NO_PAD_CTRL),	/* soDimm134 */
#define BLUE_LED_GP IMX_GPIO_NR(4, 7)
};

static void turn_on_blue_led(void)
{
	imx_iomux_v3_setup_multiple_pads(
		led_pads,
		ARRAY_SIZE(led_pads));
	gpio_direction_output(BLUE_LED_GP, 0);
}

static iomux_v3_cfg_t const rgb_pads[] = {
	MX6_PAD_EIM_A16__IPU1_DI1_DISP_CLK | MUX_PAD_CTRL(OUTPUT_RGB), /* soDimm56  TFT_PCLK  */
	MX6_PAD_EIM_DA10__IPU1_DI1_PIN15   | MUX_PAD_CTRL(OUTPUT_RGB), /* soDimm44  TFT_DE    */
	MX6_PAD_EIM_DA11__IPU1_DI1_PIN02   | MUX_PAD_CTRL(OUTPUT_RGB), /* soDimm68  TFT_HSYNC */
	MX6_PAD_EIM_DA12__IPU1_DI1_PIN03   | MUX_PAD_CTRL(OUTPUT_RGB), /* soDimm82  TFT_VSYNC */
	MX6_PAD_EIM_DA9__IPU1_DISP1_DATA00 | MUX_PAD_CTRL(OUTPUT_RGB), /* soDimm76  TFT_DAT00 */
	MX6_PAD_EIM_DA8__IPU1_DISP1_DATA01 | MUX_PAD_CTRL(OUTPUT_RGB), /* soDimm70  TFT_DAT01 */
	MX6_PAD_EIM_DA7__IPU1_DISP1_DATA02 | MUX_PAD_CTRL(OUTPUT_RGB), /* soDimm60  TFT_DAT02 */
	MX6_PAD_EIM_DA6__IPU1_DISP1_DATA03 | MUX_PAD_CTRL(OUTPUT_RGB), /* soDimm58  TFT_DAT03 */
	MX6_PAD_EIM_DA5__IPU1_DISP1_DATA04 | MUX_PAD_CTRL(OUTPUT_RGB), /* soDimm78  TFT_DAT04 */
	MX6_PAD_EIM_DA4__IPU1_DISP1_DATA05 | MUX_PAD_CTRL(OUTPUT_RGB), /* soDimm72  TFT_DAT05 */
	MX6_PAD_EIM_DA3__IPU1_DISP1_DATA06 | MUX_PAD_CTRL(OUTPUT_RGB), /* soDimm80  TFT_DAT06 */
	MX6_PAD_EIM_DA2__IPU1_DISP1_DATA07 | MUX_PAD_CTRL(OUTPUT_RGB), /* soDimm46  TFT_DAT07 */
	MX6_PAD_EIM_DA1__IPU1_DISP1_DATA08 | MUX_PAD_CTRL(OUTPUT_RGB), /* soDimm62  TFT_DAT08 */
	MX6_PAD_EIM_DA0__IPU1_DISP1_DATA09 | MUX_PAD_CTRL(OUTPUT_RGB), /* soDimm48  TFT_DAT09 */
	MX6_PAD_EIM_EB1__IPU1_DISP1_DATA10 | MUX_PAD_CTRL(OUTPUT_RGB), /* soDimm74  TFT_DAT10 */
	MX6_PAD_EIM_EB0__IPU1_DISP1_DATA11 | MUX_PAD_CTRL(OUTPUT_RGB), /* soDimm50  TFT_DAT11 */
	MX6_PAD_EIM_A17__IPU1_DISP1_DATA12 | MUX_PAD_CTRL(OUTPUT_RGB), /* soDimm52  TFT_DAT12 */
	MX6_PAD_EIM_A18__IPU1_DISP1_DATA13 | MUX_PAD_CTRL(OUTPUT_RGB), /* soDimm54  TFT_DAT13 */
	MX6_PAD_EIM_A19__IPU1_DISP1_DATA14 | MUX_PAD_CTRL(OUTPUT_RGB), /* soDimm66  TFT_DAT14 */
	MX6_PAD_EIM_A20__IPU1_DISP1_DATA15 | MUX_PAD_CTRL(OUTPUT_RGB), /* soDimm64  TFT_DAT15 */
	MX6_PAD_EIM_A21__IPU1_DISP1_DATA16 | MUX_PAD_CTRL(OUTPUT_RGB), /* soDimm57  TFT_DAT16 */
	MX6_PAD_EIM_A22__IPU1_DISP1_DATA17 | MUX_PAD_CTRL(OUTPUT_RGB), /* soDimm61  TFT_DAT17 */
	/* 24 bit video */
	MX6_PAD_EIM_A23__IPU1_DISP1_DATA18 | MUX_PAD_CTRL(OUTPUT_RGB), /* soDimm136 TFT_DAT18 */
	MX6_PAD_EIM_A24__IPU1_DISP1_DATA19 | MUX_PAD_CTRL(OUTPUT_RGB), /* soDimm138 TFT_DAT19 */
	MX6_PAD_EIM_D31__IPU1_DISP1_DATA20 | MUX_PAD_CTRL(OUTPUT_RGB), /* soDimm140 TFT_DAT20 */
	MX6_PAD_EIM_D30__IPU1_DISP1_DATA21 | MUX_PAD_CTRL(OUTPUT_RGB), /* soDimm142 TFT_DAT21 */
	MX6_PAD_EIM_D26__IPU1_DISP1_DATA22 | MUX_PAD_CTRL(OUTPUT_RGB), /* soDimm144 TFT_DAT22 */
	MX6_PAD_EIM_D27__IPU1_DISP1_DATA23 | MUX_PAD_CTRL(OUTPUT_RGB), /* soDimm146 TFT_DAT23 */
};

#if defined CONFIG_RAVION_DISPLAY_KOE
/* Defined on common/koe_init.c */
extern void turn_on_koe_display(void);
#endif /* defined CONFIG_RAVION_DISPLAY_KOE */

static void enable_rgb(struct display_info_t const *dev)
{
	imx_iomux_v3_setup_multiple_pads(
		rgb_pads, ARRAY_SIZE(rgb_pads));

#if defined CONFIG_RAVION_DISPLAY_KOE
	char *board = env_get("board");
	if(!strcmp("cimc-lite", board))
		turn_on_koe_display();
	if(!strcmp("cimc-i", board)) /* FixMe: LVDS display here */
		turn_on_koe_display();
#endif /* defined CONFIG_RAVION_DISPLAY_KOE */
	if(!strcmp("mtu", board)) mtu_display_init();
	if(!strcmp("pkk-m7", board)) pkk_m7_display_init();
	if(!strcmp("pkk-m10", board)) pkk_m10_display_init();

	common_backlight_init();

	debug("Enable RGB!\n");
}

static void enable_lvds(struct display_info_t const *dev)
{
	struct mxc_ccm_reg *mxc_ccm = (struct mxc_ccm_reg *)CCM_BASE_ADDR;
	struct iomuxc *iomux = (struct iomuxc *)IOMUXC_BASE_ADDR;
	uint32_t pclk_khz = PICOS2KHZ(dev->mode.pixclock);
	unsigned char MMDC_DIV = 8;

	/*
	 * Select LDB clock from PLL5, PLL2_PFD0, PLL2_PFD2, MMDC_CH1 or
	 * PLL3_SW source (can be divided to 3.5 or 7 with ccm)
	 *
	 * Default clock rates is:
	 *	PLL5:      630MHz => LDB_CLK 630/7 = 90MHz (disabled)
	 *	PLL2_PFD0: 352MHz => LDB_CLK 352/7 = 50.286MHz
	 *	PLL2_PFD2: 396MHz => LDB_CLK 396/7 = 56.571MHz
	 *	MMDC_CH1:  528MHz => LDB_CLK 528/7 = 75.428MHz
	 *	PLL3_SW:   480MHz => LDB_CLK 480/7 = 68.571MHz
	 *
	 * But only MMDC_CH1 clock may be changed on-the-fly, by set
	 * divider form 1 to 8, and clock one from:
	 * 		75.428, 37.714, 25.142, 18.857,
	 * 		15.086, 12.571, 10.775, 9.442 MHz.
	 * Any of them may be used as IPU pixelclock.
	 */

	if (pclk_khz >= 75428) MMDC_DIV = 1; else
	if (pclk_khz >= 37714) MMDC_DIV = 2; else
	if (pclk_khz >= 25142) MMDC_DIV = 3; else
	if (pclk_khz >= 18875) MMDC_DIV = 4; else
	if (pclk_khz >= 15086) MMDC_DIV = 5; else
	if (pclk_khz >= 12571) MMDC_DIV = 6; else
	MMDC_DIV = 7; /* (pclk_khz >=  9442) */

	debug("Request PCLK=%dKHz, div=%d, result PCLK=%dKHz\n",
		pclk_khz, MMDC_DIV, 528000/7/MMDC_DIV);

	ipu_set_ldb_clock(528000000L / MMDC_DIV);

	/* disble MMDC_CH1 clock */
	clrbits_le32(&mxc_ccm->ccdr, MXC_CCM_CCDR_MMDC_CH1_AXI_ROOT_CG);
	/* configure MMDC_CH1 clock divider */
	clrsetbits_le32(&mxc_ccm->cbcdr,
	    MXC_CCM_CBCDR_MMDC_CH1_PODF_MASK,
	    (MMDC_DIV - 1) << MXC_CCM_CBCDR_MMDC_CH1_PODF_OFFSET);
	/* allow handshake with mmdc_ch1 module */
	clrbits_le32(&mxc_ccm->ccdr, MXC_CCM_CCDR_MMDC_CH1_MASK);
	/* reenable MMDC_CH1 clock */
	setbits_le32(&mxc_ccm->ccdr, MXC_CCM_CCDR_MMDC_CH1_AXI_ROOT_CG);

	/* then select MMDC_CH1 as LDB clock source */
	select_ldb_di_clock_source(MXC_MMDC_CH1_CLK);

	/* Configure IOMUXC GPR2 and GRP3 registers */
	clrsetbits_le32(&iomux->gpr[2],
		IOMUXC_GPR2_SPLIT_MODE_EN_MASK,
		 IOMUXC_GPR2_BGREF_RRMODE_EXTERNAL_RES
		|IOMUXC_GPR2_DI0_VS_POLARITY_ACTIVE_LOW
		|IOMUXC_GPR2_DI1_VS_POLARITY_ACTIVE_LOW
		/* LVDS0 config */
		|IOMUXC_GPR2_BIT_MAPPING_CH0_JEIDA
		|IOMUXC_GPR2_DATA_WIDTH_CH0_24BIT
		|IOMUXC_GPR2_LVDS_CH0_MODE_ENABLED_DI1
		/* LVDS1 config */
		|IOMUXC_GPR2_BIT_MAPPING_CH1_SPWG
		|IOMUXC_GPR2_DATA_WIDTH_CH1_24BIT
		|IOMUXC_GPR2_LVDS_CH1_MODE_DISABLED);

	clrsetbits_le32(&iomux->gpr[3],
	    IOMUXC_GPR3_LVDS0_MUX_CTL_MASK | IOMUXC_GPR3_HDMI_MUX_CTL_MASK,
	    IOMUXC_GPR3_MUX_SRC_IPU1_DI0 << IOMUXC_GPR3_LVDS0_MUX_CTL_OFFSET);

	common_backlight_init();

#if defined CONFIG_RAVION_DISPLAY_KOE
	char *board = env_get("board");
	if(!strcmp("cimc-lite", board)) /* FixMe: TFT display here */
		turn_on_koe_display();
	if(!strcmp("cimc-i", board))
		turn_on_koe_display();
#endif /* defined CONFIG_RAVION_DISPLAY_KOE */
	if(!strcmp("mtu", board)) /* FixMe: TFT display here */
		mtu_display_init();
	if(!strcmp("pkk-m7-i", board)) pkk_m7_display_init();
	if(!strcmp("pkk-m10-i", board)) pkk_m10_display_init();

	debug("enable LVDS!\n");
}

static int detect_default(struct display_info_t const *dev)
{
	char *display = env_get("display");
	char *board = env_get("board");

	/* FixMe: here ??? */
	if(!strcmp("router", board)) {
	    turn_on_blue_led();
	}

	if(display)
		return (!strcmp(dev->mode.name,display));

	return 0;
}

/*
 * Supported PCLK for LVDS displays only from list:
 * 75428, 37714, 25142, 18857, 15086, 12571, 10775, 9442KHz.
 * This is fundamental limit for I.MX LDB IP Block clocking
 * see comment in enable_lvds function bellow.
 * Allow IPU (and parallel display output) clocking from
 * any sources, but make display sync signal (PCLK) equivalent
 * to LDB_CLK dividev by 7
 * FixMe: This time I don't know how remove this limit
 */
struct display_info_t const displays[] =
{ {	/* LVDS displays connected to di0 */
	.bus	= -1,
	.addr	= 0,
	.di	= 0,
	.pixfmt	= IPU_PIX_FMT_RGB24,
	.detect	= detect_default,
	.enable	= enable_lvds,
	.mode	= {
		.name           = "lvds-TX09D200VM0BAA",
		.refresh        = 60,
		.xres           = 240,
		.yres           = 320,
		.pixclock       = KHZ2PICOS(9442), /* 6500 */
		.hsync_len      = 16,
		.left_margin    = 48,
		.right_margin   = 16,
		.vsync_len      = 6,
		.upper_margin   = 6,
		.lower_margin   = 6,
		.sync           = 0,
		.vmode          = FB_VMODE_NONINTERLACED
} }, {
	.bus	= -1,
	.addr	= 0,
	.di	= 0,
	.pixfmt	= IPU_PIX_FMT_RGB24,
	.detect	= detect_default,
	.enable	= enable_lvds,
	.mode	= {
		.name           = "lvds-AC043NA110200",
		.refresh        = 60,
		.xres           = 480,
		.yres           = 272,
		.pixclock       = KHZ2PICOS(9400), /* 9442 */
		.hsync_len      = 16,
		.left_margin    = 48,
		.right_margin   = 16,
		.vsync_len      = 2,
		.upper_margin   = 16,
		.lower_margin   = 12,
		.sync           = 0,
		.vmode          = FB_VMODE_NONINTERLACED
} }, {
	.bus	= -1,
	.addr	= 0,
	.di	= 0,
	.pixfmt	= IPU_PIX_FMT_RGB24,
	.detect	= detect_default,
	.enable	= enable_lvds,
	.mode	= {
		.name           = "lvds-AA070ME11ADA11",
		.refresh        = 60,
		.xres           = 800,
		.yres           = 480,
		.pixclock       = KHZ2PICOS(25142), /* 30770 */
		.left_margin    = 30,
		.right_margin   = 87,
		.upper_margin   = 13,
		.lower_margin   = 30,
		.hsync_len      = 1,
		.vsync_len      = 1,
		.sync           = 0,
		.vmode          = FB_VMODE_NONINTERLACED
} }, {
	.bus	= -1,
	.addr	= 0,
	.di	= 0,
	.pixfmt	= IPU_PIX_FMT_RGB24,
	.detect	= detect_default,
	.enable	= enable_lvds,
	.mode	= {
		.name           = "lvds-AA104XF12-DE2",
		.refresh        = 60,
		.xres           = 1024,
		.yres           = 768,
		.pixclock       = KHZ2PICOS(37714), /* 65000 */
		.left_margin    = 150,
		.right_margin   = 150,
		.upper_margin   = 15,
		.lower_margin   = 15,
		.hsync_len      = 20,
		.vsync_len      = 8,
		.sync           = 0,
		.vmode          = FB_VMODE_NONINTERLACED
} }, {	/* TFT displays & VGA modulator connected to di1 */
	.bus	= -1,
	.addr	= 0,
	.di	= 1,
	.pixfmt	= IPU_PIX_FMT_RGB666,
	.detect	= detect_default,
	.enable	= enable_rgb,
	.mode	= {
		.name           = "vga",
		.refresh        = 60,
		.xres           = 640,
		.yres           = 480,
		.pixclock       = KHZ2PICOS(33000),
		.left_margin    = 48,
		.right_margin   = 16,
		.upper_margin   = 31,
		.lower_margin   = 11,
		.hsync_len      = 96,
		.vsync_len      = 2,
		.sync           = 0,
		.vmode          = FB_VMODE_NONINTERLACED
} }, {
	.bus	= -1,
	.addr	= 0,
	.di	= 1,
	.pixfmt	= IPU_PIX_FMT_RGB666,
	.enable	= enable_rgb,
	.mode	= {
		.name           = "wvga",
		.refresh        = 60,
		.xres           = 800,
		.yres           = 480,
		.pixclock       = KHZ2PICOS(25000),
		.left_margin    = 40,
		.right_margin   = 88,
		.upper_margin   = 33,
		.lower_margin   = 10,
		.hsync_len      = 128,
		.vsync_len      = 2,
		.sync           = 0,
		.vmode          = FB_VMODE_NONINTERLACED
} }, {
	.bus	= -1,
	.addr	= 0,
	.di	= 1,
	.pixfmt	= IPU_PIX_FMT_RGB24,
	.detect	= detect_default,
	.enable	= enable_rgb,
	.mode	= {
		.name           = "AA050MH01-DA1",
		.refresh        = 60,
		.xres           = 800,
		.yres           = 480,
		.pixclock       = KHZ2PICOS(30770),
		.left_margin    = 30,
		.right_margin   = 87,
		.upper_margin   = 13,
		.lower_margin   = 30,
		.hsync_len      = 1,
		.vsync_len      = 1,
		.sync           = 0,
		.vmode          = FB_VMODE_NONINTERLACED
} }, {
	.bus	= -1,
	.addr	= 0,
	.di	= 1,
	.pixfmt	= IPU_PIX_FMT_RGB24,
	.detect	= detect_default,
	.enable	= enable_lvds,
	.mode	= {
		.name           = "AC043NA110200",
		.refresh        = 60,
		.xres           = 480,
		.yres           = 272,
		.pixclock       = KHZ2PICOS(9400),
		.hsync_len      = 16,
		.left_margin    = 48,
		.right_margin   = 16,
		.vsync_len      = 2,
		.upper_margin   = 16,
		.lower_margin   = 12,
		.sync           = 0,
		.vmode          = FB_VMODE_NONINTERLACED
} }, {
	.bus	= -1,
	.addr	= 0,
	.di	= 1,
	.pixfmt	= IPU_PIX_FMT_RGB666,
	.detect	= detect_default,
	.enable	= enable_rgb,
	.mode	= {
		.name           = "AA070ME11ADA11",
		.refresh        = 60,
		.xres           = 800,
		.yres           = 480,
		.pixclock       = KHZ2PICOS(30770),
		.left_margin    = 30,
		.right_margin   = 87,
		.upper_margin   = 13,
		.lower_margin   = 30,
		.hsync_len      = 1,
		.vsync_len      = 1,
		.sync           = 0,
		.vmode          = FB_VMODE_NONINTERLACED
} }, {
	.bus	= -1,
	.addr	= 0,
	.di	= 1,
	.pixfmt	= IPU_PIX_FMT_RGB666,
	.detect	= detect_default,
	.enable	= enable_rgb,
	.mode	= {
		.name           = "AA104XF12-DE2",
		.refresh        = 60,
		.xres           = 1024,
		.yres           = 768,
		.pixclock       = KHZ2PICOS(65000),
		.left_margin    = 150,
		.right_margin   = 150,
		.upper_margin   = 15,
		.lower_margin   = 15,
		.hsync_len      = 20,
		.vsync_len      = 8,
		.sync           = 0,
		.vmode          = FB_VMODE_NONINTERLACED
} }, {
	.bus	= -1,
	.addr	= 0,
	.di	= 1,
	.pixfmt	= IPU_PIX_FMT_RGB24,
	.detect	= detect_default,
	.enable	= enable_rgb,
	.mode	= {
		.name           = "TX09D200VM0BAA",
		.refresh        = 60,
		.xres           = 240,
		.yres           = 320,
		.pixclock       = KHZ2PICOS(6500),
		.hsync_len      = 16,
		.left_margin    = 48,
		.right_margin   = 16,
		.vsync_len      = 6,
		.upper_margin   = 6,
		.lower_margin   = 6,
		.sync           = 0,
		.vmode          = FB_VMODE_NONINTERLACED
} }, { /* HDMI display connected to DI0 */
	.bus	= -1,
	.addr	= 0,
	.di	= 0,
	.pixfmt	= IPU_PIX_FMT_RGB24,
	.detect	= detect_hdmi,
	.enable	= do_enable_hdmi,
	.mode	= {
		.name           = "HDMI",
		.refresh        = 60,
		.xres           = 1024,
		.yres           = 768,
		.pixclock       = 15385,
		.left_margin    = 220,
		.right_margin   = 40,
		.upper_margin   = 21,
		.lower_margin   = 7,
		.hsync_len      = 60,
		.vsync_len      = 10,
		.sync           = FB_SYNC_EXT,
		.vmode          = FB_VMODE_NONINTERLACED
} } };
size_t display_count = ARRAY_SIZE(displays);

static void setup_display(void)
{
	enable_ipu_clock();
}
#endif /* defined(CONFIG_VIDEO_IPUV3) */

static iomux_v3_cfg_t const pwr_intb_pads[] = {
#define EXT_NRESETOUTPUT_GP IMX_GPIO_NR(5, 7)
	MX6_PAD_DISP0_DAT13__GPIO5_IO07 | MUX_PAD_CTRL(NO_PAD_CTRL),
};

int board_early_init_f(void)
{
	imx_iomux_v3_setup_multiple_pads(pwr_intb_pads,
					 ARRAY_SIZE(pwr_intb_pads));
	/* Set nRESETOUT to 100mS, then set them into ACTIVE state */
	gpio_direction_output(EXT_NRESETOUTPUT_GP, 1);

	return 0;
}

/*
 * Do not overwrite the console
 * Use always serial for U-Boot console
 */
int overwrite_console(void)
{
	return 1;
}

int board_init(void)
{
	/* address of boot parameters */
	gd->bd->bi_boot_params = PHYS_SDRAM + 0x100;
#if defined(CONFIG_VIDEO_IPUV3)
	setup_display();
#endif
	return 0;
}

#ifdef CONFIG_BOARD_LATE_INIT
int board_late_init(void)
{
#if defined(CONFIG_REVISION_TAG) && \
    defined(CONFIG_ENV_VARS_UBOOT_RUNTIME_CONFIG)
	char env_str[256];
	u32 rev;

	rev = get_board_rev();
	snprintf(env_str, ARRAY_SIZE(env_str), "%.4x", rev);
	env_set("board_rev", env_str);
#endif
	return 0;
}
#endif /* CONFIG_BOARD_LATE_INIT */

#if defined(CONFIG_OF_LIBFDT) && defined(CONFIG_OF_SYSTEM_SETUP)
int ft_system_setup(void *blob, bd_t *bd)
{
	return 0;
}
#endif

int checkboard(void)
{
	char it[] = " IT";
	int minc, maxc;

	switch (get_cpu_temp_grade(&minc, &maxc)) {
	case TEMP_AUTOMOTIVE:
	case TEMP_INDUSTRIAL:
		break;
	case TEMP_EXTCOMMERCIAL:
	default:
		it[0] = 0;
	};
	printf("CPU Module: Ravion iMX6 %s %sMB%s\n",
	       is_cpu_type(MXC_CPU_MX6QP) ? "QuadPlus" : \
			is_cpu_type(MXC_CPU_MX6Q) ? "Quad" : \
	    		is_cpu_type(MXC_CPU_MX6DL) ? "DualLite" : "Solo",
	       (gd->ram_size == 0x80000000) ? "2048" : \
			(gd->ram_size == 0x40000000) ? "1024" : \
			(gd->ram_size == 0x20000000) ? "512" : "256", it);
	return 0;
}

#if defined(CONFIG_OF_LIBFDT) && defined(CONFIG_OF_BOARD_SETUP)
int ft_board_setup(void *blob, bd_t *bd)
{
	return ft_common_board_setup(blob, bd);
}
#endif

#ifdef CONFIG_CMD_BMODE
static const struct boot_mode board_boot_modes[] = {
	{"mmc",	MAKE_CFGVAL(0x40, 0x20, 0x00, 0x00)},
	{NULL,	0},
};
#endif

int misc_init_r(void)
{
#ifdef CONFIG_CMD_BMODE
	add_board_boot_modes(board_boot_modes);
#endif
	return 0;
}

#ifdef CONFIG_LDO_BYPASS_CHECK
/* TODO, use external pmic, for now always ldo_enable */
void ldo_mode_set(int ldo_bypass)
{
	return;
}
#endif
