#!/bin/bash

export CROSS_COMPILE=armv7a-hardfloat-linux-gnueabi- 

if [ -f .config ]; then
	make $*
else
	make ravion_imx6_nospl_defconfig
	make $*
fi
