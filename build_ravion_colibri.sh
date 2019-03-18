#!/bin/bash

if [ -z $CROSS_COMPILE ]; then
	export CROSS_COMPILE=arm-linux-gnueabihf-
fi

if [ -f .config ]; then
	make $*
else
	make ravion_colibri_imx6_nospl_defconfig
	make $*
fi

if [ -f u-boot.imx ]; then
	cp -f u-boot.imx /cimc/exporttftp/u-boot-colibri.imx
fi
