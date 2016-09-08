#
# Copyright 2013 Freescale Semiconductor, Inc.
#
# SPDX-License-Identifier:	GPL-2.0+
#

obj-y	+= generic.o
ifneq ($(CONFIG_SYS_GLOBAL_TIMER),y)
obj-y	+= timer.o
endif
obj-$(CONFIG_SYS_GLOBAL_TIMER)	+= global_timer.o
