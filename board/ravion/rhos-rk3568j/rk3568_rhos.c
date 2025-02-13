#include <env.h>
#include <init.h>
#include <malloc.h>
#include <errno.h>
#include <asm/global_data.h>
#include <asm/arch-rockchip/bootrom.h>
#include <asm/io.h>
#include <miiphy.h>
#include <netdev.h>
#include <net-common.h>
#include <mmc.h>
#include <spl.h>
#include <linux/bitops.h>

#ifdef CONFIG_SPL_BUILD
/*
 * SPL hooks
 */
#else
/*
 * U-Boot hooks
 */
int board_phy_config(struct phy_device *phydev)
{
	int reg, ret = 0;
	int devad = MDIO_DEVAD_NONE;

	if (phydev->drv->config)
		ret = phydev->drv->config(phydev);

	reg = phy_read(phydev, devad, 0x0467);
	if ((reg & 0x0300) != 0x0200) {
		pr_err("PHY Strap failed (0x%04x), fix to RGMII/Auto-MDX mode.\n", reg);
		reg = (phy_read(phydev, devad, 0x0017) | (1<<9));  /* RGMII mode */
		phy_write(phydev, devad, 0x0017, reg);
		reg = (phy_read(phydev, devad, 0x0019) | (1<<15)); /* Auto-MDX mode */
		phy_write(phydev, devad, 0x0019, reg);
	}

	return ret;
}

#ifdef CONFIG_RESET_PHY_R
void reset_phy(void)
{
	struct udevice *eth_dev;

	eth_dev = eth_get_dev_by_name("eth0");
	if (!eth_dev) {
		printf("ETH device not found!");
		return;
	}
#warning No wait for link required!
	eth_start_udev(eth_dev); /* We want call board_phy_config() now */

	printf("PHY reset at startup callback\n");
}
#endif


int rk_board_late_init(void)
{

#ifdef CONFIG_ENV_VARS_UBOOT_RUNTIME_CONFIG
	switch (readl(BROM_BOOTSOURCE_ID_ADDR)){
	case BROM_BOOTSOURCE_EMMC: /* normal production boot */
		env_set("board_name", "RHOS");
		env_set("board_rev",  "RK3568J");
		break;
	case BROM_BOOTSOURCE_USB:  /* module flashing by USB */
		printf("USB boot detected. Start in fastboot mode.\n");
		env_set("bootcmd", "fastboot usb 0");
		break;
	case BROM_BOOTSOURCE_SD:  /* module flashing by USB */
		printf("Factory boot mode - install bootloader\n");
		env_set("bootcmd", "run boot0_install");
		break;
	};
#endif
	return 0;
}

#endif
