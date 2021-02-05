#ifdef USE_PRAGMA_IDENT_HDR
#pragma ident "%W% %E% %U% JVM"
#endif
/*
 * Copyright 2003 Sun Microsystems, Inc.  All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
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
 *  
 */


// Efficient swapping of data bytes from Java byte
// ordering to native byte ordering and vice versa.
inline u2 Bytes::swap_u2(u2 x) {
  address p = (address) &x;
  return  ( (u2(p[0]) << 8 ) | ( u2(p[1])) );
}

inline u4 Bytes::swap_u4(u4 x) {
  address p = (address) &x;
  return ( (u4(p[0]) << 24) | (u4(p[1]) << 16) | (u4(p[2]) << 8) | u4(p[3])) ;
}

// Helper function for swap_u8
inline u8 Bytes::swap_u8(u8 x) {
  address p = (address) &x;
  return ( (u8(p[0]) << 56) | (u8(p[1]) << 48) | (u8(p[2]) << 40) | (u8(p[3]) << 32) |
           (u8(p[4]) << 24) | (u8(p[5]) << 16) | (u8(p[6]) << 8)  | u8(p[7])) ;
}
