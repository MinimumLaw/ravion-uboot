# Copyright (c) 2016 Toradex, Inc.
# SPDX-License-Identifier:	GPL-2.0+

# Common for all Toradex modules
ifeq ($(CONFIG_SPL_BUILD),y)
# Necessary to create built-in.o
obj-$(CONFIG_RAVION_CFG_BLOCK) += spl-cfg-block.o
else
obj-$(CONFIG_RAVION_CFG_BLOCK) += rav-cfg-block.o
endif
obj-y += rav-common.o
