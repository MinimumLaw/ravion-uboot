#!/bin/bash

export CROSS_COMPILE=armv5tel-softfloat-linux-gnueabi-
export MAKEARGS="CROSS_COMPILE=${CROSS_COMPILE}"

make $MAKEARGS clean distclean
make $MAKEARGS colibri_pxa320_nand_config
make $MAKEARGS
[ -f u-boot-nand.bin ] && mv u-boot-nand.bin pxa320-u-boot.binary

make $MAKEARGS clean distclean
make $MAKEARGS pm9g45_config
make $MAKEARGS
[ -f u-boot.bin ] && mv u-boot.bin pm9g45-u-boot.binary

make $MAKEARGS clean distclean
