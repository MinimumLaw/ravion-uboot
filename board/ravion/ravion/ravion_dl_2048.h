/*
 *
 */

#define RAVION_DL_2048 { \
/* DDR IO TYPE: */ \
	0x020e0774,	0x000C0000,	/* IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE */ \
	0x020e0754,	0x00000000,	/* IOMUXC_SW_PAD_CTL_GRP_DDRPKE */ \
/*CLOCK: */ \
	0x020e04ac,	0x00000028,	/* IOMUXC_SW_PAD_CTL_PAD_DRAM_SDCLK_0 */ \
	0x020e04b0,	0x00000028,	/* IOMUXC_SW_PAD_CTL_PAD_DRAM_SDCLK_1 */ \
/*ADDRESS: */ \
	0x020e0464,	0x00000028,	/* IOMUXC_SW_PAD_CTL_PAD_DRAM_CAS */ \
	0x020e0490,	0x00000028,	/* IOMUXC_SW_PAD_CTL_PAD_DRAM_RAS */ \
	0x020e074c,	0x00000028,	/* IOMUXC_SW_PAD_CTL_GRP_ADDDS  */ \
/*Control: */ \
	0x020e0494,	0x00000028,	/* IOMUXC_SW_PAD_CTL_PAD_DRAM_RESET */ \
	0x020e04a0,	0x00000000,	/* IOMUXC_SW_PAD_CTL_PAD_DRAM_SDBA2 - DSE can be configured using Group Control Register: IOMUXC_SW_PAD_CTL_GRP_CTLDS */ \
	0x020e04b4,	0x00000028,	/* IOMUXC_SW_PAD_CTL_PAD_DRAM_SDODT0 */ \
	0x020e04b8,	0x00000028,	/* IOMUXC_SW_PAD_CTL_PAD_DRAM_SDODT1 */ \
	0x020e076c,	0x00000028,	/* IOMUXC_SW_PAD_CTL_GRP_CTLDS  */ \
/*Data Strobes: */ \
	0x020e0750,	0x00020000,	/* IOMUXC_SW_PAD_CTL_GRP_DDRMODE_CTL */ \
	0x020e04bc,	0x00000028,	/* IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS0 */ \
	0x020e04c0,	0x00000028,	/* IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS1 */ \
	0x020e04c4,	0x00000028,	/* IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS2 */ \
	0x020e04c8,	0x00000028,	/* IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS3 */ \
	0x020e04cc,	0x00000028,	/* IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS4 */ \
	0x020e04d0,	0x00000028,	/* IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS5 */ \
	0x020e04d4,	0x00000028,	/* IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS6 */ \
	0x020e04d8,	0x00000028,	/* IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS7 */ \
/*Data:	 */ \
	0x020e0760,	0x00020000,	/* IOMUXC_SW_PAD_CTL_GRP_DDRMODE */ \
	0x020e0764,	0x00000028,	/* IOMUXC_SW_PAD_CTL_GRP_B0DS */ \
	0x020e0770,	0x00000028,	/* IOMUXC_SW_PAD_CTL_GRP_B1DS */ \
	0x020e0778,	0x00000028,	/* IOMUXC_SW_PAD_CTL_GRP_B2DS */ \
	0x020e077c,	0x00000028,	/* IOMUXC_SW_PAD_CTL_GRP_B3DS */ \
	0x020e0780,	0x00000028,	/* IOMUXC_SW_PAD_CTL_GRP_B4DS */ \
	0x020e0784,	0x00000028,	/* IOMUXC_SW_PAD_CTL_GRP_B5DS */ \
	0x020e078c,	0x00000028,	/* IOMUXC_SW_PAD_CTL_GRP_B6DS */ \
	0x020e0748,	0x00000028,	/* IOMUXC_SW_PAD_CTL_GRP_B7DS */ \
	0x020e0470,	0x00000028,	/* IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM0 */ \
	0x020e0474,	0x00000028,	/* IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM1 */ \
	0x020e0478,	0x00000028,	/* IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM2 */ \
	0x020e047c,	0x00000028,	/* IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM3 */ \
	0x020e0480,	0x00000028,	/* IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM4 */ \
	0x020e0484,	0x00000028,	/* IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM5 */ \
	0x020e0488,	0x00000028,	/* IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM6 */ \
	0x020e048c,	0x00000028,	/* IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM7 */ \
/*=============================================================================*/ \
/* DDR Controller Registers                                                    */ \
/*=============================================================================*/ \
/* Manufacturer:	Micron			*/ \
/* Device Part Number:	MT41K256M16TW-107	*/ \
/* Clock Freq.: 	400MHz		 */ \
/* Density per CS in Gb: 	16	 */ \
/* Chip Selects used:	1		 */ \
/* Number of Banks:	8		 */ \
/* Row address:    	15		 */ \
/* Column address: 	10		 */ \
/* Data bus width	64		 */ \
/*=============================================================================*/ \
	0x021b001c,	0x00008000,	/* MMDC0_MDSCR, set the Configuration request bit during MMDC set up */ \
/*=============================================================================*/ \
/* Calibration setup.                                                          */ \
/*=============================================================================*/ \
	0x021b0800,	0xA1390003,	/* DDR_PHY_P0_MPZQHWCTRL, enable both one-time & periodic HW ZQ calibration */ \
	0x021b080c,	0x004C0054,	\
	0x021b0810,	0x003F0047,	\
	0x021b480c,	0x00300032,	\
	0x021b4810,	0x002E003D,	\
/*Read DQS Gating calibration */ \
	0x021b083c,	0x02440244,	/* MPDGCTRL0 PHY0 */ \
	0x021b0840,	0x022C0234,	/* MPDGCTRL1 PHY0 */ \
	0x021b483c,	0x0228022C,	/* MPDGCTRL0 PHY1 */ \
	0x021b4840,	0x02140224,	/* MPDGCTRL1 PHY1 */ \
/*Read calibration */ \
	0x021b0848,	0x42464A4E,	/* MPRDDLCTL PHY0 */ \
	0x021b4848,	0x484A4E42,	/* MPRDDLCTL PHY1 */ \
/*Write calibration */ \
	0x021b0850,	0x38323030,	/* MPWRDLCTL PHY0 */ \
	0x021b4850,	0x3C34322C,	/* MPWRDLCTL PHY1 */ \
/*read data bit delay: (3 is the reccommended default value, although out of reset value is 0) */ \
	0x021b081c,	0x33333333,	/* DDR_PHY_P0_MPREDQBY0DL3 */ \
	0x021b0820,	0x33333333,	/* DDR_PHY_P0_MPREDQBY1DL3 */ \
	0x021b0824,	0x33333333,	/* DDR_PHY_P0_MPREDQBY2DL3 */ \
	0x021b0828,	0x33333333,	/* DDR_PHY_P0_MPREDQBY3DL3 */ \
	0x021b481c,	0x33333333,	/* DDR_PHY_P1_MPREDQBY0DL3 */ \
	0x021b4820,	0x33333333,	/* DDR_PHY_P1_MPREDQBY1DL3 */ \
	0x021b4824,	0x33333333,	/* DDR_PHY_P1_MPREDQBY2DL3 */ \
	0x021b4828,	0x33333333,	/* DDR_PHY_P1_MPREDQBY3DL3 */ \
/*For i.mx6qd parts of versions A & B (v1.0, v1.1), uncomment the following lines. For version C (v1.2), keep commented */ \
/*	0x021b08c0,	0x24911492,	// fine tune SDCLK duty cyc to low - seen to improve measured duty cycle of i.mx6 */ \
/*	0x021b48c0,	0x24911492,	 */ \
/* Complete calibration by forced measurement: */ \
	0x021b08b8,	0x00000800,	/* DDR_PHY_P0_MPMUR0, frc_msr */ \
	0x021b48b8,	0x00000800,	/* DDR_PHY_P0_MPMUR0, frc_msr */ \
/*=============================================================================*/ \
/* Calibration setup end                                                       */ \
/*=============================================================================*/ \
/*MMDC init: */ \
	0x021b0004,	0x0002002D,	/* MMDC0_MDPDC */ \
	0x021b0008,	0x1B333040,	/* MMDC0_MDOTC */ \
	0x021b000c,	0x676B52F3,	/* MMDC0_MDCFG0 */ \
	0x021b0010,	0xB66D8B63,	/* MMDC0_MDCFG1 */ \
	0x021b0014,	0x01FF00DB,	/* MMDC0_MDCFG2 */ \
/*MDMISC: RALAT kept to the high level of 5. */ \
/*MDMISC: consider reducing RALAT if your 528MHz board design allow that. Lower RALAT benefits: */ \
/*a. better operation at low frequency, for LPDDR2 freq < 100MHz, change RALAT to 3 */ \
/*b. Small performence improvment */ \
	0x021b0018,	0x00011740,	/* MMDC0_MDMISC */ \
	0x021b001c,	0x00008000,	/* MMDC0_MDSCR, set the Configuration request bit during MMDC set up */ \
	0x021b002c,	0x000026D2,	/* MMDC0_MDRWD */ \
	0x021b0030,	0x006B1023,	/* MMDC0_MDOR */ \
	0x021b0040,	0x00000047,	/* Chan0 CS0_END */ \
	0x021b0000,	0x841A0000,	/* MMDC0_MDCTL */ \
/*Mode register writes */ \
	0x021b001c,	0x02008032,	/* MMDC0_MDSCR, MR2 write, CS0 */ \
	0x021b001c,	0x00008033,	/* MMDC0_MDSCR, MR3 write, CS0 */ \
	0x021b001c,	0x00048031,	/* MMDC0_MDSCR, MR1 write, CS0 */ \
	0x021b001c,	0x15208030,	/* MMDC0_MDSCR, MR0write, CS0 */ \
	0x021b001c,	0x04008040,	/* MMDC0_MDSCR, ZQ calibration command sent to device on CS0 */ \
/*	0x021b001c,	0x0200803A,	// MMDC0_MDSCR, MR2 write, CS1 */ \
/*	0x021b001c,	0x0000803B,	// MMDC0_MDSCR, MR3 write, CS1 */ \
/*	0x021b001c,	0x00048039,	// MMDC0_MDSCR, MR1 write, CS1 */ \
/*	0x021b001c,	0x15208038,	// MMDC0_MDSCR, MR0write, CS1 */ \
/*	0x021b001c,	0x04008048,	// MMDC0_MDSCR, ZQ calibration command sent to device on CS1 */ \
	0x021b0020,	0x00007800,	/* MMDC0_MDREF */ \
	0x021b0818,	0x00022227,	/* DDR_PHY_P0_MPODTCTRL */ \
	0x021b4818,	0x00022227,	/* DDR_PHY_P1_MPODTCTRL */ \
	0x021b0004,	0x0002556D,	/* MMDC0_MDPDC now SDCTL power down enabled */ \
	0x021b0404,	0x00011006,	/* MMDC0_MAPSR ADOPT power down enabled, MMDC will enter automatically to self-refresh while the number of idle cycle reached. */ \
	0x021b001c,	0x00000000,	/* MMDC0_MDSCR, clear this register (especially the configuration bit as initialization is complete) */ \
}
