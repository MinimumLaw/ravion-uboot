#!/bin/bash

MKARGS="ARCH=arm CROSS_COMPILE=arm-fsl-linux-gnueabi-"

make $MKARGS clean distclean
make $MKARGS mx51rav_config
make $MKARGS -j3 u-boot.imx

#######################################################
# Commented danegous code for writing to SD/partition #
#######################################################
#sudo dd if=u-boot.imx of=/dev/sdb bs=512 seek=2 && sudo sync && sudo sync && sudo hexdump -C /dev/sdb | less
#
#if [ -b /dev/sdb1 ]; then
#    # copy u-boot.imx on first mmc partition
#    sudo mount /dev/sdb1 /mnt/flash
#    sudo cp -f u-boot.imx /mnt/flash/
#    sudo umount /mnt/flash
#    sudo sync && sudo sync
#fi
