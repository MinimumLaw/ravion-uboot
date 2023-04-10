#!/bin/bash
if [ -z $CROSS_COMPILE ]; then
	export CROSS_COMPILE=aarch64-linux-gnu-
fi

pushd `pwd`/../rkbin/ || exit $?
    git checkout rk356x/firefly
popd
# export BL31=../rkbin/bin/rk35/rk3568_bl31_v1.36.elf
# export ROCKCHIP_TPL=../rkbin/bin/rk35/rk3568_ddr_1560MHz_v1.05-firefly.bin
export BL31=../rkbin/bin/rk35/rk3568_bl31_ultra_v2.13.elf
export ROCKCHIP_TPL=../rkbin/bin/rk35/rk3568_ddr_1560MHz_v1.15.bin

if [ -f .config ]; then
	make $*
else
	make evb-rk3568_defconfig
	make $*
fi
