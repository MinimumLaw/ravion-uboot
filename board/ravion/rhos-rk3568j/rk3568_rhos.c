#include <env.h>
#include <init.h>
#include <malloc.h>
#include <errno.h>
#include <asm/global_data.h>
#include <asm/io.h>
#include <miiphy.h>
#include <netdev.h>
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
#warning FixMe: This start ONLY if boot from ethernet, but must start allways!
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

int rk_board_late_init(void)
{
#ifdef CONFIG_ENV_VARS_UBOOT_RUNTIME_CONFIG
	env_set("board_name", "RHOS");
	env_set("board_rev",  "RK3568J");
#endif

	return 0;
}

#endif
