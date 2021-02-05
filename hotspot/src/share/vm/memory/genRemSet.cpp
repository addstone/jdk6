#ifdef USE_PRAGMA_IDENT_SRC
#pragma ident "@(#)genRemSet.cpp	1.11 07/05/05 17:05:50 JVM"
#endif
/*
 * Copyright 2001-2002 Sun Microsystems, Inc.  All Rights Reserved.
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

// This kind of "BarrierSet" allows a "CollectedHeap" to detect and
// enumerate ref fields that have been modified (since the last
// enumeration.)

# include "incls/_precompiled.incl"
# include "incls/_genRemSet.cpp.incl"

uintx GenRemSet::max_alignment_constraint(Name nm) {
  switch (nm) {
  case GenRemSet::CardTable:
    return CardTableRS::ct_max_alignment_constraint();
  default:
    guarantee(false, "Unrecognized GenRemSet type.");
    return (0); // Make Windows compiler happy
  }
}
