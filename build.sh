#!/bin/bash

export CROSS_COMPILE=arm-linux-gnueabihf-

if [ -f .config ]; then
	make $*
else
	make mx6ul_var_dart_mmc_defconfig
#	make mx6ul_var_dart_mmc_SECURE_BOOT_defconfig
	make $*
fi