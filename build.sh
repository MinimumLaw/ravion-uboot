#!/bin/bash

export ARCH=arm
export CROSS_COMPILE=armv7a-hardfloat-linux-gnueabi-

if [ -z ${1} ]; then
    if [ -f .config ]; then
	make -j5
    else
	make clean distclean
	make colibri_vf_defconfig
	make -j5
    fi
    if [ -f ./u-boot-nand.imx ]; then
	echo Update TFTP u-boot.imx
	cp -f ./u-boot-nand.imx /cimc/exporttftp/
	md5sum ./u-boot-nand.imx /cimc/exporttftp/u-boot-nand.imx
    fi
else
    make $*
fi

