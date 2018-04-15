#!/bin/bash

export CROSS_COMPILE=arm-linux-gnueabihf-

if [ -f .config ]; then
	make $*
else
	make ravion_imx6_nospl_defconfig
	make $*
fi
