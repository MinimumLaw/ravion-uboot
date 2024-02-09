/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2021 Rockchip Electronics Co., Ltd
 */

#ifndef __RHOS_RK3568_H
#define __RHOS_RK3568_H

#include "rk3568_common.h"

#define RAVBOOT \
	"fdt_file=rk3568-rhos-kitsbimx8.dtb\0" \
	"ravboot=tftpboot ${loadaddr} ${serverip}:/boot/bscript.img; source ${loadaddr}\0" \
	"bootcmd=run ravboot\0"

#undef CFG_EXTRA_ENV_SETTINGS
#define CFG_EXTRA_ENV_SETTINGS \
	"__INF1__=RHOS RK3568 CPU Module BSP package\0" \
	"__INF2__=Created: Alex A. Mihaylov AKA MinimumLaw, MinimumLaw@Rambler.Ru\0" \
	"__INF3__=Request: Radioavionica Corp, Saint-Petersburg, Russia, 2024\0" \
	"__INF4__=License: GPL v2 and above, https://github.com/MinimumLaw\0" \
	ENV_MEM_LAYOUT_SETTINGS \
	RAVBOOT \
	"ipaddr=192.168.5.91\0" \
	"netmask=255.255.255.0\0" \
	"serverip=192.168.5.254\0" \
	"gatewayip=192.168.5.254\0" \
	"board=kitsbimx8"

#endif
