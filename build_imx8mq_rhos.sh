#!/bin/bash
##############################################################################
# Let's make flash.bin like NXP:
# see ./doc/board/nxp/imx8mq_evk.rst
#  get ATF from: https://source.codeaurora.org/external/imx/imx-atf
#  branch: imx_5.4.47_2.2.0
# change repo to https://github.com/nxp-imx/imx-atf, branch imx_5.4.47_2.2.0
#   $ make PLAT=imx8mq bl31
#   $ cp build/imx8mq/release/bl31.bin $(builddir)
# get the ddr and hdmi firmware
#   $ wget https://www.nxp.com/lgfiles/NMG/MAD/YOCTO/firmware-imx-8.9.bin
#   $ chmod +x firmware-imx-8.9.bin
#   $ ./firmware-imx-8.9.bin
#   $ cp firmware-imx-8.9/firmware/hdmi/cadence/signed_hdmi_imx8m.bin $(builddir)
#   $ cp firmware-imx-8.9/firmware/ddr/synopsys/lpddr4*.bin $(builddir)
# Build U-Boot
#   $ export CROSS_COMPILE=aarch64-poky-linux-
#   $ make imx8mq_evk_defconfig
#   $ make
# Burn the flash.bin to MicroSD card offset 33KB:
#   $ sudo dd if=flash.bin of=/dev/sd[x] bs=1024 seek=33 conv=notrunc
# Bring-up IMX8MQ RHOS module from scratch (USB factory recovery)
#   $ uuu -d imx8mq-rhos.uuu
##############################################################################
set -e
export CROSS_COMPILE=aarch64-linux-gnu-

if [ -f .config ]; then
    make $*
else
    make imx8mq_rhos_defconfig
    make $* flash.bin
fi