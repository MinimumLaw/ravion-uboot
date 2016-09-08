#
# (C) Copyright 2000-2006
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# (C) Copyright 2011-2015 Freescale Semiconductor, Inc.
#
# SPDX-License-Identifier:	GPL-2.0+
#

ifeq ($(SOC),$(filter $(SOC),mx25 mx35 mx5 mx6 mx7 vf610))
obj-y	= iomux-v3.o
endif
ifeq ($(SOC),$(filter $(SOC),mx5 mx6))
obj-y	+= cpu.o speed.o
obj-$(CONFIG_SYS_I2C_MXC) += i2c-mxv7.o
endif
ifeq ($(SOC),$(filter $(SOC),mx5))
obj-y  += timer.o
endif
ifeq ($(SOC),$(filter $(SOC),mx7))
obj-y	+= cpu.o
obj-$(CONFIG_SYS_I2C_MXC) += i2c-mxv7.o
obj-$(CONFIG_GPT_TIMER) += timer.o
obj-$(CONFIG_MXC_RDC) += rdc-sema.o
obj-$(CONFIG_SYSCOUNTER_TIMER) += syscounter.o
endif
ifeq ($(SOC),$(filter $(SOC),mx7 mx6 mxs))
obj-y	+= misc.o
obj-$(CONFIG_SPL_BUILD)	+= spl.o
endif
ifeq ($(SOC),$(filter $(SOC),mx6))
obj-$(CONFIG_CMD_SATA) += sata.o
obj-$(CONFIG_IMX_VIDEO_SKIP) += video.o
obj-$(CONFIG_MXC_RDC) += rdc-sema.o
obj-$(CONFIG_GPT_TIMER) += timer.o
obj-$(CONFIG_SYSCOUNTER_TIMER) += syscounter.o
endif
ifeq ($(SOC),$(filter $(SOC),vf610))
obj-y += ddrmc-vf610.o

# The mini boot loader for Linux on the Cortex-M4 runs on ARMv7-M
# architecture, hence force ARMv7-M architecture. Use objcopy with
# binary-architecture (-B) to include the mini loader as binary blob
quiet_cmd_ccm4_s_S = M4CC    $@
cmd_ccm4_s_S       = $(CC) -Wall -march=armv7-m -mthumb -c -o $@ $<
arch/arm/imx-common/vf610m4bootldr.o: arch/arm/imx-common/vf610m4bootldr.S
	$(call if_changed,ccm4_s_S)

quiet_cmd_objcopym4 = OBJCOPY $@
cmd_objcopym4 = $(OBJCOPY) -O binary $< $@
arch/arm/imx-common/vf610m4bootldr: arch/arm/imx-common/vf610m4bootldr.o
	$(call if_changed,objcopym4)

quiet_cmd_objcopybinm4 = OBJCOPY $@
cmd_objcopybinm4 = $(OBJCOPY) -I binary -O elf32-littlearm -B arm $< $@
arch/arm/imx-common/vf610m4bootldrbin.o: arch/arm/imx-common/vf610m4bootldr
	$(call if_changed,objcopybinm4)

obj-$(CONFIG_CMD_M4BOOT) += vf610m4bootldrbin.o
endif
obj-$(CONFIG_CMD_BMODE) += cmd_bmode.o
obj-$(CONFIG_CMD_HDMIDETECT) += cmd_hdmidet.o
obj-$(CONFIG_CMD_DEKBLOB) += cmd_dek.o
obj-$(CONFIG_CMD_M4BOOT) += cmd_m4boot.o
obj-$(CONFIG_CMD_WRITEBCB) += cmd_writebcb.o
obj-$(CONFIG_CMD_WRITEBCB_MX7) += cmd_writebcb_mx7.o

quiet_cmd_cpp_cfg = CFGS    $@
      cmd_cpp_cfg = $(CPP) $(cpp_flags) -x c -o $@ $<

IMX_CONFIG = $(CONFIG_IMX_CONFIG:"%"=%).cfgtmp

$(IMX_CONFIG): %.cfgtmp: % FORCE
	$(Q)mkdir -p $(dir $@)
	$(call if_changed_dep,cpp_cfg)

MKIMAGEFLAGS_u-boot.imx = -n $(filter-out $< $(PHONY),$^) -T imximage \
	-e $(CONFIG_SYS_TEXT_BASE)

u-boot.imx: u-boot.bin $(IMX_CONFIG) FORCE
	$(call if_changed,mkimage)

ifeq ($(CONFIG_IMX_NAND),y)
quiet_cmd_u-boot-nand_imx = GEN     $@
cmd_u-boot-nand_imx = (dd bs=1024 count=1 if=/dev/zero 2>/dev/null) | \
       cat - $< > $@

u-boot-nand.imx: u-boot.imx $(IMX_CONFIG) FORCE
	$(call if_changed,u-boot-nand_imx)
endif

ifeq ($(CONFIG_OF_SEPARATE),y)
MKIMAGEFLAGS_u-boot-dtb.imx = -n $(filter-out $< $(PHONY),$^) -T imximage \
	-e $(CONFIG_SYS_TEXT_BASE)

u-boot-dtb.imx: u-boot-dtb.bin $(IMX_CONFIG) FORCE
	$(call if_changed,mkimage)
endif

MKIMAGEFLAGS_SPL = -n $(filter-out $< $(PHONY),$^) -T imximage \
	-e $(CONFIG_SPL_TEXT_BASE)

SPL: spl/u-boot-spl.bin $(IMX_CONFIG) FORCE
	$(call if_changed,mkimage)

MKIMAGEFLAGS_u-boot.uim = -A arm -O U-Boot -a $(CONFIG_SYS_TEXT_BASE) \
		-e $(CONFIG_SYS_TEXT_BASE) -C none

u-boot.uim: u-boot.bin FORCE
	$(call if_changed,mkimage)

OBJCOPYFLAGS += -I binary -O binary --pad-to=$(CONFIG_SPL_PAD_TO)
append = cat $(filter-out $< $(PHONY), $^) >> $@

quiet_cmd_pad_cat = CAT     $@
cmd_pad_cat = $(cmd_objcopy) && $(append) || rm -f $@

u-boot-with-spl.imx: SPL u-boot.uim FORCE
	$(call if_changed,pad_cat)

u-boot-with-nand-spl.imx: spl/u-boot-nand-spl.imx u-boot.uim FORCE
	$(call if_changed,pad_cat)

quiet_cmd_u-boot-nand-spl_imx = GEN     $@
cmd_u-boot-nand-spl_imx = (echo -ne '\x00\x00\x00\x00\x46\x43\x42\x20\x01' && \
	dd bs=1015 count=1 if=/dev/zero 2>/dev/null) | cat - $< > $@

spl/u-boot-nand-spl.imx: SPL FORCE
	$(call if_changed,u-boot-nand-spl_imx)

targets += $(addprefix ../../../,$(IMX_CONFIG) SPL u-boot.uim spl/u-boot-nand-spl.imx)
