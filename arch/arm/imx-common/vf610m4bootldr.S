/*
 * Freescale Vybrid VF6xx M4 boot loader
 *
 * Enable PC/PS-Bus cache and get device tree pointer from Vybrid specific
 * argument register (PERSISTENT_ARG1)
 *
 * Copyright (c) 2014-2015 Stefan Agner <stefan@agner.ch>
 *
 */

#define	LMEM_PC_BASE	0xe0082000
#define	LMEM_PS_BASE	0xe0082800
#define LMEM_PXCCR	0x0
#define	SRC_BASE	0x4006e000
#define SRC_GPR2	0x28
#define SRC_GPR3	0x2c
	.thumb
	.align
main:
	.macro invalidate_cache, base
	.local invalidate_cache
	/* GO, INVW1 & INVW0 */
	mov	r3, #0x85
	lsl	r3, r3, #24

	/* ENCACHE */
	mov	r1, #0x1
	orr	r3, r1
	str	r3, [ \base, #LMEM_PXCCR ]

	ldr	r3, =0x1
	lsl	r3, r3, #31
1:
	ldr	r2, [ \base, #LMEM_PXCCR ]
	and	r2, r2, r3
	bne	1b
	.endm

	ldr	r0, lmem_ps_base
	invalidate_cache r0
	ldr	r0, lmem_pc_base
	invalidate_cache r0

	ldr	r0, src_base
	ldr	r1, machine_id		@ Machine ID
	ldr	r2, [ r0, #SRC_GPR3 ] 	@ DT pointer from argument register
	mov	r0, #0x0
	b	end

	.align

machine_id:
	.long	0xffffffff
src_base:
	.long	SRC_BASE
lmem_pc_base:
	.long	LMEM_PC_BASE
lmem_ps_base:
	.long	LMEM_PS_BASE
	.align 7
end:
