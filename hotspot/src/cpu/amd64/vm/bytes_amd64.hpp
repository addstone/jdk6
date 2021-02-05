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

class Bytes: AllStatic 
{
 public:
  // Returns true if the byte ordering used by Java is different from
  // the native byte ordering of the underlying machine. For example,
  // this is true for Intel x86, but false for Solaris on Sparc.
  static inline bool is_Java_byte_ordering_different()
  {
    return true;
  }

  // Efficient reading and writing of unaligned unsigned data in
  // platform-specific byte ordering (no special code is needed since
  // amd64 CPUs can access unaligned data)
  static inline u2 get_native_u2(address p)
  {
    return *(u2*) p;
  }
  static inline u4 get_native_u4(address p)
  {
    return *(u4*) p;
  }
  static inline u8 get_native_u8(address p)
  {
    return *(u8*) p;
  }

  static inline void put_native_u2(address p, u2 x)
  {
    *(u2*) p = x; 
  }
  static inline void put_native_u4(address p, u4 x)
  {
    *(u4*) p = x;
  }
  static inline void put_native_u8(address p, u8 x)
  {
    *(u8*) p = x;
  }

  // Efficient reading and writing of unaligned unsigned data in Java
  // byte ordering (i.e. big-endian ordering). Byte-order reversal is
  // needed since amd64 CPUs use little-endian format.
  static inline u2 get_Java_u2(address p)
  { 
    return swap_u2(get_native_u2(p)); 
  }
  static inline u4 get_Java_u4(address p)
  { 
    return swap_u4(get_native_u4(p)); 
  }
  static inline u8 get_Java_u8(address p)
  {
    return swap_u8(get_native_u8(p));
  }

  static inline void put_Java_u2(address p, u2 x)
  { 
    put_native_u2(p, swap_u2(x)); 
  }
  static inline void put_Java_u4(address p, u4 x)
  {
    put_native_u4(p, swap_u4(x)); 
  }
  static inline void put_Java_u8(address p, u8 x)
  {
    put_native_u8(p, swap_u8(x)); 
  }

  // Efficient swapping of byte ordering
  static inline u2 swap_u2(u2 x);
  static inline u4 swap_u4(u4 x);
  static inline u8 swap_u8(u8 x);
};


// The following header contains the implementations of swap_u2,
// swap_u4, and swap_u8
#include "incls/_bytes_pd.inline.hpp.incl"
