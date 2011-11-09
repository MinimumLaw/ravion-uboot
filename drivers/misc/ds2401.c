/*
 * Copyright 2005-2010
 * Ilko Iliev <iliev@ronetix.at>
 * Ronetix Development Tools GmbH <www.ronetix.at>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * Driver for 1-wire, DS2401, a Silicon Serial Number chip
 */

#include <common.h>
#include <command.h>
#include <ds2401.h>

static DS2401_FUNCS *DS2401_Funcs;

/*
 * Generate a 1-wire reset, return 1 if no presence detect was found,
 * return 0 otherwise.
 */
static int ds2401_reset(void)
{
	int result;

	DS2401_Funcs->set(0);
	udelay(480);
	DS2401_Funcs->set(1);
	udelay(70);

	result = DS2401_Funcs->get();

	udelay(410);
	return result;
}

/*
 * Send 1 a 1-wire write bit.
 * Provide 10us recovery time.
 */
static void ds2401_write_bit(int bit)
{
	if (bit) {
		/*
		 * write '1' bit
		 */
		 DS2401_Funcs->set(0);
		udelay(6);
		DS2401_Funcs->set(1);
		udelay(64);
	} else {
		/*
		 * write '0' bit
		 */
		DS2401_Funcs->set(0);
		udelay(60);
		DS2401_Funcs->set(1);
		udelay(10);
	}
}

/*
 * Read a bit from the 1-wire bus and return it.
 * Provide 10us recovery time.
 */
static int ds2401_read_bit(void)
{
	int result;

	DS2401_Funcs->set(0);

#ifdef __OPTIMIZE__
	udelay(1);
#endif

	DS2401_Funcs->set(1);
	udelay(9);

	result = DS2401_Funcs->get();
	udelay(55);

	return result;
}

static void ds2401_write_byte(int data)
{
	int loop;

	for (loop = 0; loop < 8; loop++) {
		ds2401_write_bit(data & 0x01);
		data >>= 1;
	}
}

static int ds2401_read_byte(void)
{
	int loop, result = 0;

	for (loop = 0; loop < 8; loop++) {
		result >>= 1;
		if (ds2401_read_bit())
			result |= 0x80;
	}

	return result;
}

static unsigned char ds2401_crc8_table[] = {
	0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
	157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
	35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
	190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
	70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
	219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
	101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
	248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
	140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
	17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
	175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238,
	50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
	202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,
	87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
	233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
	116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53
};

static unsigned char ds2401_calc_crc8(unsigned char * data, int len)
{
	unsigned char crc = 0;

	while (len--)
		crc = ds2401_crc8_table[crc ^ *data++];

	return crc;
}

void ds2401_init(DS2401_FUNCS *funcs)
{
	DS2401_Funcs = funcs;
}

/*
 * Read the serial number from the 1-wire bus and return it.
 * Returns 0 if successful.
 * Returns non-zero if not successful.
 */
int ds2401_get_number( unsigned char id[6] )
{
	int ii, result;
	unsigned char crc;
	unsigned char buf[8];

	result = ds2401_reset();
	if (result != 0)
		return 1;

	ds2401_write_byte(DS2401_CMD_READ_ROM);	/* send read rom command */

	/*
	 * read 8 bytes:
	 * buf[0]	- family code (0x01)
	 * buf[1..6]	- serial number
	 * buf[7]	- CRC
	 */
	for (ii = 0; ii < 8; ii++)
		buf[ii] = ds2401_read_byte();

	/*
	 * calculate the 8-bit Dallas CRC
	 */
	crc = ds2401_calc_crc8( buf, 7 );

	if ( crc != buf[7] )
		return 1;

	for (ii = 0; ii < 6; ii++)
		id[ii] = buf[ii + 1];

	return 0;
}

int do_ds2401(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int stat;
	unsigned char ds2401_id[6];
	char str[32];

	stat = ds2401_get_number( ds2401_id );
	if (stat){
		puts("No 1-wire device detected!\n");
		return 0;
	}

	sprintf(str, "%s:%X:%X:%X",
		CONFIG_MAC_OUI,
		(int)ds2401_id[2],
		(int)ds2401_id[1],
		(int)ds2401_id[0]);

	printf("Setting environment variable 'ethaddr' to %s\n", str);
	setenv("ethaddr", str);

	return 0;
}

U_BOOT_CMD(
	onewire, 1, 1, do_ds2401,
	"Read 1-write ID and set 'ethaddr'",
	NULL
);

