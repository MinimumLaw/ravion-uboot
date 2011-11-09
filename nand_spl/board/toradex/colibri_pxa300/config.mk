#
# (C) Copyright 2008-2011
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
#
# Toradex Colibri XScale PXA300 module
#

#
# TEXT_BASE for SPL:
#
# On PXA300 platforms the SPL is initially located in the first page on
# NAND and gets automatically copied into the internal SRAM by the chip
# internal bootrom. So we set TEXT_BASE to starting address in internal
# SRAM here.
#
TEXT_BASE = 0x5C013000

# PAD_TO used to generate a 16kByte binary needed for the combined image
# -> PAD_TO = TEXT_BASE + 0x4000
PAD_TO = 0x5C017000

PLATFORM_CPPFLAGS += -DCONFIG_CPU_MONAHANS=1

ifeq ($(debug),1)
PLATFORM_CPPFLAGS += -DDEBUG
endif
