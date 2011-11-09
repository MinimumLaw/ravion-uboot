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

#ifndef __DS2401_H_
#define __DS2401_H_

#define DS2401_CMD_READ_ROM 0x33

typedef struct
{
	void (*set)(int value );
	int (*get)(void);
} DS2401_FUNCS;

extern void ds2401_init(DS2401_FUNCS *funcs);
extern int ds2401_get_number( unsigned char id[6] );

#endif /* __DS2401_H_ */

