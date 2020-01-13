#!/bin/bash

if [ -z $CROSS_COMPILE ]; then
	export CROSS_COMPILE=arm-linux-gnueabihf-
fi

if [ -f .config ]; then
	make $*
else
	make ravion_defconfig
	make $*
fi

if [ -f u-boot-with-spl.imx ]; then
	cp -f u-boot-with-spl.imx /cimc/exporttftp/u-boot-with-spl.imx
fi
