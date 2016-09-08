/*
 * Copyright (c) 2015 Sanchayan Maity <sanchayan.maity@toradex.com>
 * Copyright (C) 2015 Toradex AG
 *
 * Based on ehci-mx6 driver
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <usb.h>
#include <errno.h>
#include <linux/compiler.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/crm_regs.h>
#include <asm/imx-common/iomux-v3.h>
#include <usb/ehci-fsl.h>

#include "ehci.h"

#define USB_NC_REG_OFFSET				0x00000800
#define USBCx_CTRL_OFFSET				0x00000000
#define USBCx_PHY_CTRL_OFFSET			0x00000018

#define USBPHY_CTRL						0x00000030
#define USBPHY_CTRL_SET					0x00000034
#define USBPHY_CTRL_CLR					0x00000038
#define USBPHY_CTRL_TOG					0x0000003c

#define USBPHY_PWD						0x00000000
#define USBPHY_TX						0x00000010
#define USBPHY_RX						0x00000020
#define USBPHY_DEBUG					0x00000050
#define USBPHY_CTRL_SFTRST				0x80000000
#define USBPHY_CTRL_CLKGATE				0x40000000
#define USBPHY_CTRL_ENUTMILEVEL3		0x00008000
#define USBPHY_CTRL_ENUTMILEVEL2		0x00004000
#define USBPHY_CTRL_OTG_ID				0x08000000

#define ANADIG_PLL_CTRL_BYPASS			0x00010000
#define ANADIG_PLL_CTRL_ENABLE			0x00002000
#define ANADIG_PLL_CTRL_POWER			0x00001000
#define ANADIG_PLL_CTRL_EN_USB_CLKS		0x00000040

#define UCTRL_OVER_CUR_POL	(1 << 8) /* OTG Polarity of Overcurrent */
#define UCTRL_OVER_CUR_DIS	(1 << 7) /* Disable OTG Overcurrent Detection */

/* USBCMD */
#define UCMD_RUN_STOP		(1 << 0) /* controller run/stop */
#define UCMD_RESET			(1 << 1) /* controller reset */

static const unsigned phy_bases[] = {
	USB_PHY0_BASE_ADDR,
	USB_PHY1_BASE_ADDR,
};

static const unsigned nc_reg_bases[] = {
	USBC0_BASE_ADDR,
	USBC1_BASE_ADDR,
};

static void usb_internal_phy_clock_gate(int index)
{
	u32 reg;
	void __iomem *phy_reg;

	phy_reg = (void __iomem *)phy_bases[index];
	reg = __raw_readl(phy_reg + USBPHY_CTRL);
	reg &= ~USBPHY_CTRL_CLKGATE;
	__raw_writel(reg, phy_reg + USBPHY_CTRL_SET);
}

static void usb_power_config(int index)
{
	struct anadig_reg __iomem *anadig =
		(struct anadig_reg __iomem *)ANADIG_BASE_ADDR;
	void __iomem *pll_ctrl;
	u32 reg;

	switch (index) {
	case 0:
		pll_ctrl = &anadig->pll3_ctrl;
		break;
	case 1:
		pll_ctrl = &anadig->pll7_ctrl;
		break;
	default:
		return;
	}

	reg = __raw_readl(pll_ctrl);
	reg &= ~ANADIG_PLL_CTRL_BYPASS;
	__raw_writel(reg, pll_ctrl);

	reg = __raw_readl(pll_ctrl);
	reg |= ANADIG_PLL_CTRL_ENABLE | ANADIG_PLL_CTRL_POWER
			| ANADIG_PLL_CTRL_EN_USB_CLKS;
	__raw_writel(reg, pll_ctrl);
}

static void usb_phy_enable(int index, struct usb_ehci *ehci)
{
	void __iomem *phy_reg;
	void __iomem *phy_ctrl;
	void __iomem *usb_cmd;
	u32 val;

	phy_reg = (void __iomem *)phy_bases[index];
	phy_ctrl = (void __iomem *)(phy_reg + USBPHY_CTRL);
	usb_cmd = (void __iomem *)&ehci->usbcmd;

	/* Stop then Reset */
	val = __raw_readl(usb_cmd);
	val &= ~UCMD_RUN_STOP;
	__raw_writel(val, usb_cmd);
	while (__raw_readl(usb_cmd) & UCMD_RUN_STOP)
		;

	val = __raw_readl(usb_cmd);
	val |= UCMD_RESET;
	__raw_writel(val, usb_cmd);
	while (__raw_readl(usb_cmd) & UCMD_RESET)
		;

	/* Reset USBPHY module */
	val = __raw_readl(phy_ctrl);
	val |= USBPHY_CTRL_SFTRST;
	__raw_writel(val, phy_ctrl);
	udelay(10);

	/* Remove CLKGATE and SFTRST */
	val = __raw_readl(phy_ctrl);
	val &= ~(USBPHY_CTRL_CLKGATE | USBPHY_CTRL_SFTRST);
	__raw_writel(val, phy_ctrl);
	udelay(10);

	/* Power up the PHY */
	__raw_writel(0, phy_reg + USBPHY_PWD);
	/* enable FS/LS device */
	val = __raw_readl(phy_ctrl);
	val |= (USBPHY_CTRL_ENUTMILEVEL2 | USBPHY_CTRL_ENUTMILEVEL3);
	__raw_writel(val, phy_ctrl);
}

static void usb_oc_config(int index)
{
	void __iomem *ctrl;
	u32 val;

	ctrl = (void __iomem *)(nc_reg_bases[index] + USB_NC_REG_OFFSET +
						USBCx_CTRL_OFFSET);

	val = __raw_readl(ctrl);
	val |= UCTRL_OVER_CUR_POL;
	__raw_writel(val, ctrl);

	val = __raw_readl(ctrl);
	val |= UCTRL_OVER_CUR_DIS;
	__raw_writel(val, ctrl);
}

int __weak board_ehci_hcd_init(int port)
{
	return 0;
}

int ehci_hcd_init(int index, enum usb_init_type init,
		struct ehci_hccr **hccr, struct ehci_hcor **hcor)
{
	struct usb_ehci *ehci;

	if (index >= ARRAY_SIZE(nc_reg_bases))
		return -EINVAL;

	if (init == USB_INIT_DEVICE && index == 1)
		return -ENODEV;
	if (init == USB_INIT_HOST && index == 0)
		return -ENODEV;

	ehci = (struct usb_ehci *)nc_reg_bases[index];

	/* Do board specific initialisation */
	board_ehci_hcd_init(index);

	usb_power_config(index);
	usb_oc_config(index);
	usb_internal_phy_clock_gate(index);
	usb_phy_enable(index, ehci);

	*hccr = (struct ehci_hccr *)((uint32_t)&ehci->caplength);
	*hcor = (struct ehci_hcor *)((uint32_t)*hccr +
			HC_LENGTH(ehci_readl(&(*hccr)->cr_capbase)));

	if (init == USB_INIT_DEVICE) {
		setbits_le32(&ehci->usbmode, CM_DEVICE);
		__raw_writel((PORT_PTS_UTMI | PORT_PTS_PTW), &ehci->portsc);
		setbits_le32(&ehci->portsc, USB_EN);
	} else if (init == USB_INIT_HOST) {
		setbits_le32(&ehci->usbmode, CM_HOST);
		__raw_writel((PORT_PTS_UTMI | PORT_PTS_PTW), &ehci->portsc);
		setbits_le32(&ehci->portsc, USB_EN);
	}

	return 0;
}

int ehci_hcd_stop(int index)
{
	return 0;
}
