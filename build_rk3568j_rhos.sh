#!/bin/bash
##############################################################################
#
# ATTENTION:
# ~~~~~~~~~~
#
# 1. Requre cloned https://github.com/rockchip-linux/rkbin.git into ./../rkbin
#    Be warned: need to fix debug uart speed to 115200 as defined in this code
# 2. Fixup BL31 and ROCKCHIP_TPL env bellow, if required
# 3. Write u-boot-rockcip.bin with offset 64 (0x40) to SD/eMMC
#
##############################################################################
export BL31=../rkbin/bin/rk35/rk3568_bl31_ultra_v2.17.elf
export ROCKCHIP_TPL=../rkbin/bin/rk35/rk3568_ddr_1560MHz_v1.21.bin

if [ -z $CROSS_COMPILE ]; then
	export CROSS_COMPILE=aarch64-linux-gnu-
fi

pushd `pwd`/../rkbin/ || exit $?
    git checkout master
popd


if [ -f .config ]; then
	make $*
else
	make rhos-rk3568j_defconfig
	make $*
fi
