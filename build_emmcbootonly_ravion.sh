#!/bin/bash
############################################
# MassProduction or debrick:               #
# ${PWD} # uuu -d ravion-v2.uuu            #
# (a) put recovery jumper if required      #
# (b) remove old u-boot-dtb.img            #
# (c) remove/rename old uboot.env          #
############################################

if [ -z $CROSS_COMPILE ]; then
	export CROSS_COMPILE=arm-linux-gnueabihf-
fi

if [ -f .config ]; then
	make $*
else
	make ravion_emmcbootonly_defconfig
	make $*
fi

if [ -f SPL ]; then
	cp -f SPL /cimc/exporttftp/SPL
fi

if [ -f u-boot-dtb.img ]; then
	sudo cp -f u-boot-dtb.img /cimc/root/colibri-imx6/boot/
	sudo cp -f SPL /cimc/root/colibri-imx6/boot/
fi
