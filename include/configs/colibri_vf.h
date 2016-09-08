/*
 * Copyright 2013-2015 Toradex, Inc.
 *
 * Configuration settings for the Toradex VF50/VF61 module.
 *
 * Based on vf610twr.h:
 * Copyright 2013 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <asm/arch/imx-regs.h>
#include <config_cmd_default.h>
#include <linux/sizes.h>

#define CONFIG_VF610
#define CONFIG_SYS_THUMB_BUILD
#define CONFIG_USE_ARCH_MEMCPY
#define CONFIG_USE_ARCH_MEMSET

#define CONFIG_SYS_GENERIC_BOARD
#define CONFIG_SYS_GLOBAL_TIMER
#define CONFIG_ARCH_CPU_INIT
#define CONFIG_ARCH_MISC_INIT
#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO_LATE

#define CONFIG_SKIP_LOWLEVEL_INIT

#define CONFIG_CMD_BMODE
#define CONFIG_CMD_FUSE
#ifdef CONFIG_CMD_FUSE
#define CONFIG_MXC_OCOTP
#endif

#define CONFIG_FSL_DCU_FB

#ifdef CONFIG_FSL_DCU_FB
#define CONFIG_VIDEO
#define CONFIG_CMD_BMP
#define CONFIG_CFB_CONSOLE
#define CONFIG_VGA_AS_SINGLE_DEVICE
#define CONFIG_SPLASH_SCREEN_ALIGN
#define CONFIG_VIDEO_LOGO
#define CONFIG_VIDEO_BMP_LOGO

#define CONFIG_SYS_CONSOLE_IS_IN_ENV
#define CONFIG_CONSOLE_MUX

#define CONFIG_SYS_FSL_DCU_LE
#define CONFIG_SYS_DCU_ADDR		DCU0_BASE_ADDR
#define CONFIG_FSL_DCU_MAX_FB_SIZE	(4 * SZ_1M)
#define DCU_TOTAL_LAYER_NUM		64
#define DCU_LAYER_MAX_NUM		6
#endif

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + 8 * 1024 * 1024)

#define CONFIG_BOARD_EARLY_INIT_F

#define CONFIG_FSL_LPUART
#define LPUART_BASE			UART0_BASE

/* Allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE
#define CONFIG_VERSION_VARIABLE
#define CONFIG_ENV_VARS_UBOOT_CONFIG
#define CONFIG_ENV_VARS_UBOOT_RUNTIME_CONFIG

#define CONFIG_SYS_UART_PORT		(0)
#define CONFIG_BAUDRATE			115200
#define CONFIG_CMD_ASKENV

/* NAND support */
#define CONFIG_CMD_NAND
#define CONFIG_CMD_WRITEBCB
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_BASE		NFC_BASE_ADDR

/* GPIO support */
#define CONFIG_DM_GPIO
#define CONFIG_CMD_GPIO
#define CONFIG_VYBRID_GPIO

/* Dynamic MTD partition support */
#define CONFIG_CMD_MTDPARTS	/* Enable 'mtdparts' command line support */
#define CONFIG_MTD_PARTITIONS
#define CONFIG_MTD_DEVICE	/* needed for mtdparts commands */
#define MTDIDS_DEFAULT		"nand0=vf610_nfc"
#define MTDPARTS_DEFAULT	"mtdparts=vf610_nfc:"		\
				"128k(vf-bcb)ro,"		\
				"1408k(u-boot)ro,"		\
				"512k(u-boot-env),"		\
				"-(ubi)"

#undef CONFIG_CMD_IMLS

#define CONFIG_MMC
#define CONFIG_FSL_ESDHC
#define CONFIG_SYS_FSL_ESDHC_ADDR	0
#define CONFIG_SYS_FSL_ESDHC_NUM	1

#define CONFIG_SYS_FSL_ERRATUM_ESDHC111

#define CONFIG_CMD_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_CMD_FAT
#define CONFIG_CMD_EXT3
#define CONFIG_CMD_EXT4
#define CONFIG_DOS_PARTITION

#define CONFIG_RBTREE
#define CONFIG_LZO
#define CONFIG_CMD_FS_GENERIC
#define CONFIG_CMD_UBI
#define CONFIG_MTD_UBI_FASTMAP
#define CONFIG_CMD_UBIFS	/* increases size by almost 60 KB */

#define CONFIG_CMD_PING
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_MII
#define CONFIG_CMD_NET
#define CONFIG_FEC_MXC
#define CONFIG_MII
#define IMX_FEC_BASE			ENET1_BASE_ADDR
#define CONFIG_FEC_XCV_TYPE		RMII
#define CONFIG_FEC_MXC_PHYADDR		0
#define CONFIG_PHYLIB
#define CONFIG_PHY_MICREL
#define CONFIG_TFTP_TSIZE
#define CONFIG_IP_DEFRAG
#define CONFIG_TFTP_BLOCKSIZE		16384

#define CONFIG_IPADDR		192.168.10.2
#define CONFIG_NETMASK		255.255.255.0
#define CONFIG_SERVERIP		192.168.10.1

#define CONFIG_BOOTDELAY		1
#define CONFIG_ZERO_BOOTDELAY_CHECK
#define CONFIG_BOARD_LATE_INIT

#define CONFIG_LOADADDR			0x80008000
#define CONFIG_FDTADDR			0x84000000

/* We boot from the gfxRAM area of the OCRAM. */
#define CONFIG_SYS_TEXT_BASE		0x3f408000
#define CONFIG_BOARD_SIZE_LIMIT		524288

#define MEM_LAYOUT_ENV_SETTINGS \
	"fdt_addr_r=0x82000000\0" \
	"fdt_high=0xffffffff\0" \
	"initrd_high=0xffffffff\0" \
	"kernel_addr_r=0x81000000\0" \
	"ramdisk_addr_r=0x82100000\0"

#define SD_BOOTCMD \
	"sdargs=root=/dev/mmcblk0p2 rw rootwait\0"	\
	"sdboot=run setup; setenv bootargs ${defargs} ${sdargs} " \
	"${setupargs} ${vidargs}; echo Booting from MMC/SD card...; " \
	"load mmc 0:2 ${kernel_addr_r} /boot/${kernel_file} && " \
	"load mmc 0:2 ${fdt_addr_r} /boot/${soc}-colibri-${fdt_board}.dtb && " \
	"run fdt_fixup && bootz ${kernel_addr_r} - ${fdt_addr_r}\0" \

#define NFS_BOOTCMD \
	"nfsargs=ip=:::::eth0: root=/dev/nfs\0"	\
	"nfsboot=run setup; " \
	"setenv bootargs ${defargs} ${nfsargs} " \
	"${setupargs} ${vidargs}; echo Booting from NFS...;" \
	"dhcp ${kernel_addr_r} && "	\
	"tftp ${fdt_addr_r} ${soc}-colibri-${fdt_board}.dtb && " \
	"run fdt_fixup && bootz ${kernel_addr_r} - ${fdt_addr_r}\0" \

#define UBI_BOOTCMD	\
	"ubiargs=ubi.mtd=ubi root=ubi0:rootfs rootfstype=ubifs " \
	"ubi.fm_autoconvert=1\0" \
	"ubiboot=run setup; " \
	"setenv bootargs ${defargs} ${ubiargs} " \
	"${setupargs} ${vidargs}; echo Booting from NAND...; " \
	"ubi part ubi && " \
	"ubi read ${kernel_addr_r} kernel && " \
	"ubi read ${fdt_addr_r} dtb && " \
	"run fdt_fixup && bootz ${kernel_addr_r} - ${fdt_addr_r}\0" \

#define CONFIG_BOOTCOMMAND "run ubiboot; run sdboot; run nfsboot"

#define DFU_ALT_NAND_INFO	"vf-bcb part 0,1;u-boot part 0,2;ubi part 0,4"

#define CONFIG_EXTRA_ENV_SETTINGS \
	MEM_LAYOUT_ENV_SETTINGS \
	"console=ttyLP0\0" \
	"defargs=\0" \
	"dfu_alt_info=" DFU_ALT_NAND_INFO "\0" \
	"fdt_board=eval-v3\0" \
	"fdt_file=${soc}-colibri-${fdt_board}.dtb\0" \
	"fdt_fixup=;\0" \
	"kernel_file=zImage\0" \
	"mtdparts=" MTDPARTS_DEFAULT "\0" \
	NFS_BOOTCMD \
	SD_BOOTCMD \
	"setethupdate=if env exists ethaddr; then; else setenv ethaddr " \
		"00:14:2d:00:00:00; fi; tftpboot ${loadaddr} " \
		"flash_eth.img && source ${loadaddr}\0" \
	"setsdupdate=mmc rescan && setenv interface mmc && " \
		"fatload ${interface} 0:1 ${loadaddr} flash_blk.img && " \
		"source ${loadaddr}\0" \
	"setup=setenv setupargs " \
		"console=tty1 console=${console}" \
		",${baudrate}n8 ${memargs} consoleblank=0\0" \
	"setupdate=run setsdupdate || run setusbupdate || run setethupdate\0" \
	"setusbupdate=usb start && setenv interface usb && " \
		"fatload ${interface} 0:1 ${loadaddr} flash_blk.img && " \
		"source ${loadaddr}\0" \
	"splashpos=m,m\0" \
	UBI_BOOTCMD \
	"video-mode=dcufb:640x480-16@60,monitor=lcd\0"

/* Miscellaneous configurable options */
#define CONFIG_SYS_LONGHELP		/* undef to save memory */
#define CONFIG_SYS_HUSH_PARSER		/* use "hush" command parser */
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "
#define CONFIG_SYS_PROMPT		"Colibri VFxx # "
#undef CONFIG_AUTO_COMPLETE
#define CONFIG_SYS_CBSIZE		1024	/* Console I/O Buffer Size */
#define CONFIG_SYS_PBSIZE		\
			(CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_MAXARGS		16	/* max number of command args */
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE

#define CONFIG_CMD_MEMTEST
#define CONFIG_SYS_MEMTEST_START	0x80010000
#define CONFIG_SYS_MEMTEST_END		0x87C00000

#define CONFIG_SYS_LOAD_ADDR		CONFIG_LOADADDR
#define CONFIG_SYS_HZ			1000
#define CONFIG_CMDLINE_EDITING

/*
 * Stack sizes
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE		SZ_256K

/* Physical memory map */
#define CONFIG_NR_DRAM_BANKS		1
#define PHYS_SDRAM			(0x80000000)
#define PHYS_SDRAM_SIZE			(256 * 1024 * 1024)

#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM
#define CONFIG_SYS_INIT_RAM_ADDR	IRAM_BASE_ADDR
#define CONFIG_SYS_INIT_RAM_SIZE	IRAM_SIZE

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

/* Environment organization */
#define CONFIG_SYS_NO_FLASH

#ifdef CONFIG_ENV_IS_IN_MMC
#define CONFIG_SYS_MMC_ENV_DEV		0
#define CONFIG_ENV_OFFSET		(12 * 64 * 1024)
#define CONFIG_ENV_SIZE			(8 * 1024)
#endif

#ifdef CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_SIZE			(64 * 2048)
#define CONFIG_ENV_RANGE		(4 * 64 * 2048)
#define CONFIG_ENV_OFFSET		(12 * 64 * 2048)
#endif

#define CONFIG_OF_LIBFDT
#define CONFIG_OF_BOARD_SETUP
#define CONFIG_OF_SYSTEM_SETUP
#define CONFIG_FDT_FIXUP_PARTITIONS

#define CONFIG_CMD_BOOTZ
#define CONFIG_SUPPORT_RAW_INITRD
#define CONFIG_SYS_BOOT_RAMDISK_HIGH

#define CONFIG_CMD_M4BOOT
#define CONFIG_CMD_CACHE
#define CONFIG_FIT

#define CONFIG_SYS_NO_FLASH

#define CONFIG_SYS_CACHELINE_SIZE 32

/* USB Host support */
#define CONFIG_CMD_USB
#define CONFIG_USB_EHCI
#define CONFIG_USB_EHCI_VF
#define CONFIG_USB_MAX_CONTROLLER_COUNT 2
#define CONFIG_EHCI_HCD_INIT_AFTER_RESET

/* USB Client Support */
#define CONFIG_USB_GADGET
#define CONFIG_CI_UDC
#define CONFIG_USB_GADGET_DUALSPEED
#define CONFIG_USB_GADGET_VBUS_DRAW	2
#define CONFIG_G_DNL_MANUFACTURER	"Toradex"
#define CONFIG_G_DNL_VENDOR_NUM		0x1b67
#define CONFIG_G_DNL_PRODUCT_NUM	0xffff /* Fallback, set using fixup */

/* USB DFU */
#define CONFIG_USBDOWNLOAD_GADGET
#define CONFIG_CMD_DFU
#define CONFIG_DFU_FUNCTION
#define CONFIG_DFU_NAND
#define CONFIG_DFU_MMC
#define CONFIG_SYS_DFU_DATA_BUF_SIZE (1024*1024)

/* USB Storage */
#define CONFIG_USB_STORAGE
#define CONFIG_USB_GADGET_MASS_STORAGE
#define CONFIG_CMD_USB_MASS_STORAGE

/* Enable SPI support */
#ifdef CONFIG_OF_CONTROL
#define CONFIG_DM_SPI
#define CONFIG_CMD_SPI
#define CONFIG_FSL_DSPI
#endif

#define CONFIG_CRC32_VERIFY

#endif /* __CONFIG_H */
