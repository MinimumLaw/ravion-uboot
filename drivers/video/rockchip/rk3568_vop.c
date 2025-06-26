// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2024 Dang Huynh <danct12@riseup.net>
 *
 * Based on rk3399_vop.c:
 *   Copyright (c) 2017 Theobroma Systems Design und Consulting GmbH
 *   Copyright (c) 2015 Google, Inc
 *   Copyright 2014 Rockchip Inc.
 */

#include <clk.h>
#include <display.h>
#include <dm.h>
#include <dm/device_compat.h>
#include <regmap.h>
#include <video.h>
#include <asm/arch-rockchip/hardware.h>
#include <asm/global_data.h>
#include <linux/bitfield.h>
#include "rk_vop2.h"

DECLARE_GLOBAL_DATA_PTR;

#define M_MIPI1_INFACE_MUX (3 << 21)
#define M_LVDS_INFACE_MUX (3 << 18)
#define M_MIPI_INFACE_MUX (3 << 16)
#define M_EDP_INFACE_MUX (3 << 14)
#define M_HDMI_INFACE_MUX (3 << 10)
#define M_RGB_INFACE_MUX (3 << 8)

#define V_MIPI1_INFACE_MUX(x) (((x) & 3) << 21)
#define V_LVDS_INFACE_MUX(x) (((x) & 3) << 18)
#define V_MIPI_INFACE_MUX(x) (((x) & 3) << 16)
#define V_EDP_INFACE_MUX(x) (((x) & 3) << 14)
#define V_HDMI_INFACE_MUX(x) (((x) & 3) << 10)
#define V_RGB_INFACE_MUX(x) (((x) & 3) << 8)

#define M_MIPI_POL (0xf << 16)
#define M_EDP_POL (0xf << 12)
#define M_HDMI_POL (0xf << 4)
#define M_RGB_LVDS_POL (0xf << 0)

#define V_MIPI_POL(x) (((x) & 0xf) << 16)
#define V_EDP_POL(x) (((x) & 0xf) << 12)
#define V_HDMI_POL(x) (((x) & 0xf) << 4)
#define V_RGB_LVDS_POL(x) (((x) & 0xf) << 0)

#define M_MIPI1_OUT_EN (1 << 20)
#define M_BT656_OUT_EN (1 << 7)
#define M_BT1120_OUT_EN (1 << 6)
#define M_LVDS_OUT_EN (1 << 5)
#define M_MIPI_OUT_EN (1 << 4)
#define M_EDP_OUT_EN (1 << 3)
#define M_HDMI_OUT_EN (1 << 1)
#define M_RGB_OUT_EN (1 << 0)

#define M_ALL_OUT_EN (M_MIPI1_OUT_EN | M_BT656_OUT_EN | M_BT1120_OUT_EN | M_LVDS_OUT_EN | \
			M_MIPI_OUT_EN | M_EDP_OUT_EN | M_HDMI_OUT_EN | M_RGB_OUT_EN)

#define V_MIPI1_OUT_EN(x) (((x) & 1) << 20)
#define V_BT656_OUT_EN(x) (((x) & 1) << 7)
#define V_BT1120_OUT_EN(x) (((x) & 1) << 6)
#define V_LVDS_OUT_EN(x) (((x) & 1) << 5)
#define V_MIPI_OUT_EN(x) (((x) & 1) << 4)
#define V_EDP_OUT_EN(x) (((x) & 1) << 3)
#define V_HDMI_OUT_EN(x) (((x) & 1) << 1)
#define V_RGB_OUT_EN(x) (((x) & 1) << 0)

static void rk3568_enable_output(struct udevice *dev,
				 enum vop_modes mode, u32 port)
{
	struct rk_vop2_priv *priv = dev_get_priv(dev);
	struct rk3568_vop_sysctrl *sysctrl = priv->regs + VOP2_SYSREG_OFFSET;
	u32 reg;

	switch (mode) {
	case VOP_MODE_EDP:
		reg |= M_EDP_OUT_EN | V_EDP_INFACE_MUX(port);
		break;

	case VOP_MODE_HDMI:
		reg |= M_HDMI_OUT_EN | V_HDMI_INFACE_MUX(port);
		break;

	case VOP_MODE_MIPI:
		reg |= M_MIPI_OUT_EN | V_MIPI_INFACE_MUX(port);
		break;

	case VOP_MODE_LVDS:
		reg |= M_LVDS_OUT_EN | V_LVDS_INFACE_MUX(port);
		break;

	default:
		debug("%s: unsupported output mode %x\n", __func__, mode);
		return;
	}

	debug("%s: vop output 0x%08x\n", __func__, reg);
	writel(reg, &sysctrl->dsp_en);
}

static void rk3568_set_pin_polarity(struct udevice *dev,
				    enum vop_modes mode, u32 polarity)
{
	struct rk_vop2_priv *priv = dev_get_priv(dev);
	struct rk3568_vop_sysctrl *sysctrl = priv->regs + VOP2_SYSREG_OFFSET;
	u32 reg;

	reg = M_DSP_INFACE_REGDONE;

	switch (mode) {
	case VOP_MODE_EDP:
		reg |= V_EDP_POL(polarity);
		break;

	case VOP_MODE_HDMI:
		reg |= V_HDMI_POL(polarity);
		break;

	case VOP_MODE_MIPI:
		reg |= V_MIPI_POL(polarity);
		break;

	/* RGB and LVDS shares the same polarity */
	case VOP_MODE_LVDS:
		reg |= V_RGB_LVDS_POL(polarity);
		break;

	default:
		debug("%s: unsupported output mode %x\n", __func__, mode);
		return;
	}

	debug("%s: vop polarity 0x%08x\n", __func__, reg);
	writel(reg, &sysctrl->dsp_pol);
}

static int rkvop_initialize(struct udevice *dev)
{
	struct rk_vop2_priv *priv = dev_get_priv(dev);
	struct rk3568_vop_sysctrl *sysctrl = priv->regs + VOP2_SYSREG_OFFSET;
	struct clk aclk;
	int ret;

	ret = clk_get_by_name(dev, "aclk", &aclk);
	if (ret < 0)
		return ret;

	ret = clk_enable(&aclk);
	if (ret < 0) {
		dev_err(dev, "Failed to enable aclk: %d\n", ret);
		return ret;
	}

	debug("aclk rate: %ld\n", clk_get_rate(&aclk));

	/* Enable OTP function */
	clrsetbits_le32(&sysctrl->otp_win, M_OTP_WIN, V_OTP_WIN(1));

	writel(M_GLOBAL_REGDONE, &sysctrl->reg_cfg_done);

	/* Disable auto gating */
	clrsetbits_le32(&sysctrl->autogating_ctrl, M_AUTO_GATING, V_AUTO_GATING(0));

	return 0;
}

/*
 * FIXME: Booting into Linux with a window plane enabled causes VOP IOMMU
 * to fail.
 *
 * This can be removed when there's a better way to handle MMU under Linux.
 */
static int rk3568_vop_remove(struct udevice *dev)
{
	struct rk_vop2_priv *priv = dev_get_priv(dev);
	struct rk3568_vop_sysctrl *sysctrl = priv->regs + VOP2_SYSREG_OFFSET;
	struct rk3568_vop_esmart *esmart = priv->regs + VOP2_ESMART_OFFSET(0);
	struct rk3568_vop_post *post = priv->regs + VOP2_POST_OFFSET(priv->vp);

	debug("Removing VOP2 driver (vp=%d)\n", priv->vp);

	/*
	 * It's best to put the display to standby mode to prevent
	 * unexpected behaviors.
	 */
	clrsetbits_le32(&post->dsp_ctrl, M_POST_STANDBY, V_POST_STANDBY(1));
	mdelay(120);

	writel(0, &esmart->esmart_region0_mst_ctl);

	/* On RK3568, we don't need to shift the bit by 16. */
	writel(M_GLOBAL_REGDONE | M_LOAD_GLOBAL(priv->vp),
	       &sysctrl->reg_cfg_done);

	return 0;
}

static int rk3568_vop_probe(struct udevice *dev)
{
	int ret;

	/* Before relocation we don't need to do anything */
	if (!(gd->flags & GD_FLG_RELOC))
		return 0;

	ret = rkvop_initialize(dev);
	if (ret)
		return ret;

	return rk_vop2_probe(dev);
}

struct rkvop_platdata rk3568_platdata = {
	.delay = 20,
	.bg_dly = {42, 40, 40},
};

struct rkvop_driverdata rk3568_driverdata = {
	.features = VOP_FEATURE_OUTPUT_10BIT,
	.set_pin_polarity = rk3568_set_pin_polarity,
	.enable_output = rk3568_enable_output,
	.platdata = &rk3568_platdata,
};

static const struct udevice_id rk3568_vop_ids[] = {
	{ .compatible = "rockchip,rk3566-vop",
	  .data = (ulong)&rk3568_driverdata },
	{ .compatible = "rockchip,rk3568-vop",
	  .data = (ulong)&rk3568_driverdata },
	{ }
};

static const struct video_ops rk3568_vop_ops = {
};

U_BOOT_DRIVER(rk3568_vop) = {
	.name	= "rk3568_vop",
	.id	= UCLASS_VIDEO,
	.of_match = rk3568_vop_ids,
	.ops	= &rk3568_vop_ops,
	.bind	= rk_vop2_bind,
	.probe	= rk3568_vop_probe,
	.remove = rk3568_vop_remove,
	.priv_auto	= sizeof(struct rk_vop2_priv),
	.flags = DM_FLAG_PRE_RELOC | DM_FLAG_OS_PREPARE,
};
