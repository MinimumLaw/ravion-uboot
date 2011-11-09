/*
 * (C) Copyright 2008-2011
 * Marcel Ziswiler, Noser Engineering, marcel.ziswiler@noser.com.
 *
 * (C) Copyright 2006 DENX Software Engineering
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <config.h>
#include <version.h>
#include <asm/arch/pxa-regs.h>

DRAM_SIZE:  .long   CONFIG_SYS_DRAM_SIZE

.macro wait time
	ldr		r2, =OSCR
	mov		r3, #0
	str		r3, [r2]
0:
	ldr		r3, [r2]
	cmp		r3, \time
	bls		0b
.endm

.globl lowlevel_init
lowlevel_init:
	/* Set up GPIO pins first */
	mov	 r10, lr

	/* Configure GPIO Pins 99, 100 UART1 / altern. Fkt. 1 */
	ldr		r0, =GPIO99
	ldr		r1, =0x801
	str		r1, [r0]

	ldr		r0, =GPIO100
	ldr		r1, =0x801
	str		r1, [r0]

	/*  Configure GPIO Pin 1 as Ethernet chip select 2 / altern. Fkt. 1 */
	ldr		r0, =GPIO1
	ldr		r1, =0x801
	str		r1, [r0]

	/*  Configure GPIO Pin 2 as CPLD chip select 3 / altern. Fkt. 1 */
	ldr		r0, =GPIO2
	ldr		r1, =0x801
	str		r1, [r0]

	/* set CSADRCFG2 for Ethernet chip */
	ldr		r1, =CSADRCFG2
	/* ALT 0b11 = one DF_SCLK of setup and one DF_SCLK of hold
	   ALW 0b001 = address latch width for synchronous device
	   ADDR CONFIG 0b011 = low-order addressing
	   ADDRSPLIT 0b1000 = address split by byte address bit 16
	   ADDRBASE 0b00 = address base by byte address bit <0>
	   INFTYPE 0b1011 = DFI AA/D multiplexing VLIO */
	ldr		r2, =0x0032C80B
	str		r2, [r1]		@ WRITE
	ldr		r2, [r1]		@ DELAY UNTIL WRITTEN

	/* set CSADRCFG3 for external chip selects (e.g. CAN, Ethernet on
	   Protea via CPLD) */
	ldr		r1, =CSADRCFG3
	ldr		r2, =0x0032C80B
	str		r2, [r1]		@ WRITE
	ldr		r2, [r1]		@ DELAY UNTIL WRITTEN

	ldr		r1, =MSC1
	/* set CS3 static memory control register for external chip selects (e.g. CAN,
	   Ethernet on Protea via CPLD):
	   RDN3 0b1111 = RDN of 7 external clock cycles
	   RDF3 0b1111 = RDF of 7 external clock cycles
	   RT3 0b001 = SRAM
	   set CS2 static memory control register for Ethernet chip:
	   RDN2 0b0010 = RDN of 7 external clock cycles
	   RDF2 0b0011 = RDF of 7 external clock cycles
	   RT2 0b001 = SRAM */
	ldr		r2, =0x07790779
	str		r2, [r1]		@ WRITE
	ldr		r2, [r1]		@ DELAY UNTIL WRITTEN

	/* tebrandt - ASCR, clear the RDH bit */
/* Application Subsystem Power Status/Configuration Register */
	ldr		r0, =ASCR
	ldr		r1, [r0]
	bic		r1, r1, #0x80000000
	str		r1, [r0]

#ifdef CONFIG_MMC
	/* Configure GPIOs for MMC/SD card slot 1 */
	ldr		r0, =GPIO3	/* MM1_DATA<0> */
	ldr		r1, =0x4904
	str		r1, [r0]

	ldr		r0, =GPIO4	/* MM1_DATA<1> */
	ldr		r1, =0x4904
	str		r1, [r0]

	ldr		r0, =GPIO5	/* MM1_DATA<2> */
	ldr		r1, =0x4904
	str		r1, [r0]

	ldr		r0, =GPIO6	/* MM1_DATA<3> */
	ldr		r1, =0x4904
	str		r1, [r0]

	ldr		r0, =GPIO7	/* MM1_CLK */
	ldr		r1, =0x4904
	str		r1, [r0]

	ldr		r0, =GPIO14	/* MM1_CMD */
	ldr		r1, =0x4905
	str		r1, [r0]
#endif /* CONFIG_MMC */

#ifdef CONFIG_USB_OHCI_NEW
	/*  Configure GPIO Pin 0_2 as USB host power enable / altern. Fkt. 1 */
	ldr		r0, =GPIO0_2
	ldr		r1, =0x801
	str		r1, [r0]

	/*  Configure GPIO Pin 1_2 as USB host overcurrent control / altern. Fkt. 1 */
	ldr		r0, =GPIO1_2
	ldr		r1, =0x801
	str		r1, [r0]
#endif /* CONFIG_USB_OHCI_NEW */

#if defined(CONFIG_NAND_SPL) || !defined(CONFIG_NAND_U_BOOT)
#ifndef CONFIG_SKIP_SDRAM_INIT
mem_init:
	/* setup memory controller
	   1 piece of 512 Mbit chip totalling 64 Mbyte
	   one 16-bit chip on 16-bit bus
	   4 banks x 8 Mbit x 16 organization
	   column address strobe latency CL = 3 at 133 MHz
	   1 bank x 13 rows x 10 columns address bit configuration */

	/* 1.4.3.7 on page 24 of vol 2
	   1. Set MDCNFG[HWFREQ]
	   2. Ensure MDCNFG[DMAP] is set to enable the phase detector.
	   4. Set application Subsystem Clock Configuration Register (ACCR)
	      register (ACCR[DMCFS]) to preferred frequency. */

	/* 1.4.3.3 on page 22 of vol 2
	   2. Appropriately program MDCNFG, leaving MDCNFG[DMCEN] cleared to
	   prevent the dynamic memory controller from accepting memory requests
	   until the SDRAM is conditioned and the DDR circuits are calibrated.
	   Enable the occupied DRAM partitions by setting appropriate
	   MDCNFG[DCSE] bits. When the partitions are enabled, SDCKE is
	   asserted and SDCLK<1:0> begins to toggle. */
	ldr		r0, =MDCNFG
/*	ldr		r1, =(MDCNFG_DMAP | MDCNFG_DTYPE | MDCNFG_DTC_2 |
			      MDCNFG_DCSE0 | MDCNFG_DRAC_13) */
	ldr		r1, =0x8000072D
	str		r1, [r0]
	ldr		r1, [r0]	/* delay until written */

	/* 3. Wait a typical NOP power-up waiting period required by SDRAM
	   (200 ms)
	   Optimization: Steps 4 and 6 can be done during this
	 */
	wait #0x300

	/* 4. Perform an initial Rcomp calibration cycle by setting the
	   RCOMP[SWEVAL] bit. Program the Rcomp Control register to perform
	   automatic Rcomp-evaluation cycles. */
	ldr		r0, =RCOMP
	ldr		r1, =0x000000FF
	str		r1, [r0]
	ldr		r1, [r0]	/* delay until written */

	/* 5. Perform DDR DRAM strobe delay calibration by enabling the
	   calibration-complete interrupt DMCEIER[EDLP], then set
	   DDR_HCAL[HCEN]. Software is interrupted when calibration is
	   complete. */
	ldr		r0, =DDR_HCAL
	ldr		r1, =0xb81dddc0
	str		r1, [r0]
	wait #0x200
	ldr		r1, [r0]	/* delay until written */

	/* 6. Write the MRS register to condition the SDRAM and send the MRS
	   command. The SDRAM gets "conditioned" by setting MDMRS[MDCOND],
	   before the MRS command MDMRS[13:0] is issued. This should be done by
	   writing to the MDMRS register once. Program the MDREFR[DRI] register
	   with the required refresh interval. */
	ldr		r0, =MDMRS
	ldr		r1, =0x40000033
	str		r1, [r0]
/*	wait	#0x300 */
	ldr		r1, [r0]

	ldr		r0, =MDREFR
	ldr		r1, =0x0000001E
	str		r1, [r0]
	ldr		r1, [r0]

	/* 7. Enable the dynamic memory controller by setting MDCNFG[DMCEN] to
	   allow the dynamic memory controller to accept memory requests. DMEMC
	   begins or resumes (if previously in self refresh mode) to normal
	   operation. */
	ldr		r0, =MDCNFG
	ldr		r1, [r0]
	orr		r1, r1, #MDCNFG_DMCEN
	str		r1, [r0]

#ifndef CONFIG_SYS_SKIP_DRAM_SCRUB
	/* scrub/init SDRAM if enabled/present */
	ldr	r8, =CONFIG_SYS_DRAM_BASE /* SDRAM base address */
	ldr	r9, =CONFIG_SYS_DRAM_SIZE /* memory size to scrub */
	mov	r0, #0			   /* scrub with 0x0000:0000 */
	mov	r1, #0
	mov	r2, #0
	mov	r3, #0
	mov	r4, #0
	mov	r5, #0
	mov	r6, #0
	mov	r7, #0
10:	/* fastScrubLoop */
	subs	r9, r9, #32	/* 8 words/line */
	stmia	r8!, {r0-r7}
	beq	15f
	b	10b
#endif /* CONFIG_SYS_SKIP_DRAM_SCRUB */
#endif /* CONFIG_SKIP_SDRAM_INIT */
#endif /* CONFIG_NAND_SPL || !CONFIG_NAND_U_BOOT */

15:
	/* Mask all interrupts */
	mov	r1, #0
	mcr	p6, 0, r1, c1, c0, 0	@ ICMR

	/* Disable software and data breakpoints */
	mov	r0, #0
	mcr	p15,0,r0,c14,c8,0  /* ibcr0 */
	mcr	p15,0,r0,c14,c9,0  /* ibcr1 */
	mcr	p15,0,r0,c14,c4,0  /* dbcon */

	/* Enable all debug functionality */
	mov	r0,#0x80000000
	mcr	p14,0,r0,c10,c0,0  /* dcsr */

endlowlevel_init:
	mov	pc, lr
