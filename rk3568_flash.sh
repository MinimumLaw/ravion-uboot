#!/bin/bash

echo Bootloader
./../rkbin/tools/rkdeveloptool db ./../rkbin/rk356x_spl_loader_v1.21.113.bin
echo U-Boot image
sleep 1
./../rkbin/tools/rkdeveloptool wl 64 u-boot-rockchip.bin
echo Reset board
sleep 1
./../rkbin/tools/rkdeveloptool rd
