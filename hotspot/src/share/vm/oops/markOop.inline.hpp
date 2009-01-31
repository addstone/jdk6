#ifdef USE_PRAGMA_IDENT_HDR
#pragma ident "%W% %E% %U% JVM"
#endif
/*
 * Copyright 2006 Sun Microsystems, Inc.  All Rights Reserved.
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

// Should this header be preserved during GC?
inline bool markOopDesc::must_be_preserved_with_bias(oop obj_containing_mark) const {
  assert(UseBiasedLocking, "unexpected");
  if (has_bias_pattern()) {
    // Will reset bias at end of collection
    // Mark words of biased and currently locked objects are preserved separately
    return false;
  }
  markOop prototype_header = prototype_for_object(obj_containing_mark);
  if (prototype_header->has_bias_pattern()) {
    // Individual instance which has its bias revoked; must return
    // true for correctness
    return true;
  }
  return (!is_unlocked() || !has_no_hash());
}

// Should this header (including its age bits) be preserved in the
// case of a promotion failure during scavenge?
inline bool markOopDesc::must_be_preserved_with_bias_for_promotion_failure(oop obj_containing_mark) const {
  assert(UseBiasedLocking, "unexpected");
  // We don't explicitly save off the mark words of biased and
  // currently-locked objects during scavenges, so if during a
  // promotion failure we encounter either a biased mark word or a
  // klass which still has a biasable prototype header, we have to
  // preserve the mark word. This results in oversaving, but promotion
  // failures are rare, and this avoids adding more complex logic to
  // the scavengers to call new variants of
  // BiasedLocking::preserve_marks() / restore_marks() in the middle
  // of a scavenge when a promotion failure has first been detected.
  if (has_bias_pattern() ||
      prototype_for_object(obj_containing_mark)->has_bias_pattern()) {
    return true;
  }
  return (this != prototype());
}

// Should this header (including its age bits) be preserved in the
// case of a scavenge in which CMS is the old generation?
inline bool markOopDesc::must_be_preserved_with_bias_for_cms_scavenge(klassOop klass_of_obj_containing_mark) const {
  assert(UseBiasedLocking, "unexpected");
  // CMS scavenges preserve mark words in similar fashion to promotion failures; see above
  if (has_bias_pattern() ||
      klass_of_obj_containing_mark->klass_part()->prototype_header()->has_bias_pattern()) {
    return true;
  }
  return (this != prototype());
}

inline markOop markOopDesc::prototype_for_object(oop obj) {
#ifdef ASSERT
  markOop prototype_header = obj->klass()->klass_part()->prototype_header();
  assert(prototype_header == prototype() || prototype_header->has_bias_pattern(), "corrupt prototype header");
#endif
  return obj->klass()->klass_part()->prototype_header();
}