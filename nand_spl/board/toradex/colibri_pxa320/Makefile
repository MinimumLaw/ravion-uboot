#
# (C) Copyright 2007-2011
# Marcel Ziswiler, Noser Engineering, marcel.ziswiler@noser.com.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

include $(TOPDIR)/config.mk
include $(TOPDIR)/nand_spl/board/$(BOARDDIR)/config.mk

LDSCRIPT= $(TOPDIR)/nand_spl/board/$(BOARDDIR)/u-boot.lds
LDFLAGS	= -Bstatic -T $(LDSCRIPT) -Ttext $(TEXT_BASE) $(PLATFORM_LDFLAGS)
AFLAGS	+= -DCONFIG_NAND_SPL
CFLAGS	+= -DCONFIG_NAND_SPL

SOBJS	= start.o lowlevel_init.o _divsi3.o _udivsi3.o
COBJS	= nand_boot.o nand_ecc.o pxa3xx_nand.o timer.o time.o interrupts.o \
	  div64.o colibri_pxa3xx.o

SRCS	:= $(addprefix $(obj),$(SOBJS:.o=.S) $(COBJS:.o=.c))
OBJS	:= $(addprefix $(obj),$(SOBJS) $(COBJS))
__OBJS	:= $(SOBJS) $(COBJS)
LNDIR	:= $(OBJTREE)/nand_spl/board/$(BOARDDIR)

nandobj	:= $(OBJTREE)/nand_spl/

ALL	= $(nandobj)u-boot-spl $(nandobj)u-boot-spl.bin $(nandobj)u-boot-spl-16k.bin

all:	$(obj).depend $(ALL)

$(nandobj)u-boot-spl-16k.bin: $(nandobj)u-boot-spl
	$(OBJCOPY) ${OBJCFLAGS} --pad-to=$(PAD_TO) -O binary $< $@

$(nandobj)u-boot-spl.bin:	$(nandobj)u-boot-spl
	$(OBJCOPY) ${OBJCFLAGS} -O binary $< $@

$(nandobj)u-boot-spl:	$(OBJS)
	cd $(LNDIR) && $(LD) $(LDFLAGS) $$UNDEF_SYM $(__OBJS) \
		-Map $(nandobj)u-boot-spl.map \
		-o $(nandobj)u-boot-spl

# create symbolic links for common files

# from cpu directory
$(obj)timer.c:
	@rm -f $(obj)timer.c
	ln -s $(SRCTREE)/arch/arm/cpu/pxa/timer.c $(obj)timer.c

$(obj)start.S:
	@rm -f $(obj)start.S
	ln -s $(SRCTREE)/arch/arm/cpu/pxa/start.S $(obj)start.S

# from library directory
$(obj)_divsi3.S:
	@rm -f $(obj)_divsi3.S
	ln -s $(SRCTREE)/arch/arm/lib/_divsi3.S $(obj)_divsi3.S
$(obj)_udivsi3.S:
	@rm -f $(obj)_udivsi3.S
	ln -s $(SRCTREE)/arch/arm/lib/_udivsi3.S $(obj)_udivsi3.S
$(obj)interrupts.c:
	@rm -f $(obj)interrupts.c
	ln -s $(SRCTREE)/arch/arm/lib/interrupts.c $(obj)interrupts.c
$(obj)div64.c:
	@rm -f $(obj)div64.c
	ln -s $(SRCTREE)/lib/div64.c $(obj)div64.c
$(obj)time.c:
	@rm -f $(obj)time.c
	ln -s $(SRCTREE)/lib/time.c $(obj)time.c

# from board directory
$(obj)colibri_pxa3xx.c:
	@rm -f $(obj)colibri_pxa3xx.c
	ln -s $(SRCTREE)/board/toradex/common/colibri_pxa3xx.c $(obj)colibri_pxa3xx.c
$(obj)lowlevel_init.S:
	@rm -f $(obj)lowlevel_init.S
	ln -s $(SRCTREE)/board/toradex/colibri_pxa320/lowlevel_init.S $(obj)lowlevel_init.S

# from nand_spl directory
$(obj)nand_boot.c:
	@rm -f $(obj)nand_boot.c
	ln -s $(SRCTREE)/nand_spl/nand_boot.c $(obj)nand_boot.c

# from drivers/nand directory
$(obj)nand_ecc.c:
	@rm -f $(obj)nand_ecc.c
	ln -s $(SRCTREE)/drivers/mtd/nand/nand_ecc.c $(obj)nand_ecc.c
$(obj)pxa3xx_nand.c:
	@rm -f $(obj)pxa3xx_nand.c
	ln -s $(SRCTREE)/drivers/mtd/nand/pxa3xx_nand.c $(obj)pxa3xx_nand.c

#########################################################################

$(obj)%.o:	$(obj)%.S
	$(CC) $(AFLAGS) -c -o $@ $<

$(obj)%.o:	$(obj)%.c
	$(CC) $(CFLAGS) -c -o $@ $<

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
