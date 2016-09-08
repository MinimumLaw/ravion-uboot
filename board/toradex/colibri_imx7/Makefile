# (C) Copyright 2015 Toradex AG
#
# SPDX-License-Identifier:	GPL-2.0+
#

obj-y  := colibri_imx7.o
obj-$(CONFIG_TRDX_CMD_IMX_MFGR)  += do_fuse.o

extra-$(CONFIG_USE_PLUGIN) :=  plugin.bin
$(obj)/plugin.bin: $(obj)/plugin.o
	$(OBJCOPY) -O binary --gap-fill 0xff $< $@
