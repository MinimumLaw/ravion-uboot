/*
 * Copyright (C) 2015 Freescale Semiconductor, Inc.
 *               2015-2016 Toradex AG
 *
 * Configuration settings for the Colibri iMX7 module.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __COLIBRI_IMX7_CONFIG_H
#define __COLIBRI_IMX7_CONFIG_H

#include <asm/arch/imx-regs.h>
#include <linux/sizes.h>
#include "mx7_common.h"
#include <asm/imx-common/gpio.h>

#define CONFIG_MX7
#define CONFIG_SYS_THUMB_BUILD
#define CONFIG_USE_ARCH_MEMCPY
#define CONFIG_USE_ARCH_MEMSET

#define CONFIG_COLIBRI_IMX7
#define CONFIG_ROM_UNIFIED_SECTIONS
#define CONFIG_SYS_GENERIC_BOARD
#define CONFIG_ARCH_MISC_INIT
#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO_LATE

#define CONFIG_DBG_MONITOR
/* uncomment for PLUGIN mode support */
/* #define CONFIG_USE_PLUGIN */

/* uncomment for SECURE mode support */
/* #define CONFIG_SECURE_BOOT */

#ifdef CONFIG_SECURE_BOOT
#ifndef CONFIG_CSF_SIZE
#define CONFIG_CSF_SIZE 0x4000
#endif
#endif

#define CONFIG_CMDLINE_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG
#define CONFIG_REVISION_TAG
#define CONFIG_SERIAL_TAG
#define CONFIG_SERIAL_TAG_BOARD

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(32 * SZ_1M)

#define CONFIG_BOARD_EARLY_INIT_F
#define CONFIG_BOARD_LATE_INIT
#define CONFIG_MXC_GPIO
#define CONFIG_CMD_GPIO

#define CONFIG_MXC_UART
#define CONFIG_MXC_UART_BASE		UART1_IPS_BASE_ADDR

/* Make the HW version stuff available in U-Boot env */
#define CONFIG_VERSION_VARIABLE		/* ver environment variable */
#define CONFIG_ENV_VARS_UBOOT_CONFIG
#define CONFIG_ENV_VARS_UBOOT_RUNTIME_CONFIG

/* I2C configs */
#define CONFIG_CMD_I2C
#define CONFIG_SYS_I2C
#define CONFIG_SYS_I2C_MXC
#define CONFIG_SYS_I2C_SPEED		100000

/* OCOTP Configs */
#define CONFIG_CMD_FUSE
#ifdef CONFIG_CMD_FUSE
#define CONFIG_MXC_OCOTP
#endif

/* MMC Configs */
#define CONFIG_FSL_ESDHC
#define CONFIG_FSL_USDHC
#define CONFIG_SYS_FSL_ESDHC_ADDR	0

#define CONFIG_MMC
#define CONFIG_CMD_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_BOUNCE_BUFFER
#define CONFIG_CMD_FS_GENERIC
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_EXT3
#define CONFIG_CMD_EXT4
#define CONFIG_CMD_FAT
#define CONFIG_FAT_WRITE
#define CONFIG_DOS_PARTITION

#define PHYS_SDRAM_SIZE			SZ_2G

#define CONFIG_CMD_PING
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_MII
#define CONFIG_CMD_NET
#define CONFIG_FEC_MXC
#define CONFIG_MII
#define CONFIG_FEC_XCV_TYPE             RMII
#define CONFIG_ETHPRIME                 "FEC"
#define CONFIG_FEC_MXC_PHYADDR          1

#define CONFIG_PHYLIB
#define CONFIG_PHY_MICREL
#define CONFIG_FEC_DMA_MINALIGN		64
#define CONFIG_TFTP_TSIZE
#define CONFIG_IP_DEFRAG
#define CONFIG_TFTP_BLOCKSIZE		16384

/* ENET1 */
#define IMX_FEC_BASE			ENET_IPS_BASE_ADDR

/* PMIC */
#define CONFIG_POWER
#define CONFIG_POWER_I2C
#define CONFIG_POWER_RN5T567
#define CONFIG_POWER_RN5T567_I2C_ADDR	0x33

#undef CONFIG_BOOTM_NETBSD
#undef CONFIG_BOOTM_PLAN9
#undef CONFIG_BOOTM_RTEMS

#undef CONFIG_CMD_EXPORTENV
#undef CONFIG_CMD_IMPORTENV

/* USB Configs */
/* Host */
#define CONFIG_CMD_USB
#define CONFIG_USB_EHCI
#define CONFIG_USB_EHCI_MX7
#define CONFIG_USB_STORAGE
#define CONFIG_USB_HOST_ETHER
#define CONFIG_USB_ETHER_ASIX
#define CONFIG_USB_MAX_CONTROLLER_COUNT		2
#define CONFIG_EHCI_HCD_INIT_AFTER_RESET	/* For OTG port */
#define CONFIG_MXC_USB_PORTSC		(PORT_PTS_UTMI | PORT_PTS_PTW)
#define CONFIG_MXC_USB_FLAGS		0
#define CONFIG_USB_KEYBOARD
#ifdef CONFIG_USB_KEYBOARD
#define CONFIG_SYS_USB_EVENT_POLL
#define CONFIG_SYS_STDIO_DEREGISTER
#endif /* CONFIG_USB_KEYBOARD */
/* Client */
#define CONFIG_USB_GADGET
#define CONFIG_USB_GADGET_VBUS_DRAW	2
#define CONFIG_CI_UDC
#define CONFIG_USBD_HS
#define CONFIG_USB_GADGET_DUALSPEED

#define CONFIG_CMD_USB_MASS_STORAGE
#define CONFIG_USB_GADGET_MASS_STORAGE
#define CONFIG_USBDOWNLOAD_GADGET
#define CONFIG_TRDX_VID			0x1B67
#define CONFIG_TRDX_PID_COLIBRI_IMX6	0x0027
#define CONFIG_G_DNL_MANUFACTURER	"Toradex"
#define CONFIG_G_DNL_VENDOR_NUM		CONFIG_TRDX_VID
#define CONFIG_G_DNL_PRODUCT_NUM	CONFIG_TRDX_PID_COLIBRI_IMX6
/* USB DFU */
#define CONFIG_CMD_DFU
#define CONFIG_DFU_FUNCTION
#define CONFIG_DFU_MMC

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE
#define CONFIG_CONS_INDEX		1
#define CONFIG_BAUDRATE			115200
#define CONFIG_CMD_ASKENV

/* Command definition */
#include <config_cmd_default.h>

#undef CONFIG_CMD_IMLS

#undef CONFIG_BOOTDELAY
#define CONFIG_BOOTDELAY		1
#undef CONFIG_IPADDR
#define CONFIG_IPADDR			192.168.10.2
#define CONFIG_NETMASK			255.255.255.0
#undef CONFIG_SERVERIP
#define CONFIG_SERVERIP			192.168.10.1

#define CONFIG_LOADADDR			0x80800000
#define CONFIG_SYS_TEXT_BASE		0x87800000

#define CONFIG_CMD_BOOTAUX /* Boot M4 */
#define CONFIG_MXC_RDC /* Enable RDC to isolate the peripherals for A7 and M4 */
#define CONFIG_CMD_SETEXPR

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
	"run m4boot && " \
	"load mmc 0:1 ${kernel_addr_r} ${kernel_file} && " \
	"load mmc 0:1 ${fdt_addr_r} ${soc}-colibri-${fdt_board}.dtb && " \
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
		"ubi part ubi && run m4boot && " \
		"ubi read ${kernel_addr_r} kernel && " \
		"ubi read ${fdt_addr_r} dtb && " \
		"run fdt_fixup && bootz ${kernel_addr_r} - ${fdt_addr_r}\0" \

#define CONFIG_BOOTCOMMAND "run ubiboot; run sdboot; run nfsboot"

#define CONFIG_SYS_MMC_IMG_LOAD_PART	1

#define CONFIG_EXTRA_ENV_SETTINGS \
	MEM_LAYOUT_ENV_SETTINGS \
	NFS_BOOTCMD \
	SD_BOOTCMD \
	UBI_BOOTCMD \
	"console=ttymxc0\0" \
	"defargs=\0" \
	"fdt_board=eval-v3\0" \
	"fdt_fixup=;\0" \
	"m4boot=;\0" \
	"ip_dyn=yes\0" \
	"kernel_file=zImage\0" \
	"mtdparts=" MTDPARTS_DEFAULT "\0" \
	"setethupdate=if env exists ethaddr; then; else setenv ethaddr " \
		"00:14:2d:00:00:00; fi; tftpboot ${loadaddr} " \
		"${board}/flash_eth.img && source ${loadaddr}\0" \
	"setsdupdate=mmc rescan && setenv interface mmc && " \
		"fatload ${interface} 0:1 ${loadaddr} " \
		"${board}/flash_blk.img && source ${loadaddr}\0" \
	"setup=setenv setupargs " \
		"console=tty1 console=${console}" \
		",${baudrate}n8 ${memargs} consoleblank=0 ${mtdparts}\0" \
	"setupdate=run setsdupdate || run setusbupdate || run setethupdate\0" \
	"setusbupdate=usb start && setenv interface usb && " \
		"fatload ${interface} 0:1 ${loadaddr} " \
		"${board}/flash_blk.img && source ${loadaddr}\0" \
        "splashpos=m,m\0" \

/* Miscellaneous configurable options */
#define CONFIG_SYS_LONGHELP
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT		"Colibri iMX7 # "
#define CONFIG_AUTO_COMPLETE
#define CONFIG_SYS_CBSIZE		1024

/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_MAXARGS		256
#define CONFIG_SYS_BARGSIZE CONFIG_SYS_CBSIZE

#define CONFIG_SYS_ALT_MEMTEST
#define CONFIG_CMD_MEMTEST
#define CONFIG_SYS_MEMTEST_START	0x80000000
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_MEMTEST_START + 0x1c000000)

#define CONFIG_SYS_LOAD_ADDR		CONFIG_LOADADDR
#define CONFIG_SYS_HZ			1000

#define CONFIG_CMDLINE_EDITING
#define CONFIG_STACKSIZE		SZ_128K

/* Physical Memory Map */
#define CONFIG_NR_DRAM_BANKS		1
#define PHYS_SDRAM			MMDC0_ARB_BASE_ADDR

#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM
#define CONFIG_SYS_INIT_RAM_ADDR	IRAM_BASE_ADDR
#define CONFIG_SYS_INIT_RAM_SIZE	IRAM_SIZE

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

/* FLASH and environment organization */
#define CONFIG_SYS_NO_FLASH

#define CONFIG_ENV_SIZE			SZ_8K

#ifdef CONFIG_SYS_BOOT_NAND
#define CONFIG_SYS_USE_NAND
#define CONFIG_ENV_IS_IN_NAND
#else
#define CONFIG_ENV_IS_IN_MMC
#endif

#ifdef CONFIG_SYS_USE_NAND
#define CONFIG_CMD_NAND
#define CONFIG_CMD_WRITEBCB_MX7
#define CONFIG_BCH
#define CONFIG_CMD_NAND_TRIMFFS

/* NAND stuff */
#define CONFIG_NAND_MXS
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_BASE		0x40000000
#define CONFIG_SYS_NAND_5_ADDR_CYCLE
#define CONFIG_SYS_NAND_ONFI_DETECTION
#define CONFIG_SYS_NAND_MX7_GPMI_62_ECC_BYTES

/* UBI stuff */
#define CONFIG_RBTREE
#define CONFIG_LZO
#define CONFIG_CMD_UBI
#define CONFIG_MTD_UBI_FASTMAP
#define CONFIG_CMD_UBIFS	/* increases size by almost 60 KB */

/* Dynamic MTD partition support */
#define CONFIG_CMD_MTDPARTS	/* Enable 'mtdparts' command line support */
#define CONFIG_MTD_PARTITIONS
#define CONFIG_MTD_DEVICE	/* needed for mtdparts commands */
#define MTDIDS_DEFAULT		"nand0=gpmi-nand"
#define MTDPARTS_DEFAULT	"mtdparts=gpmi-nand:"		\
				"512k(mx7-bcb),"		\
				"3584k(u-boot)ro,"		\
				"512k(u-boot-env),"		\
				"-(ubi)"

/* DMA stuff, needed for GPMI/MXS NAND support */
#define CONFIG_APBH_DMA
#define CONFIG_APBH_DMA_BURST
#define CONFIG_APBH_DMA_BURST8
#endif

#if defined(CONFIG_ENV_IS_IN_MMC)
#define CONFIG_ENV_OFFSET		(8 * SZ_64K)
#elif defined(CONFIG_ENV_IS_IN_NAND)
#undef CONFIG_ENV_SIZE
#define CONFIG_ENV_OFFSET		(4 * 1024 * 1024)
#define CONFIG_ENV_SECT_SIZE		(128 * 1024)
#define CONFIG_ENV_SIZE			CONFIG_ENV_SECT_SIZE
#endif

#define CONFIG_SYS_FSL_USDHC_NUM	1
#define CONFIG_SYS_MMC_ENV_DEV		0   /* USDHC1 */
#define CONFIG_SYS_MMC_ENV_PART		0	/* user area */
#define CONFIG_MMCROOT			"/dev/mmcblk0p2"  /* USDHC1 */

#define CONFIG_OF_LIBFDT
#define CONFIG_CMD_BOOTZ
#define CONFIG_SUPPORT_RAW_INITRD

#define CONFIG_CMD_BMODE

#ifndef CONFIG_SYS_DCACHE_OFF
#define CONFIG_CMD_CACHE
#endif

#define CONFIG_VIDEO
#ifdef CONFIG_VIDEO
#define	CONFIG_CFB_CONSOLE
#define	CONFIG_VIDEO_MXS
#define	CONFIG_VIDEO_LOGO
#define	CONFIG_VIDEO_SW_CURSOR
#define	CONFIG_VGA_AS_SINGLE_DEVICE
#define	CONFIG_SYS_CONSOLE_IS_IN_ENV
#define	CONFIG_SPLASH_SCREEN
#define CONFIG_SPLASH_SCREEN_ALIGN
#define	CONFIG_CMD_BMP
#define	CONFIG_BMP_16BPP
#define	CONFIG_VIDEO_BMP_RLE8
#define CONFIG_VIDEO_BMP_LOGO
#endif

#define CONFIG_IMX_THERMAL

#endif	/* __CONFIG_H */
