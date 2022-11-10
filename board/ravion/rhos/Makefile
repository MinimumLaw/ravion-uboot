#
# Copyright 2017 NXP
#
# SPDX-License-Identifier:      GPL-2.0+
#

obj-y += imx8mq_rhos.o

ifdef CONFIG_SPL_BUILD
obj-y += spl.o
obj-$(CONFIG_IMX8M_LPDDR4) += lpddr4_timing.o
obj-$(CONFIG_POWER_PFUZE100)	+= pfuze.o
obj-$(CONFIG_DM_PMIC_PFUZE100)	+= pfuze.o
endif
