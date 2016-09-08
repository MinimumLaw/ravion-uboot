/*
 * Copyright (C) 2015 Freescale Semiconductor, Inc.
 *               2016 Toradex AG
 * 
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <asm/arch/clock.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/mx7-pins.h>
#include <asm/arch/sys_proto.h>
#include <asm/gpio.h>
#include <asm/imx-common/iomux-v3.h>
#include <asm/imx-common/boot_mode.h>
#include <asm/io.h>
#include <linux/sizes.h>
#include <common.h>
#include <fsl_esdhc.h>
#include <mmc.h>
#include <micrel.h>
#include <miiphy.h>
#include <netdev.h>
#include <power/pmic.h>
#include <power/rn5t567_pmic.h>
#ifdef CONFIG_SYS_I2C_MXC
#include <i2c.h>
#include <asm/imx-common/mxc_i2c.h>
#endif
#include <asm/arch/crm_regs.h>
#ifdef CONFIG_VIDEO_MXS
#include <linux/fb.h>
#include <mxsfb.h>
#endif
#ifdef CONFIG_FASTBOOT
#include <fastboot.h>
#ifdef CONFIG_ANDROID_RECOVERY
#include <recovery.h>
#endif
#endif /*CONFIG_FASTBOOT*/
#include "../common/configblock.h"

#define COLIBRI_EXT_PHYCLK

DECLARE_GLOBAL_DATA_PTR;

#define BUTTON_PAD_CTRL    (PAD_CTL_PUS_PU5KOHM | PAD_CTL_DSE_3P3V_98OHM)

#define ENET_PAD_CTRL  (PAD_CTL_PUS_PU100KOHM | PAD_CTL_DSE_3P3V_32OHM)
#define ENET_PAD_CTRL_MII  (PAD_CTL_DSE_3P3V_32OHM)
#define ENET_RX_PAD_CTRL  (PAD_CTL_PUS_PU100KOHM | PAD_CTL_DSE_3P3V_49OHM)

#define I2C_PAD_CTRL    (PAD_CTL_DSE_3P3V_32OHM | PAD_CTL_SRE_SLOW | \
	PAD_CTL_HYS | PAD_CTL_PUE | PAD_CTL_PUS_PU100KOHM)

#define LCD_PAD_CTRL    (PAD_CTL_HYS | PAD_CTL_PUS_PU100KOHM | \
	PAD_CTL_DSE_3P3V_49OHM)

#define NAND_PAD_CTRL (PAD_CTL_DSE_3P3V_49OHM | PAD_CTL_SRE_SLOW | PAD_CTL_HYS)

#define NAND_PAD_READY0_CTRL (PAD_CTL_DSE_3P3V_49OHM | PAD_CTL_PUS_PU5KOHM)

#define QSPI_PAD_CTRL	\
	(PAD_CTL_DSE_3P3V_49OHM | PAD_CTL_PUE | PAD_CTL_PUS_PU47KOHM)

#define SPI_PAD_CTRL (PAD_CTL_DSE_3P3V_49OHM | PAD_CTL_SRE_SLOW | PAD_CTL_HYS)

#define UART_PAD_CTRL  (PAD_CTL_DSE_3P3V_49OHM | \
	PAD_CTL_PUS_PU100KOHM | PAD_CTL_HYS)

#define USDHC_PAD_CTRL (PAD_CTL_DSE_3P3V_32OHM | PAD_CTL_SRE_SLOW | \
	PAD_CTL_HYS | PAD_CTL_PUE | PAD_CTL_PUS_PU47KOHM)

#define NO_PULLUP (PAD_CTL_HYS | PAD_CTL_SRE_SLOW | PAD_CTL_DSE_3P3V_49OHM)

#define WEAK_PULLUP (PAD_CTL_PUS_PU47KOHM | PAD_CTL_PUE \
	| PAD_CTL_HYS |	PAD_CTL_SRE_SLOW | PAD_CTL_DSE_3P3V_49OHM)

#define WEAK_PULLDOWN (PAD_CTL_PUS_PU47KOHM | PAD_CTL_PUE \
	| PAD_CTL_HYS |	PAD_CTL_SRE_SLOW | PAD_CTL_DSE_3P3V_49OHM)


#ifdef CONFIG_SYS_I2C_MXC
#define PC MUX_PAD_CTRL(I2C_PAD_CTRL)
/* I2C1 for PMIC */
struct i2c_pads_info i2c_pad_info1 = {
	.scl = {
		.i2c_mode = MX7D_PAD_GPIO1_IO04__I2C1_SCL | PC,
		.gpio_mode = MX7D_PAD_GPIO1_IO04__GPIO1_IO4 | PC,
		.gp = IMX_GPIO_NR(1, 4),
	},
	.sda = {
		.i2c_mode = MX7D_PAD_GPIO1_IO05__I2C1_SDA | PC,
		.gpio_mode = MX7D_PAD_GPIO1_IO05__GPIO1_IO5 | PC,
		.gp = IMX_GPIO_NR(1, 5),
	},
};
/* I2C4 for Colibri I2C */
struct i2c_pads_info i2c_pad_info4 = {
	.scl = {
		.i2c_mode = MX7D_PAD_ENET1_RGMII_TD2__I2C4_SCL | PC,
		.gpio_mode = MX7D_PAD_ENET1_RGMII_TD2__GPIO7_IO8 | PC,
		.gp = IMX_GPIO_NR(7, 8),
	},
	.sda = {
		.i2c_mode = MX7D_PAD_ENET1_RGMII_TD3__I2C4_SDA | PC,
		.gpio_mode = MX7D_PAD_ENET1_RGMII_TD3__GPIO7_IO9 | PC,
		.gp = IMX_GPIO_NR(7, 9),
	},
};
#endif

int dram_init(void)
{
	gd->ram_size = get_ram_size((void *)PHYS_SDRAM, PHYS_SDRAM_SIZE);

	return 0;
}

static iomux_v3_cfg_t const uart1_pads[] = {
	MX7D_PAD_UART1_RX_DATA__UART1_DTE_TX | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX7D_PAD_UART1_TX_DATA__UART1_DTE_RX | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX7D_PAD_SAI2_TX_BCLK__UART1_DTE_CTS | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX7D_PAD_SAI2_TX_SYNC__UART1_DTE_RTS | MUX_PAD_CTRL(UART_PAD_CTRL),
};

static iomux_v3_cfg_t const usdhc1_pads[] = {
	MX7D_PAD_SD1_CLK__SD1_CLK | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX7D_PAD_SD1_CMD__SD1_CMD | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX7D_PAD_SD1_DATA0__SD1_DATA0 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX7D_PAD_SD1_DATA1__SD1_DATA1 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX7D_PAD_SD1_DATA2__SD1_DATA2 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX7D_PAD_SD1_DATA3__SD1_DATA3 | MUX_PAD_CTRL(USDHC_PAD_CTRL),

	MX7D_PAD_GPIO1_IO00__GPIO1_IO0 | MUX_PAD_CTRL(NO_PAD_CTRL),
#define USDHC1_CD_GPIO	IMX_GPIO_NR(1, 0)
};

#ifdef CONFIG_SYS_USE_NAND
static iomux_v3_cfg_t const gpmi_pads[] = {
	MX7D_PAD_SD3_DATA0__NAND_DATA00 | MUX_PAD_CTRL(NAND_PAD_CTRL),
	MX7D_PAD_SD3_DATA1__NAND_DATA01 | MUX_PAD_CTRL(NAND_PAD_CTRL),
	MX7D_PAD_SD3_DATA2__NAND_DATA02 | MUX_PAD_CTRL(NAND_PAD_CTRL),
	MX7D_PAD_SD3_DATA3__NAND_DATA03 | MUX_PAD_CTRL(NAND_PAD_CTRL),
	MX7D_PAD_SD3_DATA4__NAND_DATA04 | MUX_PAD_CTRL(NAND_PAD_CTRL),
	MX7D_PAD_SD3_DATA5__NAND_DATA05 | MUX_PAD_CTRL(NAND_PAD_CTRL),
	MX7D_PAD_SD3_DATA6__NAND_DATA06 | MUX_PAD_CTRL(NAND_PAD_CTRL),
	MX7D_PAD_SD3_DATA7__NAND_DATA07 | MUX_PAD_CTRL(NAND_PAD_CTRL),
	MX7D_PAD_SD3_CLK__NAND_CLE	| MUX_PAD_CTRL(NAND_PAD_CTRL),
	MX7D_PAD_SD3_CMD__NAND_ALE	| MUX_PAD_CTRL(NAND_PAD_CTRL),
	MX7D_PAD_SD3_STROBE__NAND_RE_B	| MUX_PAD_CTRL(NAND_PAD_CTRL),
	MX7D_PAD_SD3_RESET_B__NAND_WE_B	| MUX_PAD_CTRL(NAND_PAD_CTRL),
	MX7D_PAD_SAI1_RX_DATA__NAND_CE1_B	| MUX_PAD_CTRL(NAND_PAD_CTRL),
	MX7D_PAD_SAI1_TX_BCLK__NAND_CE0_B	| MUX_PAD_CTRL(NAND_PAD_CTRL),
	MX7D_PAD_SAI1_TX_DATA__NAND_READY_B	| MUX_PAD_CTRL(NAND_PAD_READY0_CTRL),
};

static void setup_gpmi_nand(void)
{
	imx_iomux_v3_setup_multiple_pads(gpmi_pads, ARRAY_SIZE(gpmi_pads));

	/*
	 * NAND_USDHC_BUS_CLK is set in rom
	 */
	set_clk_nand();

	/*
	 * APBH clock root is set in init_esdhc, USDHC3_CLK.
	 * There is no clk gate for APBHDMA.
	 * No touch here.
	 */
}
#endif

#ifdef CONFIG_VIDEO_MXS
static iomux_v3_cfg_t const lcd_pads[] = {
	MX7D_PAD_LCD_CLK__LCD_CLK | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_ENABLE__LCD_ENABLE | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_HSYNC__LCD_HSYNC | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_VSYNC__LCD_VSYNC | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA00__LCD_DATA0 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA01__LCD_DATA1 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA02__LCD_DATA2 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA03__LCD_DATA3 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA04__LCD_DATA4 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA05__LCD_DATA5 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA06__LCD_DATA6 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA07__LCD_DATA7 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA08__LCD_DATA8 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA09__LCD_DATA9 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA10__LCD_DATA10 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA11__LCD_DATA11 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA12__LCD_DATA12 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA13__LCD_DATA13 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA14__LCD_DATA14 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA15__LCD_DATA15 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA16__LCD_DATA16 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA17__LCD_DATA17 | MUX_PAD_CTRL(LCD_PAD_CTRL),
#if 0
	MX7D_PAD_LCD_DATA18__LCD_DATA18 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA19__LCD_DATA19 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA20__LCD_DATA20 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA21__LCD_DATA21 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA22__LCD_DATA22 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA23__LCD_DATA23 | MUX_PAD_CTRL(LCD_PAD_CTRL),
#endif
};

static iomux_v3_cfg_t const backlight_pads[] = {
	/* Backlight On */
	MX7D_PAD_SD1_WP__GPIO5_IO1 | MUX_PAD_CTRL(NO_PAD_CTRL),
#define RGB_BACKLIGHT_GP IMX_GPIO_NR(5, 1)
/* TODO PWM not GPIO */
	MX7D_PAD_GPIO1_IO08__GPIO1_IO8   | MUX_PAD_CTRL(NO_PULLUP),
	MX7D_PAD_ECSPI2_MOSI__GPIO4_IO21 | MUX_PAD_CTRL(NO_PAD_CTRL),
#define RGB_BACKLIGHTPWM_GP IMX_GPIO_NR(1, 8)
};

struct lcd_panel_info_t {
	unsigned int lcdif_base_addr;
	int depth;
	void	(*enable)(struct lcd_panel_info_t const *dev);
	struct fb_videomode mode;
};

void do_enable_parallel_lcd(struct lcd_panel_info_t const *dev)
{
	imx_iomux_v3_setup_multiple_pads(lcd_pads, ARRAY_SIZE(lcd_pads));

	imx_iomux_v3_setup_multiple_pads(backlight_pads, ARRAY_SIZE(backlight_pads));

	/* Set Brightness to high */
	gpio_direction_output(RGB_BACKLIGHT_GP , 1);
	gpio_direction_output(RGB_BACKLIGHTPWM_GP , 0);
}

static struct lcd_panel_info_t const displays[] = {{
	.lcdif_base_addr = ELCDIF1_IPS_BASE_ADDR,
	.depth = 18,
	.enable	= do_enable_parallel_lcd,
	.mode	= {
		.name           = "vga-rgb",
		.refresh        = 60,
		.xres           = 640,
		.yres           = 480,
		.pixclock       = 39722,
		.left_margin    = 48,
		.right_margin   = 16,
		.upper_margin   = 33,
		.lower_margin   = 10,
		.hsync_len      = 96,
		.vsync_len      = 2,
		.sync           = 0,
		.vmode          = FB_VMODE_NONINTERLACED
} }, {
	.lcdif_base_addr = ELCDIF1_IPS_BASE_ADDR,
	.depth = 18,
	.enable	= do_enable_parallel_lcd,
	.mode	= {
		.name           = "wvga-rgb",
		.refresh        = 60,
		.xres           = 800,
		.yres           = 480,
		.pixclock       = 25000,
		.left_margin    = 40,
		.right_margin   = 88,
		.upper_margin   = 33,
		.lower_margin   = 10,
		.hsync_len      = 128,
		.vsync_len      = 2,
		.sync           = 0,
		.vmode          = FB_VMODE_NONINTERLACED
} } };

int board_video_skip(void)
{
	int i;
	int ret;
	char const *panel = getenv("panel");
	if (!panel) {
		panel = displays[0].mode.name;
		printf("No panel detected: default to %s\n", panel);
		i = 0;
	} else {
		for (i = 0; i < ARRAY_SIZE(displays); i++) {
			if (!strcmp(panel, displays[i].mode.name))
				break;
		}
	}
	if (i < ARRAY_SIZE(displays)) {
		ret = mxs_lcd_panel_setup(displays[i].mode, displays[i].depth,
					  displays[i].lcdif_base_addr);
		if (!ret) {
			if (displays[i].enable)
				displays[i].enable(displays+i);
			printf("Display: %s (%ux%u)\n",
			       displays[i].mode.name,
			       displays[i].mode.xres,
			       displays[i].mode.yres);
		} else
			printf("LCD %s cannot be configured: %d\n",
			       displays[i].mode.name, ret);
	} else {
		printf("unsupported panel %s\n", panel);
		return -EINVAL;
	}

	return 0;
}
#endif

#ifdef CONFIG_FEC_MXC
static iomux_v3_cfg_t const fec1_pads[] = {
	MX7D_PAD_ENET1_CRS__GPIO7_IO14 | MUX_PAD_CTRL(WEAK_PULLDOWN), /* don't interfere with RX_CTL */
	MX7D_PAD_ENET1_RGMII_RX_CTL__ENET1_RGMII_RX_CTL | MUX_PAD_CTRL(ENET_RX_PAD_CTRL),
	MX7D_PAD_ENET1_RGMII_RD0__ENET1_RGMII_RD0 | MUX_PAD_CTRL(ENET_RX_PAD_CTRL),
	MX7D_PAD_ENET1_RGMII_RD1__ENET1_RGMII_RD1 | MUX_PAD_CTRL(ENET_RX_PAD_CTRL),
	MX7D_PAD_ENET1_RGMII_RXC__ENET1_RX_ER | MUX_PAD_CTRL(ENET_RX_PAD_CTRL),
//	MX7D_PAD_ENET1_RGMII_RXC__ENET1_RGMII_RXC | MUX_PAD_CTRL(ENET_RX_PAD_CTRL),
	MX7D_PAD_ENET1_RGMII_TX_CTL__ENET1_RGMII_TX_CTL | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX7D_PAD_ENET1_RGMII_TD0__ENET1_RGMII_TD0 | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX7D_PAD_ENET1_RGMII_TD1__ENET1_RGMII_TD1 | MUX_PAD_CTRL(ENET_PAD_CTRL),
#ifndef COLIBRI_EXT_PHYCLK
	MX7D_PAD_GPIO1_IO12__CCM_ENET_REF_CLK1 | MUX_PAD_CTRL(ENET_PAD_CTRL) | MUX_MODE_SION,
#else
	MX7D_PAD_GPIO1_IO12__CCM_ENET_REF_CLK1 | MUX_PAD_CTRL(ENET_PAD_CTRL),
#endif
	MX7D_PAD_SD2_CD_B__ENET1_MDIO | MUX_PAD_CTRL(ENET_PAD_CTRL_MII),
	MX7D_PAD_SD2_WP__ENET1_MDC | MUX_PAD_CTRL(ENET_PAD_CTRL_MII),
};

static void setup_iomux_fec(void)
{
	imx_iomux_v3_setup_multiple_pads(fec1_pads, ARRAY_SIZE(fec1_pads));
}
#endif

/* UARTs are used in DTE mode, switch the mode on all UARTs before
 * any pinmuxing connects a (DCE) output to a transceiver output.
 */
#define UFCR		0x90	/* FIFO Control Register */
#define UFCR_DCEDTE	(1<<6)	/* DCE=0 */

static void setup_dtemode_uart(void)
{
	setbits_le32((u32 *)(UART1_IPS_BASE_ADDR + UFCR), UFCR_DCEDTE);
	setbits_le32((u32 *)(UART2_IPS_BASE_ADDR + UFCR), UFCR_DCEDTE);
	setbits_le32((u32 *)(UART3_IPS_BASE_ADDR + UFCR), UFCR_DCEDTE);
}

static void setup_iomux_uart(void)
{
	imx_iomux_v3_setup_multiple_pads(uart1_pads, ARRAY_SIZE(uart1_pads));
}

#ifdef CONFIG_FSL_ESDHC

static struct fsl_esdhc_cfg usdhc_cfg[2] = {
	{USDHC1_BASE_ADDR, 0, 4},
};

int mmc_get_env_devno(void)
{
	struct bootrom_sw_info **p =
		(struct bootrom_sw_info **)ROM_SW_INFO_ADDR;

	u8 boot_type = (*p)->boot_dev_type;
	u8 dev_no = (*p)->boot_dev_instance;

	/* If not boot from sd/mmc, use default value */
	if ((boot_type != BOOT_TYPE_SD) && (boot_type != BOOT_TYPE_MMC))
		return CONFIG_SYS_MMC_ENV_DEV;

	if (2 == dev_no)
		dev_no--;

	return dev_no;
}

int mmc_map_to_kernel_blk(int dev_no)
{
	if (1 == dev_no)
		dev_no++;

	return dev_no;
}

int board_mmc_getcd(struct mmc *mmc)
{
	struct fsl_esdhc_cfg *cfg = (struct fsl_esdhc_cfg *)mmc->priv;
	int ret = 0;

	switch (cfg->esdhc_base) {
	case USDHC1_BASE_ADDR:
		ret = !gpio_get_value(USDHC1_CD_GPIO);
		break;
	}

	return ret;
}

int board_mmc_init(bd_t *bis)
{
	int i, ret;
	/*
	 * (U-boot device node)    (Physical Port)
	 * mmc0                    USDHC1
	 */
	for (i = 0; i < CONFIG_SYS_FSL_USDHC_NUM; i++) {
		switch (i) {
		case 0:
			imx_iomux_v3_setup_multiple_pads(
				usdhc1_pads, ARRAY_SIZE(usdhc1_pads));
			gpio_request(USDHC1_CD_GPIO, "usdhc1_cd");
			gpio_direction_input(USDHC1_CD_GPIO);
			usdhc_cfg[0].sdhc_clk = mxc_get_clock(MXC_ESDHC_CLK);
			break;
		default:
			printf("Warning: you configured more USDHC controllers"
				"(%d) than supported by the board\n", i + 1);
			return 0;
		}

		ret = fsl_esdhc_initialize(bis, &usdhc_cfg[i]);
		if (ret)
			return ret;
	}

	return 0;
}

int check_mmc_autodetect(void)
{
	char *autodetect_str = getenv("mmcautodetect");

	if ((autodetect_str != NULL) &&
		(strcmp(autodetect_str, "yes") == 0)) {
		return 1;
	}

	return 0;
}

void board_late_mmc_init(void)
{
	char cmd[32];
	char mmcblk[32];
	u32 dev_no = mmc_get_env_devno();

	if (!check_mmc_autodetect())
		return;

	setenv_ulong("mmcdev", dev_no);

	/* Set mmcblk env */
	sprintf(mmcblk, "/dev/mmcblk%dp2 rootwait rw",
		mmc_map_to_kernel_blk(dev_no));
	setenv("mmcroot", mmcblk);

	sprintf(cmd, "mmc dev %d", dev_no);
	run_command(cmd, 0);
}

#endif

#ifdef CONFIG_FEC_MXC
int board_eth_init(bd_t *bis)
{
	int ret;

	setup_iomux_fec();

	ret = fecmxc_initialize_multi(bis, 0,
		CONFIG_FEC_MXC_PHYADDR, IMX_FEC_BASE);
	if (ret)
		printf("FEC1 MXC: %s:failed\n", __func__);

	return ret;
}

static int setup_fec(void)
{
	struct iomuxc_gpr_base_regs *const iomuxc_gpr_regs
		= (struct iomuxc_gpr_base_regs *) IOMUXC_GPR_BASE_ADDR;
	int ret;

#ifndef COLIBRI_EXT_PHYCLK
	/* Use 50M anatop REF_CLK1 for ENET1, clear gpr1[13], set gpr1[17]
	 * and output it on the pin */
	clrsetbits_le32(&iomuxc_gpr_regs->gpr[1],
		IOMUXC_GPR_GPR1_GPR_ENET1_TX_CLK_SEL_MASK,
		IOMUXC_GPR_GPR1_GPR_ENET1_CLK_DIR_MASK);
#else
	/* Use 50M external CLK for ENET1, set gpr1[13], clear gpr1[17] */
	clrsetbits_le32(&iomuxc_gpr_regs->gpr[1],
		IOMUXC_GPR_GPR1_GPR_ENET1_CLK_DIR_MASK,
		IOMUXC_GPR_GPR1_GPR_ENET1_TX_CLK_SEL_MASK);
#endif
	ret = set_clk_enet(ENET_50MHz);
	if (ret)
		return ret;

	return 0;
}


int board_phy_config(struct phy_device *phydev)
{
#if 0
	/* enable rgmii rxc skew and phy mode select to RGMII copper */
	phy_write(phydev, MDIO_DEVAD_NONE, 0x1e, 0x21);
	phy_write(phydev, MDIO_DEVAD_NONE, 0x1f, 0x7ea8);
	phy_write(phydev, MDIO_DEVAD_NONE, 0x1e, 0x2f);
	phy_write(phydev, MDIO_DEVAD_NONE, 0x1f, 0x71b7);
#endif

	if (phydev->drv->config)
		phydev->drv->config(phydev);
	return 0;
}
#endif

int board_early_init_f(void)
{
	setup_dtemode_uart();
	setup_iomux_uart();

#ifdef CONFIG_SYS_I2C_MXC
	setup_i2c(0, CONFIG_SYS_I2C_SPEED, 0x7f, &i2c_pad_info1);
	setup_i2c(3, CONFIG_SYS_I2C_SPEED, 0x7f, &i2c_pad_info4);
#endif

	return 0;
}

int board_init(void)
{
	/* address of boot parameters */
	gd->bd->bi_boot_params = PHYS_SDRAM + 0x100;

#ifdef CONFIG_FEC_MXC
	setup_fec();
#endif

#ifdef CONFIG_SYS_USE_NAND
	setup_gpmi_nand();
#endif

#ifdef CONFIG_FSL_QSPI
	board_qspi_init();
#endif

	return 0;
}

int checkboard_fallback(void)
{
	printf("Model: Toradex Colibri iMX7 %s\n",
		(gd->ram_size == 0x20000000) ? "Dual" : "Solo");
	return 0;
}

#ifdef CONFIG_CMD_BMODE
static const struct boot_mode board_boot_modes[] = {
	/* 4 bit bus width */
	{"nand", MAKE_CFGVAL(0x40, 0x34, 0x00, 0x00)},
	{"sd1", MAKE_CFGVAL(0x10, 0x10, 0x00, 0x00)},
	{NULL, 0},
};
#endif

#ifdef CONFIG_POWER
#define I2C_PMIC	0
int power_init_board(void)
{
	struct pmic *p;
	int ret;

	unsigned int reg, ver;

	ret = power_rn5t567_init(I2C_PMIC);
	if (ret)
		return ret;

	p = pmic_get("RN5T567");
	ret = pmic_probe(p);
	if (ret)
		return ret;

	pmic_reg_read(p, RN5T567_LSIVER, &ver);
	pmic_reg_read(p, RN5T567_OTPVER, &reg);
	printf("PMIC: RN5T567 LSIVER=0x%x OTPVER=0x%x\n", ver, reg);

	/* set jduge and press timer of N_OE to minimal values */
	pmic_reg_read(p, RN5T567_NOETIMSETCNT, &reg);
	reg &= ~0x7;
	pmic_reg_write(p, RN5T567_NOETIMSETCNT, reg);

	return 0;
}

void reset_cpu(ulong addr)
{
	struct pmic *p;
	p = pmic_get("RN5T567");

	/* Use PMIC to reset, set REPWRTIM to 0 and REPWRON to 1 */
	pmic_reg_write(p, RN5T567_REPCNT, 0x1);
	pmic_reg_write(p, RN5T567_SLPCNT, 0x1);
}
#endif

int board_late_init(void)
{
#ifdef CONFIG_CMD_BMODE
	add_board_boot_modes(board_boot_modes);
#endif

#ifdef CONFIG_ENV_IS_IN_MMC
	board_late_mmc_init();
#endif

	return 0;
}

#ifdef CONFIG_USB_EHCI_MX7
iomux_v3_cfg_t const usb_otg1_pads[] = {
	MX7D_PAD_GPIO1_IO05__USB_OTG1_PWR | MUX_PAD_CTRL(NO_PAD_CTRL),
};

iomux_v3_cfg_t const usb_otg2_pads[] = {
	MX7D_PAD_UART3_CTS_B__GPIO4_IO7 | MUX_PAD_CTRL(NO_PAD_CTRL),
};
#define USBH_PWR IMX_GPIO_NR(4, 7)

int board_ehci_hcd_init(int port)
{
	switch (port) {
	case 0:
#if 0 /* TODO make this a USB device */
		imx_iomux_v3_setup_multiple_pads(usb_otg1_pads,
						 ARRAY_SIZE(usb_otg1_pads));
#endif
		break;
	case 1:
		/* i.MX 7Solo only has one USB instance... */
		if (is_cpu_type(MXC_CPU_MX7S))
			return -ENODEV;

		imx_iomux_v3_setup_multiple_pads(usb_otg2_pads,
						 ARRAY_SIZE(usb_otg2_pads));
		gpio_direction_output(USBH_PWR , 1);
		break;
	default:
		printf("MXC USB port %d not yet supported\n", port);
		return 1;
	}
	return 0;
}

int board_ehci_power(int port, int on)
{
	switch (port) {
	case 0:
		break;
	case 1:
		imx_iomux_v3_setup_multiple_pads(usb_otg2_pads,
						 ARRAY_SIZE(usb_otg2_pads));
		gpio_direction_output(USBH_PWR , !on);
		break;
	default:
		printf("MXC USB port %d not yet supported\n", port);
		return 1;
	}
	return 0;
}

#endif

#ifdef CONFIG_FASTBOOT

void board_fastboot_setup(void)
{
	switch (get_boot_device()) {
#if defined(CONFIG_FASTBOOT_STORAGE_MMC)
	case SD1_BOOT:
	case MMC1_BOOT:
		if (!getenv("fastboot_dev"))
			setenv("fastboot_dev", "mmc0");
		if (!getenv("bootcmd"))
			setenv("bootcmd", "booti mmc0");
		break;
	case SD3_BOOT:
	case MMC3_BOOT:
		if (!getenv("fastboot_dev"))
			setenv("fastboot_dev", "mmc1");
		if (!getenv("bootcmd"))
			setenv("bootcmd", "booti mmc1");
		break;
#endif /*CONFIG_FASTBOOT_STORAGE_MMC*/
	default:
		printf("unsupported boot devices\n");
		break;
	}
}

#ifdef CONFIG_ANDROID_RECOVERY

/* Use S3 button for recovery key */
#define GPIO_VOL_DN_KEY IMX_GPIO_NR(5, 10)
iomux_v3_cfg_t const recovery_key_pads[] = {
	(MX7D_PAD_SD2_WP__GPIO5_IO10 | MUX_PAD_CTRL(BUTTON_PAD_CTRL)),
};

int check_recovery_cmd_file(void)
{
	int button_pressed = 0;
	int recovery_mode = 0;

	recovery_mode = recovery_check_and_clean_flag();

	/* Check Recovery Combo Button press or not. */
	imx_iomux_v3_setup_multiple_pads(recovery_key_pads,
		ARRAY_SIZE(recovery_key_pads));

	gpio_direction_input(GPIO_VOL_DN_KEY);

	if (gpio_get_value(GPIO_VOL_DN_KEY) == 0) { /* VOL_DN key is low assert */
		button_pressed = 1;
		printf("Recovery key pressed\n");
	}

	return recovery_mode || button_pressed;
}

void board_recovery_setup(void)
{
	int bootdev = get_boot_device();

	switch (bootdev) {
#if defined(CONFIG_FASTBOOT_STORAGE_MMC)
	case SD1_BOOT:
	case MMC1_BOOT:
		if (!getenv("bootcmd_android_recovery"))
			setenv("bootcmd_android_recovery", "booti mmc0 recovery");
		break;
	case SD3_BOOT:
	case MMC3_BOOT:
		if (!getenv("bootcmd_android_recovery"))
			setenv("bootcmd_android_recovery", "booti mmc1 recovery");
		break;
#endif /*CONFIG_FASTBOOT_STORAGE_MMC*/
	default:
		printf("Unsupported bootup device for recovery: dev: %d\n",
			bootdev);
		return;
	}

	printf("setup env for recovery..\n");
	setenv("bootcmd", "run bootcmd_android_recovery");
}
#endif /*CONFIG_ANDROID_RECOVERY*/

#endif /*CONFIG_FASTBOOT*/

#ifdef CONFIG_IMX_UDC
void udc_pins_setting(void)
{
}

#endif /*CONFIG_IMX_UDC*/
