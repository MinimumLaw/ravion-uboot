/*
 * Copyright (c) 2011-2015 Toradex, Inc.
 *
 * Configuration settings for the Toradex Colibri T20 modules.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include "tegra20-common.h"

#undef CONFIG_SYS_DCACHE_OFF		/* breaks L4T kernel boot */
#define CONFIG_ARCH_MISC_INIT

#define CONFIG_TEGRA_EARLY_TPS6586X

/* High-level configuration options */
#define V_PROMPT			"Colibri T20 # "
#define CONFIG_CUSTOM_BOARDINFO		/* not from device-tree model node */
#undef CONFIG_DISPLAY_BOARDINFO
#define CONFIG_DISPLAY_BOARDINFO_LATE

/* Board-specific serial config */
#define CONFIG_SERIAL_MULTI
#define CONFIG_TEGRA_ENABLE_UARTA
#define CONFIG_TEGRA_UARTA_SDIO1
#define CONFIG_SYS_NS16550_COM1		NV_PA_APB_UARTA_BASE

#define CONFIG_MACH_TYPE		MACH_TYPE_COLIBRI_T20

#define CONFIG_INITRD_TAG
#define CONFIG_REVISION_TAG
#define CONFIG_SERIAL_TAG

/* Make the HW version stuff available in U-Boot env */
#define CONFIG_VERSION_VARIABLE		/* ver environment variable */
#define CONFIG_ENV_VARS_UBOOT_RUNTIME_CONFIG
#define CONFIG_CMD_ASKENV

/* I2C */
#define CONFIG_SYS_I2C_TEGRA
#define CONFIG_CMD_I2C

/* SD/MMC support */
#define CONFIG_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_TEGRA_MMC
#define CONFIG_CMD_MMC

/* USB client support */
#define CONFIG_G_DNL_MANUFACTURER	"Toradex"
#define CONFIG_G_DNL_VENDOR_NUM		0x1b67
#define CONFIG_G_DNL_PRODUCT_NUM	0xffff /* Fallback, set using fixup */

/* USB DFU */
#define CONFIG_DFU_NAND

/* USB host support */
#define CONFIG_USB_EHCI
#define CONFIG_USB_EHCI_TEGRA
#define CONFIG_USB_ULPI
#define CONFIG_USB_ULPI_VIEWPORT
#define CONFIG_USB_MAX_CONTROLLER_COUNT	3
#define CONFIG_USB_STORAGE
#define CONFIG_CMD_USB

/* USB networking support */
#define CONFIG_USB_HOST_ETHER
#define CONFIG_USB_ETHER_ASIX

/* General networking support */
#define CONFIG_CMD_NET
#define CONFIG_CMD_DHCP
#define CONFIG_IP_DEFRAG
#define CONFIG_TFTP_BLOCKSIZE		1536
#define CONFIG_TFTP_TSIZE

/* LCD support */
#define CONFIG_LCD
#define CONFIG_PWM_TEGRA
#define CONFIG_VIDEO_TEGRA
#define LCD_BPP				LCD_COLOR16
#define CONFIG_SYS_WHITE_ON_BLACK
#define CONFIG_CONSOLE_SCROLL_LINES	10
#define CONFIG_CMD_BMP
#define CONFIG_LCD_LOGO

/* NAND support */
#define CONFIG_CMD_NAND
#define CONFIG_TEGRA_NAND
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_ONFI_DETECTION

/* Dynamic MTD partition support */
#define CONFIG_CMD_MTDPARTS	/* Enable 'mtdparts' command line support */
#define CONFIG_MTD_PARTITIONS
#define CONFIG_MTD_DEVICE	/* needed for mtdparts commands */
#define MTDIDS_DEFAULT		"nand0=tegra_nand"
#define MTDPARTS_DEFAULT	"mtdparts=tegra_nand:"		\
				"2m(u-boot)ro,"			\
				"1m(u-boot-env),"		\
				"1m(cfgblock)ro,"		\
				"-(ubi)"

#undef CONFIG_CMD_IMLS

/* Environment in NAND, 64K is a bit excessive but erase block is 512K anyway */
#define CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_OFFSET		(SZ_2M)
#undef CONFIG_ENV_SIZE	/* undef size from tegra20-common.h */
#define CONFIG_ENV_SIZE			(SZ_64K)
#define CONFIG_ENV_RANGE		(SZ_1M)

#define CONFIG_RBTREE
#define CONFIG_LZO
#define CONFIG_CMD_BOOTZ
#define CONFIG_CMD_UBI
#define CONFIG_MTD_UBI_FASTMAP
#define CONFIG_CMD_UBIFS	/* increases size by almost 60 KB */

/* Debug commands */
#define CONFIG_CMD_BDI
#define CONFIG_CMD_CACHE

/* Miscellaneous commands */
#define CONFIG_CMD_SETEXPR
#define CONFIG_FAT_WRITE

#undef CONFIG_BOOTDELAY
#define CONFIG_BOOTDELAY	1
#define CONFIG_ZERO_BOOTDELAY_CHECK
#undef CONFIG_IPADDR
#define CONFIG_IPADDR		192.168.10.2
#define CONFIG_NETMASK		255.255.255.0
#undef CONFIG_SERVERIP
#define CONFIG_SERVERIP		192.168.10.1

#define CONFIG_BOOTCOMMAND \
	"run ubiboot; echo; echo ubiboot failed; " \
	"run sdboot; echo; echo sdboot failed; " \
	"run usbboot; echo; echo usbboot failed; " \
	"run nfsboot; echo; echo nfsboot failed"

#define DFU_ALT_NAND_INFO	"u-boot part 0,0;ubi part 0,3"

#define NFS_BOOTCMD \
	"nfsargs=ip=:::::eth0:on root=/dev/nfs rw netdevwait\0" \
	"nfsboot=usb start; run setup; setenv bootargs ${defargs} ${mtdparts}" \
		" ${nfsargs} ${setupargs} ${vidargs}; echo Booting via DHCP/" \
		"TFTP/NFS...; run nfsdtbload; dhcp ${kernel_addr_r} " \
		"&& bootz ${kernel_addr_r} - ${dtbparam}\0" \
	"nfsdtbload=setenv dtbparam; tftp ${fdt_addr_r} " \
		"${soc}-colibri-${fdt_board}.dtb " \
		"&& setenv dtbparam ${fdt_addr_r}\0"

#define SD_BOOTCMD \
	"sdargs=ip=off root=/dev/mmcblk0p2 rw,noatime rootfstype=ext3 " \
		"rootwait\0" \
	"sdboot=run setup; setenv bootargs ${defargs} ${mtdparts} ${sdargs} " \
		"${setupargs} ${vidargs}; echo Booting from SD card...; " \
		"run sddtbload; load mmc 1:1 ${kernel_addr_r} ${boot_file} " \
		"&& bootz ${kernel_addr_r} - ${dtbparam}\0" \
	"sddtbload=setenv dtbparam; load mmc 1:1 ${fdt_addr_r} " \
		"${soc}-colibri-${fdt_board}.dtb " \
		"&& setenv dtbparam ${fdt_addr_r}\0"

#define UBI_BOOTCMD \
	"ubiargs=ubi.mtd=ubi root=ubi0:rootfs rootfstype=ubifs " \
		"ubi.fm_autoconvert=1\0" \
	"ubiboot=run setup; setenv bootargs ${defargs} ${ubiargs} ${mtdparts}" \
		" ${setupargs} ${vidargs}; echo Booting from NAND...; " \
		"ubi part ubi && " \
        "ubi read ${kernel_addr_r} kernel &&" \
        "run ubidtbload; " \
		"bootz ${kernel_addr_r} - ${dtbparam}; " \
		"bootz ${kernel_addr_r} - \0" \
	"ubidtbload=setenv dtbparam; " \
		"ubi read ${fdt_addr_r} dtb && " \
		"setenv dtbparam ${fdt_addr_r}\0"

#define USB_BOOTCMD \
	"usbargs=ip=off root=/dev/sda2 rw,noatime rootfstype=ext3 " \
		"rootwait\0" \
	"usbboot=run setup; setenv bootargs ${defargs} ${mtdparts} " \
		"${setupargs} ${usbargs} ${vidargs}; echo Booting from USB " \
		"stick...; usb start && run usbdtbload; load usb 0:1 " \
		"${kernel_addr_r} ${boot_file} && " \
		"bootz ${kernel_addr_r} - ${dtbparam}\0" \
	"usbdtbload=setenv dtbparam; load usb 0:1 ${fdt_addr_r} " \
		"${soc}-colibri-${fdt_board}.dtb " \
		"&& setenv dtbparam ${fdt_addr_r}\0"

#define BOARD_EXTRA_ENV_SETTINGS \
	"boot_file=zImage\0" \
	"console=ttyS0\0" \
	"defargs=vmalloc=128M usb_high_speed=1\0" \
	"dfu_alt_info=" DFU_ALT_NAND_INFO "\0" \
	"fdt_board=eval-v3\0" \
	"mtdparts=" MTDPARTS_DEFAULT "\0" \
	NFS_BOOTCMD \
	SD_BOOTCMD \
	"setethupdate=if env exists ethaddr; then; else setenv ethaddr " \
		"00:14:2d:00:00:00; fi; usb start && tftpboot ${loadaddr} " \
		"flash_eth.img && source ${loadaddr}\0" \
	"setsdupdate=setenv interface mmc; setenv drive 0; mmc rescan; load " \
		"${interface} ${drive}:1 ${loadaddr} flash_blk.img && " \
		"source ${loadaddr}\0" \
	"setup=setenv setupargs asix_mac=${ethaddr} " \
		"consoleblank=0 no_console_suspend=1 console=tty1 " \
		"console=${console},${baudrate}n8 debug_uartport=lsport,0 " \
		"${memargs}\0" \
	"setupdate=run setsdupdate || run setusbupdate || run setethupdate\0" \
	"setusbupdate=usb start && setenv interface usb; setenv drive 0; " \
		"load ${interface} ${drive}:1 ${loadaddr} flash_blk.img && " \
		"source ${loadaddr}\0" \
	UBI_BOOTCMD \
	USB_BOOTCMD \
	"vidargs=video=tegrafb0:640x480-16@60\0"

/* Increase console I/O buffer size */
#undef CONFIG_SYS_CBSIZE
#define CONFIG_SYS_CBSIZE		1024

/* Increase arguments buffer size */
#undef CONFIG_SYS_BARGSIZE
#define CONFIG_SYS_BARGSIZE CONFIG_SYS_CBSIZE

/* Increase print buffer size */
#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)

/* Increase maximum number of arguments */
#undef CONFIG_SYS_MAXARGS
#define CONFIG_SYS_MAXARGS		32

#define CONFIG_CMD_TIME
#define CONFIG_CMD_MEMTEST
#define CONFIG_SYS_ALT_MEMTEST

#define CONFIG_OF_SYSTEM_SETUP

#define CONFIG_SUPPORT_RAW_INITRD
#define CONFIG_SYS_BOOT_RAMDISK_HIGH

#include "tegra-common-usb-gadget.h"
#undef CONFIG_SYS_DFU_DATA_BUF_SIZE
#include "tegra-common-post.h"

#endif /* __CONFIG_H */
