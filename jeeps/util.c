/*
    Misc utilities. From GPSBABEL

    Copyright (C) 2002 Robert Lipe, robertlipe@usa.net

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111 USA

 */

//#include "defs.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

#include <time.h>
#include <math.h>
#include <string.h>
#include <stddef.h>


static int i_am_little_endian = -1;
static int doswap(void);


void
fatal(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	exit(1);
}

void
warning(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
}

/*
 * Read 4 bytes in big-endian.   Return as "int" in native endianness.
 */
signed int
be_read32(void *p)
{
	unsigned char *i = (unsigned char *) p;
	return i[0] << 24 | i[1] << 16  | i[2] << 8 | i[3];
}

signed int
be_read16(void *p)
{
	unsigned char *i = (unsigned char *) p;
	return i[0] << 8 | i[1];
}

void
be_write16(void *addr, unsigned value)
{
	unsigned char *p = addr;
	p[0] = value >> 8;
	p[1] = value;
	
}

void
be_write32(void *pp, unsigned i)
{
	char *p = (char *)pp;

	p[0] = (i >> 24) & 0xff;
	p[1] = (i >> 16) & 0xff;
	p[2] = (i >> 8) & 0xff;
	p[3] = i & 0xff;
}

signed int
le_read16(void *addr)
{
	unsigned char *p = addr;
	return p[0] | (p[1] << 8);
}

signed int
le_read32(void *addr)
{
	unsigned char *p = addr;
	return p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
}

/*
 *  Read a little-endian 64-bit value from 'src' and return it in 'dest' 
 *  in host endianness.
 */
void
le_read64(void *dest, const void *src)
{
	char *cdest = dest;
	const char *csrc = src;

	doswap(); /* make sure i_am_little_endian is initialized */

	if (i_am_little_endian) {
		memcpy(dest, src, 8);
	} else {
		int i;
		for (i = 0; i < 8; i++) {
			cdest[i] = csrc[7-i];
		}
	}
}

void
le_write16(void *addr, unsigned value)
{
	unsigned char *p = addr;
	p[0] = value;
	p[1] = value >> 8;
	
}

void 
le_write32(void *addr, unsigned value)
{
	unsigned char *p = addr;
	p[0] = value;
	p[1] = value >> 8;
	p[2] = value >> 16;
	p[3] = value >> 24;
}
static int doswap()
{
  if (i_am_little_endian < 0)
  {
        /*      On Intel, Vax and MIPs little endian, -1.0 maps to the bytes
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x3f and on Motorola,
                SPARC, ARM, and PowerPC, it maps to
                0x3f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00.
        */
        double d = 1.0;
        char c[8];
        memcpy(c, &d, 8);
        i_am_little_endian = (c[0] == 0);
  }
  return i_am_little_endian;
}
