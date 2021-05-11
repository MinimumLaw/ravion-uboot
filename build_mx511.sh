#!/bin/bash
############################################
#     NOT      ACTUAL      BOOTLOADER      #
#    THIS CODE JUST MEMTEST TOOLS FOR TEST #
# DRAM CHIPS AND NETS                      #
#    Write  utsvu_sdcard.img  to  microSD, #
# put SA1  3,4,5 to on, install microSD to #
# card slot, power on, boot, user memtest. #
############################################

if [ -z $CROSS_COMPILE ]; then
	export CROSS_COMPILE=arm-linux-gnueabihf-
fi

if [ -f .config ]; then
	make $*
else
	make ravion_mx511_defconfig
	make $*
fi

if [ -f u-boot-dtb.imx ]; then
	dd if=u-boot-dtb.imx of=utsvu_sdcard.img seek=2 bs=512
	echo Use off-tree genimage to build realy bootable sdcard
fi
