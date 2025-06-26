/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2017 Theobroma Systems Design und Consulting GmbH
 */

#ifndef __RK_VOP2_H__
#define __RK_VOP2_H__

#include <asm/arch-rockchip/vop_rk3568.h>

struct rk_vop2_priv {
	void *grf;
	void *regs;
	int vp;
};

enum vop_features {
	VOP_FEATURE_OUTPUT_10BIT = (1 << 0),
};

struct rkvop_platdata {
	const u8 delay;
	const u8 bg_dly[3]; /* VOP2 supports up to 4 video ports (0-3) */
};

struct rkvop_driverdata {
	/* configuration */
	u32 features;
	void (*platdata);
	/* block-specific setters/getters */
	void (*enable_output)(struct udevice *dev, enum vop_modes mode, u32 port);
	void (*set_pin_polarity)(struct udevice *dev, enum vop_modes mode, u32 port);
};

/**
 * rk_vop2_probe() - common probe implementation
 *
 * Performs the rk_display_init on each port-subnode until finding a
 * working port (or returning an error if none of the ports could be
 * successfully initialised).
 *
 * @dev:	device
 * Return: 0 if OK, -ve if something went wrong
 */
int rk_vop2_probe(struct udevice *dev);

/**
 * rk_vop2_bind() - common bind implementation
 *
 * Sets the plat->size field to the amount of memory to be reserved for
 * the framebuffer: this is always
 *     (32 BPP) x VIDEO_ROCKCHIP_MAX_XRES x VIDEO_ROCKCHIP_MAX_YRES
 *
 * @dev:	device
 * Return: 0 (always OK)
 */
int rk_vop2_bind(struct udevice *dev);

#endif
