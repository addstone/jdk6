#ifdef USE_PRAGMA_IDENT_HDR
#pragma ident "%W% %E% %U% JVM"
#endif
/*
 * Copyright 2002-2006 Sun Microsystems, Inc.  All Rights Reserved.
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

inline PSPromotionManager* PSPromotionManager::manager_array(int index) {
  assert(_manager_array != NULL, "access of NULL manager_array");
  assert(index >= 0 && index <= (int)ParallelGCThreads, "out of range manager_array access");
  return _manager_array[index];
}

inline void PSPromotionManager::claim_or_forward_internal_depth(oop* p) {
  if (p != NULL) {
    oop o = *p;
    if (o->is_forwarded()) {
      o = o->forwardee();

      // Card mark
      if (PSScavenge::is_obj_in_young((HeapWord*) o)) {
	PSScavenge::card_table()->inline_write_ref_field_gc(p, o);
      }
      *p = o;
    } else {
      if (!claimed_stack_depth()->push(p)) {
	overflow_stack_depth()->push(p);
      }
    }
  }
}

inline void PSPromotionManager::claim_or_forward_internal_breadth(oop* p) {
  if (p != NULL) {
    oop o = *p;
    if (o->is_forwarded()) {
      o = o->forwardee();
    } else {
      o = copy_to_survivor_space(o, false);
    }
  
    // Card mark
    if (PSScavenge::is_obj_in_young((HeapWord*) o)) {
      PSScavenge::card_table()->inline_write_ref_field_gc(p, o);
    }
    *p = o;
  }
}

inline void PSPromotionManager::flush_prefetch_queue() {
  assert(!depth_first(), "invariant");
  for (int i=0; i<_prefetch_queue.length(); i++) {
    claim_or_forward_internal_breadth(_prefetch_queue.pop());
  }
}

inline void PSPromotionManager::claim_or_forward_depth(oop* p) {
  assert(depth_first(), "invariant");
  assert(PSScavenge::should_scavenge(*p, true), "revisiting object?");
  assert(Universe::heap()->kind() == CollectedHeap::ParallelScavengeHeap, "Sanity");
  assert(Universe::heap()->is_in(p), "pointer outside heap");

  claim_or_forward_internal_depth(p);
}

inline void PSPromotionManager::claim_or_forward_breadth(oop* p) {
  assert(!depth_first(), "invariant");
  assert(PSScavenge::should_scavenge(*p, true), "revisiting object?");
  assert(Universe::heap()->kind() == CollectedHeap::ParallelScavengeHeap, "Sanity");
  assert(Universe::heap()->is_in(p), "pointer outside heap");

  if (UsePrefetchQueue) {
    claim_or_forward_internal_breadth(_prefetch_queue.push_and_pop(p));
  } else {
    // This option is used for testing.  The use of the prefetch
    // queue can delay the processing of the objects and thus
    // change the order of object scans.  For example, remembered
    // set updates are typically the clearing of the remembered
    // set (the cards) followed by updates of the remembered set
    // for young-to-old pointers.  In a situation where there
    // is an error in the sequence of clearing and updating
    // (e.g. clear card A, update card A, erroneously clear
    // card A again) the error can be obscured by a delay
    // in the update due to the use of the prefetch queue
    // (e.g., clear card A, erroneously clear card A again,
    // update card A that was pushed into the prefetch queue
    // and thus delayed until after the erronous clear).  The
    // length of the delay is random depending on the objects
    // in the queue and the delay can be zero.
    claim_or_forward_internal_breadth(p);
  }
}