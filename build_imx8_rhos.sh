#!/bin/bash
############################################
# MassProduction or debrick:               #
# ${PWD} # uuu -d rhos-imx8.uuu            #
# (a) put recovery jumper if required      #
# (b) remove old u-boot-dtb.img            #
# (c) remove/rename old uboot.env          #
############################################

if [ -z $CROSS_COMPILE ]; then
	export CROSS_COMPILE=aarch64-linux-gnu-
fi

if [ -f .config ]; then
	make $*
else
	make imx8mq_evk_defconfig
	make $*
fi
