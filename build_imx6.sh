#!/bin/bash
#################################################################
#              ! ! !    A T T E N T I O N    ! ! !              #
#################################################################
# GCC verision 4.5 build u-boot binary, but resulted bootloader #
# can't load linux kernel (zImage and uImage).  Working version #
# build with GCC version 4.9 (tested) or above.                 #
#################################################################

export ARCH=arm
export CROSS_COMPILE=arm-hardfloat-linux-gnueabi-

if [ -z ${1} ]; then
    if [ -f .config ]; then
	make -j5
    else
	make clean distclean
	make colibri_imx6_defconfig
	make -j5
    fi
    if [ -f ./u-boot.imx ]; then
	echo Update TFTP u-boot.imx
	cp -f ./u-boot.imx /cimc/exporttftp/
	md5sum ./u-boot.imx /cimc/exporttftp/u-boot.imx
    fi
else
    make $*
fi

