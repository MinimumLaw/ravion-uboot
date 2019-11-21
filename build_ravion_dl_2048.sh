#!/bin/bash

if [ -z $CROSS_COMPILE ]; then
	export CROSS_COMPILE=arm-linux-gnueabihf-
fi

if [ -f .config ]; then
	make $*
else
	make ravion_dl_2048_defconfig
	make $*
fi

if [ -f u-boot-dtb.imx ]; then
	cp -f u-boot-dtb.imx /cimc/exporttftp/u-boot-ravion-2048dl.imx
fi
