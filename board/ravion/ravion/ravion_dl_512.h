#define RAVION_V2_512MB_DEFAULTS { \
//DDR IO TYPE:			
DATA 4, 0x020e0774, 0x000C0000	// IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE 
DATA 4, 0x020e0754, 0x00000000	// IOMUXC_SW_PAD_CTL_GRP_DDRPKE 
			
//CLOCK:			
DATA 4, 0x020e04ac, 0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDCLK_0
DATA 4, 0x020e04b0, 0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDCLK_1
			
//ADDRESS:			
DATA 4, 0x020e0464, 0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_CAS
DATA 4, 0x020e0490, 0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_RAS
DATA 4, 0x020e074c, 0x00000028	// IOMUXC_SW_PAD_CTL_GRP_ADDDS 
			
//Control:			
DATA 4, 0x020e0494, 0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_RESET
DATA 4, 0x020e04a0, 0x00000000	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDBA2 - DSE can be configured using Group Control Register: IOMUXC_SW_PAD_CTL_GRP_CTLDS
DATA 4, 0x020e04b4, 0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDODT0
DATA 4, 0x020e04b8, 0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDODT1
DATA 4, 0x020e076c, 0x00000028	// IOMUXC_SW_PAD_CTL_GRP_CTLDS 
			
//Data Strobes:			
DATA 4, 0x020e0750, 0x00020000	// IOMUXC_SW_PAD_CTL_GRP_DDRMODE_CTL 
DATA 4, 0x020e04bc, 0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS0 
DATA 4, 0x020e04c0, 0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS1 
DATA 4, 0x020e04c4, 0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS2 
DATA 4, 0x020e04c8, 0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS3 
DATA 4, 0x020e04cc, 0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS4 
DATA 4, 0x020e04d0, 0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS5 
DATA 4, 0x020e04d4, 0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS6 
DATA 4, 0x020e04d8, 0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS7 
			
//Data:			
DATA 4, 0x020e0760, 0x00020000	// IOMUXC_SW_PAD_CTL_GRP_DDRMODE
DATA 4, 0x020e0764, 0x00000028	// IOMUXC_SW_PAD_CTL_GRP_B0DS 
DATA 4, 0x020e0770, 0x00000028	// IOMUXC_SW_PAD_CTL_GRP_B1DS 
DATA 4, 0x020e0778, 0x00000028	// IOMUXC_SW_PAD_CTL_GRP_B2DS 
DATA 4, 0x020e077c, 0x00000028	// IOMUXC_SW_PAD_CTL_GRP_B3DS 
DATA 4, 0x020e0780, 0x00000028	// IOMUXC_SW_PAD_CTL_GRP_B4DS 
DATA 4, 0x020e0784, 0x00000028	// IOMUXC_SW_PAD_CTL_GRP_B5DS 
DATA 4, 0x020e078c, 0x00000028	// IOMUXC_SW_PAD_CTL_GRP_B6DS 
DATA 4, 0x020e0748, 0x00000028	// IOMUXC_SW_PAD_CTL_GRP_B7DS 
			
DATA 4, 0x020e0470, 0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM0
DATA 4, 0x020e0474, 0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM1
DATA 4, 0x020e0478, 0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM2
DATA 4, 0x020e047c, 0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM3
DATA 4, 0x020e0480, 0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM4
DATA 4, 0x020e0484, 0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM5
DATA 4, 0x020e0488, 0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM6
DATA 4, 0x020e048c, 0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM7
			
			
//=============================================================================			
// DDR Controller Registers			
//=============================================================================			
// Manufacturer:	Micron		
// Device Part Number:	MT41K64M16TW-107		
// Clock Freq.: 	400MHz		
// Density per CS in Gb: 	4		
// Chip Selects used:	1		
// Number of Banks:	8		
// Row address:    	13		
// Column address: 	10		
// Data bus width	64		
//=============================================================================			
DATA 4, 0x021b0800, 0xa1390003 	// DDR_PHY_P0_MPZQHWCTRL, enable both one-time & periodic HW ZQ calibration.

//DATA 4, 0x021b001c, 0x00008000	// MMDC0_MDSCR, set the Configuration request bit during MMDC set up
			
//=============================================================================			
// Calibration setup.			
//=============================================================================			
// write leveling, based on Freescale board layout and T topology			
// For target board, may need to run write leveling calibration 			
// to fine tune these settings			
// If target board does not use T topology, then these registers			
// should either be cleared or write leveling calibration can be run			
DATA 4, 0x021b080c, 0x00510056
DATA 4, 0x021b0810, 0x0046004C
DATA 4, 0x021b480c, 0x0027002F
DATA 4, 0x021b4810, 0x002A003A
			
//######################################################			
//calibration values based on calibration compare of 0x00ffff00:			
//Note, these calibration values are based on Freescale's board			
//May need to run calibration on target board to fine tune these 			
//######################################################			
			
//Read DQS Gating calibration			
DATA 4, 0x021b083c, 0x423C0238	// MPDGCTRL0 PHY0
DATA 4, 0x021b0840, 0x0220022C	// MPDGCTRL1 PHY0
DATA 4, 0x021b483c, 0x42180218	// MPDGCTRL0 PHY1
DATA 4, 0x021b4840, 0x02040214	// MPDGCTRL1 PHY1
			
//Read calibration			
DATA 4, 0x021b0848, 0x42464C4C	// MPRDDLCTL PHY0
DATA 4, 0x021b4848, 0x4046483E	// MPRDDLCTL PHY1
			
//Write calibration			
DATA 4, 0x021b0850, 0x342E2A2A	// MPWRDLCTL PHY0
DATA 4, 0x021b4850, 0x30302C26	// MPWRDLCTL PHY1

//DATA 4, 0x021b0800, 0xA1390003	// DDR_PHY_P0_MPZQHWCTRL, enable both one-time & periodic HW ZQ calibration.
			
// For target board, may need to run write leveling calibration to fine tune these settings.			
//DATA 4, 0x021b080c, 0x00000000	
//DATA 4, 0x021b0810, 0x00000000	
//DATA 4, 0x021b480c, 0x00000000	
//DATA 4, 0x021b4810, 0x00000000	
			
////Read DQS Gating calibration			
//DATA 4, 0x021b083c, 0x00000000	// MPDGCTRL0 PHY0
//DATA 4, 0x021b0840, 0x00000000	// MPDGCTRL1 PHY0
//DATA 4, 0x021b483c, 0x00000000	// MPDGCTRL0 PHY1
//DATA 4, 0x021b4840, 0x00000000	// MPDGCTRL1 PHY1
			
//Read calibration			
//DATA 4, 0x021b0848, 0x40404040	// MPRDDLCTL PHY0
//DATA 4, 0x021b4848, 0x40404040	// MPRDDLCTL PHY1
			
//Write calibration                     			
//DATA 4, 0x021b0850, 0x40404040	// MPWRDLCTL PHY0
//DATA 4, 0x021b4850, 0x40404040	// MPWRDLCTL PHY1
			
//read data bit delay: (3 is the reccommended default value, although out of reset value is 0)			
DATA 4, 0x021b081c, 0x33333333	// DDR_PHY_P0_MPREDQBY0DL3
DATA 4, 0x021b0820, 0x33333333	// DDR_PHY_P0_MPREDQBY1DL3
DATA 4, 0x021b0824, 0x33333333	// DDR_PHY_P0_MPREDQBY2DL3
DATA 4, 0x021b0828, 0x33333333	// DDR_PHY_P0_MPREDQBY3DL3
DATA 4, 0x021b481c, 0x33333333	// DDR_PHY_P1_MPREDQBY0DL3
DATA 4, 0x021b4820, 0x33333333	// DDR_PHY_P1_MPREDQBY1DL3
DATA 4, 0x021b4824, 0x33333333	// DDR_PHY_P1_MPREDQBY2DL3
DATA 4, 0x021b4828, 0x33333333	// DDR_PHY_P1_MPREDQBY3DL3
			
//For i.mx6qd parts of versions A & B (v1.0, v1.1), uncomment the following lines. For version C (v1.2), keep commented			
//DATA 4, 0x021b08c0, 0x24911492	// fine tune SDCLK duty cyc to low - seen to improve measured duty cycle of i.mx6
//DATA 4, 0x021b48c0, 0x24911492	
			
// Complete calibration by forced measurement:                  			
DATA 4, 0x021b08b8, 0x00000800	// DDR_PHY_P0_MPMUR0, frc_msr
DATA 4, 0x021b48b8, 0x00000800	// DDR_PHY_P0_MPMUR0, frc_msr
//=============================================================================			
// Calibration setup end			
//=============================================================================			
			
//MMDC init: 			
DATA 4, 0x021b0004, 0x0002002D	// MMDC0_MDPDC
DATA 4, 0x021b0008, 0x1B333040	// MMDC0_MDOTC
DATA 4, 0x021b000c, 0x2B2F52F3	// MMDC0_MDCFG0
DATA 4, 0x021b0010, 0xB66D8B63	// MMDC0_MDCFG1
DATA 4, 0x021b0014, 0x01FF00DB	// MMDC0_MDCFG2
			
//MDMISC: RALAT kept to the high level of 5. 			
//MDMISC: consider reducing RALAT if your 528MHz board design allow that. Lower RALAT benefits: 			
//a. better operation at low frequency, for LPDDR2 freq < 100MHz, change RALAT to 3			
//b. Small performence improvment 			
DATA 4, 0x021b0018, 0x00011740	// MMDC0_MDMISC
DATA 4, 0x021b001c, 0x00008000	// MMDC0_MDSCR, set the Configuration request bit during MMDC set up
DATA 4, 0x021b002c, 0x000026D2	// MMDC0_MDRWD
DATA 4, 0x021b0030, 0x002F1023	// MMDC0_MDOR
DATA 4, 0x021b0040, 0x00000017	// Chan0 CS0_END 
DATA 4, 0x021b0000, 0x821A0000	// MMDC0_MDCTL
			
//Mode register writes                 			
DATA 4, 0x021b001c, 0x02008032	// MMDC0_MDSCR, MR2 write, CS0
DATA 4, 0x021b001c, 0x00008033	// MMDC0_MDSCR, MR3 write, CS0
DATA 4, 0x021b001c, 0x00048031	// MMDC0_MDSCR, MR1 write, CS0
DATA 4, 0x021b001c, 0x15208030	// MMDC0_MDSCR, MR0write, CS0
DATA 4, 0x021b001c, 0x04008040	// MMDC0_MDSCR, ZQ calibration command sent to device on CS0
			
//DATA 4, 0x021b001c, 0x0200803A	// MMDC0_MDSCR, MR2 write, CS1
//DATA 4, 0x021b001c, 0x0000803B	// MMDC0_MDSCR, MR3 write, CS1
//DATA 4, 0x021b001c, 0x00048039	// MMDC0_MDSCR, MR1 write, CS1
//DATA 4, 0x021b001c, 0x15208038	// MMDC0_MDSCR, MR0write, CS1
//DATA 4, 0x021b001c, 0x04008048	// MMDC0_MDSCR, ZQ calibration command sent to device on CS1
			
DATA 4, 0x021b0020, 0x00007800	// MMDC0_MDREF
			
DATA 4, 0x021b0818, 0x00022227	// DDR_PHY_P0_MPODTCTRL
DATA 4, 0x021b4818, 0x00022227	// DDR_PHY_P1_MPODTCTRL
			
DATA 4, 0x021b0004, 0x0002556D	// MMDC0_MDPDC now SDCTL power down enabled
			
DATA 4, 0x021b0404, 0x00011006	// MMDC0_MAPSR ADOPT power down enabled, MMDC will enter automatically to self-refresh while the number of idle cycle reached.
			
DATA 4, 0x021b001c, 0x00000000	// MMDC0_MDSCR, clear this register (especially the configuration bit as initialization is complete)