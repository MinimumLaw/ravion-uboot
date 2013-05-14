/*
 * (C) Copyright 2012-2013
 * Texas Instruments, <www.ti.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <config.h>
#include <palmas.h>

void palmas_init_settings(void)
{
	return;
}

int palmas_mmc1_poweron_ldo(void)
{
	u8 val = 0;

	/* set LDO9 TWL6035 to 3V */
	val = 0x2b; /* (3 -.9)*28 +1 */

	if (palmas_i2c_write_u8(0x48, LDO9_VOLTAGE, val)) {
		printf("twl6035: could not set LDO9 voltage.\n");
		return 1;
	}

	/* TURN ON LDO9 */
	val = LDO_ON | LDO_MODE_SLEEP | LDO_MODE_ACTIVE;

	if (palmas_i2c_write_u8(0x48, LDO9_CTRL, val)) {
		printf("twl6035: could not turn on LDO9.\n");
		return 1;
	}

	return 0;
}
