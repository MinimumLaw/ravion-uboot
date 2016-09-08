/*
 * Copyright (c) 2010-2012, NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <common.h>
#include <asm/arch/clock.h>
#include <asm/arch/pmu.h>
#include <asm/arch/tegra.h>
#include <asm/arch-tegra/clk_rst.h>
#include <asm/arch-tegra/pmc.h>
#include <asm/arch-tegra/tegra_i2c.h>
#include <asm/io.h>
#include "../cpu.h"

#define I2C_SEND_2_BYTES		0x0a02
#define TPS6586X_I2C_ADDR		(0x34<<1)
#define TPS6586X_VCC1_REG		0x20
#define TPS6586X_SM1V1_REG		0x23
#define TPS6586X_SM0V1_REG		0x26

/* Tegra20-specific DVC init code */
void tegra_i2c_ll_init(void)
{
	struct dvc_ctlr *reg = (struct dvc_ctlr *)TEGRA_DVC_BASE;

	writel(DVC_CTRL_REG3_I2C_HW_SW_PROG_MASK, &reg->ctrl3);
}

void tegra_i2c_ll_write(uint offset, uint8_t data)
{
	struct dvc_ctlr *reg = (struct dvc_ctlr *)TEGRA_DVC_BASE;

	writel(TPS6586X_I2C_ADDR, &reg->cmd_addr0);
	writel(2, &reg->cnfg);

	writel((data << 8) | (offset & 0xff), &reg->cmd_data1);
	writel(I2C_SEND_2_BYTES, &reg->cnfg);
}

static void enable_cpu_power_rail(void)
{
	struct pmc_ctlr *pmc = (struct pmc_ctlr *)NV_PA_PMC_BASE;
	u32 reg;

	debug("enable_cpu_power_rail entry\n");

#ifdef CONFIG_TEGRA_EARLY_TPS6586X
	tegra_i2c_ll_init();

	/* Set SM0 being VDD_CORE_1.2V to 1.2 volts */
	tegra_i2c_ll_write(TPS6586X_SM0V1_REG, 0x13);

	udelay(1000);

	/* Set SM1 being VDD_CPU_1.0V to 1.0 volts */
	tegra_i2c_ll_write(TPS6586X_SM1V1_REG, 0x0b);

	udelay(1000);

	/* Make sure primary voltages are selected and ramped towards */
	tegra_i2c_ll_write(TPS6586X_VCC1_REG, 0x05);

	udelay(10 * 1000);
#endif

	reg = readl(&pmc->pmc_cntrl);
	reg |= CPUPWRREQ_OE;
	writel(reg, &pmc->pmc_cntrl);

	/*
	 * The TI PMU65861C needs a 3.75ms delay between enabling
	 * the power rail and enabling the CPU clock.  This delay
	 * between SM1EN and SM1 is for switching time + the ramp
	 * up of the voltage to the CPU (VDD_CPU from PMU).
	 */
	udelay(3750);
}

/* T20 requires some special clock initialization, incl. setting up DVC I2C */
void t20_init_clocks(void)
{
	debug("t20_init_clocks entry\n");

	/* Put i2c in reset and enable clocks */
	reset_set_enable(PERIPH_ID_DVC_I2C, 1);
	clock_set_enable(PERIPH_ID_DVC_I2C, 1);

	/*
	 * Our high-level clock routines are not available prior to
	 * relocation. We use the low-level functions which require a
	 * hard-coded divisor. Use CLK_M with divide by (n + 1 = 16)
	 */
	clock_ll_set_source_divisor(PERIPH_ID_DVC_I2C, 3, 15);

	/* Give clocks time to stabilize, then take i2c out of reset */
	udelay(1000);

	reset_set_enable(PERIPH_ID_DVC_I2C, 0);
}

void start_cpu(u32 reset_vector)
{
	debug("%s entry, reset_vector = %x\n", __func__, reset_vector);

	t20_init_clocks();

	/* Enable VDD_CPU */
	enable_cpu_power_rail();

	/* Hold the CPUs in reset */
	reset_A9_cpu(1);

	/* Disable the CPU clock */
	enable_cpu_clock(0);

	/* Enable CoreSight */
	clock_enable_coresight(1);

	/*
	 * Set the entry point for CPU execution from reset,
	 *  if it's a non-zero value.
	 */
	if (reset_vector)
		writel(reset_vector, EXCEP_VECTOR_CPU_RESET_VECTOR);

	/* Enable the CPU clock */
	enable_cpu_clock(1);

	/* If the CPU doesn't already have power, power it up */
	powerup_cpu();

	/* Take the CPU out of reset */
	reset_A9_cpu(0);
}
