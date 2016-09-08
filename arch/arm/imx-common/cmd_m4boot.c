/*
 * Copyright (c) 2015, Stefan Agner.  All rights reserved.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <bootm.h>
#include <command.h>
#include <malloc.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/crm_regs.h>
#include <asm/io.h>
#include <image.h>
#include <inttypes.h>

#ifndef CONFIG_SYS_FDT_PAD
#define CONFIG_SYS_FDT_PAD 0x3000
#endif

DECLARE_GLOBAL_DATA_PTR;

#define CCM_CCOWR_START 0x00015a5a

extern unsigned char _binary_arch_arm_imx_common_vf610m4bootldr_start;
extern unsigned char _binary_arch_arm_imx_common_vf610m4bootldr_end;

static void boot_startm4(void)
{
	struct ccm_reg *ccm = (struct ccm_reg *)CCM_BASE_ADDR;

	/* Gate Cortex-M4 clock... */
	writel(CCM_CCOWR_START, &ccm->ccowr);
}

static void boot_startm4_linux(bootm_headers_t *images)
{
	struct src *src = (struct src *)SRC_BASE_ADDR;
	int size = &_binary_arch_arm_imx_common_vf610m4bootldr_end -
		   &_binary_arch_arm_imx_common_vf610m4bootldr_start;
	ulong ep_loader = images->ep - size;

	/* Copy the Linux mini boot loader just in front of the image... */
	memcpy((void *)(images->os.image_start - size),
		&_binary_arch_arm_imx_common_vf610m4bootldr_start, size);

	printf("Booting Cortex-M4 @0x%08lx\n", ep_loader);

	/* Write entry point to GPR2 (PERSISTENT_ENTRY1) */
	writel(ep_loader, &src->gpr2);

	/* Write fdt offset to GPR3 (PERSISTENT_ARG1) */
	writel((ulong)images->ft_addr, &src->gpr3);

	flush_dcache_all();

	boot_startm4();
}

static void boot_startm4_generic(bootm_headers_t *images)
{
	struct src *src = (struct src *)SRC_BASE_ADDR;

	printf("Booting Cortex-M4 @0x%08lx\n", images->ep);

	/* Write entry point to GPR2 (PERSISTENT_ENTRY1) */
	writel(images->ep, &src->gpr2);

	/* Write fdt offset to GPR3 (PERSISTENT_ARG1) */
	writel(0, &src->gpr3);

	flush_dcache_all();

	boot_startm4();
}

static int fdt_chosenm4(void *fdt)
{
	int   nodeoffset;
	int   err;
	char  *str;		/* used to set string properties */

	err = fdt_check_header(fdt);
	if (err < 0) {
		printf("fdt_chosen: %s\n", fdt_strerror(err));
		return err;
	}

	/* find or create "/chosen" node. */
	nodeoffset = fdt_find_or_add_subnode(fdt, 0, "chosen");
	if (nodeoffset < 0)
		return nodeoffset;

	str = getenv("m4bootargs");
	if (str) {
		err = fdt_setprop(fdt, nodeoffset, "bootargs", str,
				  strlen(str) + 1);
		if (err < 0) {
			printf("WARNING: could not set bootargs %s.\n",
			       fdt_strerror(err));
			return err;
		}
	}

	return 0;
}

static int do_m4boot(cmd_tbl_t *cmdtp, int flag, int argc,
		       char * const argv[])
{
	int ret;
	bootm_headers_t images;
	ulong img_addr, initrd_len;

	memset(&images, '\0', sizeof(images));

	/* Consume 'm4boot' */
	argc--; argv++;

	/*
	 * We support FIT images only, call directly into FIT image functions
	 */
	bootm_disable_interrupts();
	lmb_init(&images.lmb);

	/*
	 * Parse provided address - default to load_addr in case not provided -
	 * and store config name as string in fit_uname_cfg (e.g. config@1)
	 */
	fit_parse_conf(argc < 1 ? NULL : argv[0], load_addr, &img_addr,
		       &images.fit_uname_cfg);


	/*
	 * Load OS image...
	 */
	ret = fit_image_load(&images, img_addr, &images.fit_uname_os,
			     &images.fit_uname_cfg, IH_ARCH_ARM,
			     IH_TYPE_KERNEL, BOOTSTAGE_ID_FIT_KERNEL_START,
			     FIT_LOAD_OPTIONAL_NON_ZERO,
			     &images.os.image_start, &images.os.image_len);

	if (ret < 0) {
		puts("ERROR: can't get kernel image!\n");
		return 1;
	}

	images.fit_noffset_os = ret;
	images.fit_hdr_os = map_sysmem(img_addr, 0);
	fit_image_get_entry(images.fit_hdr_os, images.fit_noffset_os,
			    &images.ep);
	fit_image_get_os(images.fit_hdr_os, images.fit_noffset_os,
			 &images.os.os);

	/* Verify entry point is a Thumb2 address */
	if (!(images.ep & 0x1)) {
		printf("Entry point 0x%08lx is not a valid Thumb2 address\n",
			images.ep);
		return 0;
	}

	ret = fit_image_load(&images, img_addr, &images.fit_uname_rd,
			     &images.fit_uname_cfg, IH_ARCH_ARM,
			     IH_TYPE_RAMDISK, BOOTSTAGE_ID_FIT_RD_START,
			     FIT_LOAD_OPTIONAL_NON_ZERO,
			     &images.initrd_start, &initrd_len);

	if (ret) {
		images.fit_noffset_rd = ret;
		images.initrd_end = images.initrd_start + initrd_len;
	}

	ret = fit_image_load(&images, img_addr, &images.fit_uname_fdt,
			     &images.fit_uname_cfg, IH_ARCH_ARM,
			     IH_TYPE_FLATDT, BOOTSTAGE_ID_FIT_FDT_START,
			     FIT_LOAD_OPTIONAL_NON_ZERO,
			     (ulong *)&images.ft_addr, &images.ft_len);

	if (ret > 0) {
		images.fit_noffset_fdt = ret;
		/*
		 * We assume there is space after the existing fdt to use
		 * for padding
		 */
		images.ft_len += CONFIG_SYS_FDT_PAD;
		lmb_reserve(&images.lmb, (ulong)images.ft_addr, images.ft_len);
		fdt_set_totalsize(images.ft_addr, images.ft_len);
		printf("   Using Device Tree in place at %p, end %p\n",
			images.ft_addr, images.ft_addr + images.ft_len - 1);

		set_working_fdt_addr((ulong)images.ft_addr);

		/*
		 * We could use image_setup_libfdt here, however that function
		 * goes through the whole arch/board/system fdt setup, which is
		 * not what we want for Cortex-M4
		 *
		 * Fixup some properties directly...
		 */
		ret = fdt_chosenm4(images.ft_addr);
		if (ret < 0)
			return ret;

		ret = fdt_initrd(images.ft_addr, images.initrd_start,
				 images.initrd_end);
		if (ret < 0)
			return ret;
	} else if (images.os.os == IH_OS_LINUX) {
		/* Linux on Cortex-M4 needs a valid device tree... */
		puts("Could not find a valid device tree\n");
		return 1;
	}

	if (images.os.os == IH_OS_LINUX)
		boot_startm4_linux(&images);
	else
		boot_startm4_generic(&images);

	return 1;
}

U_BOOT_CMD(
	m4boot, CONFIG_SYS_MAXARGS, 1, do_m4boot,
	"boot Linux on secondary Cortex-M4 core using FIT image from memory",
	"[<addr>[#<conf>]]\n"
	"    - boot Linux on secondary Cortex-M4 core using FIT image\n"
	"The argument needs to be a valid FIT image\n"
);
