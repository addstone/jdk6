#ifdef USE_PRAGMA_IDENT_HDR
#pragma ident "%W% %E% %U% JVM"
#endif
/*
 * Copyright 1997-2004 Sun Microsystems, Inc.  All Rights Reserved.
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

// Interface for updating the instruction cache.  Whenever the VM modifies
// code, part of the processor instruction cache potentially has to be flushed.

// On the x86, this is a no-op -- the I-cache is guaranteed to be consistent
// after the next jump, and the VM never modifies instructions directly ahead
// of the instruction fetch path.

// [phh] It's not clear that the above comment is correct, because on an MP
// system where the dcaches are not snooped, only the thread doing the invalidate
// will see the update.  Even in the snooped case, a memory fence would be
// necessary if stores weren't ordered.  Fortunately, they are on all known
// x86 implementations.

class ICache : public AbstractICache {
 public:
  static void initialize() {}
  static void invalidate_word (address addr) {} // for writes up to 4 bytes
  static void invalidate_range(address start, int nbytes) {}
};