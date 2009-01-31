/*
 * Copyright 2000 Sun Microsystems, Inc.  All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.  Sun designates this
 * particular file as subject to the "Classpath" exception as provided
 * by Sun in the LICENSE file that accompanied this code.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa Clara,
 * CA 95054 USA or visit www.sun.com if you need additional information or
 * have any questions.
 */

#include "AlphaMath.h"

unsigned char mul8table[256][256];
unsigned char div8table[256][256];

void initAlphaTables()
{
    unsigned int i;
    unsigned int j;

    for (i = 1; i < 256; i++) {			/* SCALE == (1 << 24) */
	int inc = (i << 16) + (i<<8) + i;	/* approx. SCALE * (i/255.0) */
	int val = inc + (1 << 23);		/* inc + SCALE*0.5 */
        for (j = 1; j < 256; j++) {
            mul8table[i][j] = (val >> 24);	/* val / SCALE */
	    val += inc;
        }
    }

    for (i = 1; i < 256; i++) {
	unsigned int inc;
	unsigned int val;
	inc = 0xff;
	inc = ((inc << 24) + i/2) / i;
	val = (1 << 23);
	for (j = 0; j < i; j++) {
	    div8table[i][j] = (val >> 24);
	    val += inc;
	}
	for (j = i; j < 256; j++) {
	    div8table[i][j] = 255;
	}
    }
}