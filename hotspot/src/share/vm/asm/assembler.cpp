#ifdef USE_PRAGMA_IDENT_SRC
#pragma ident "%W% %E% %U% JVM"
#endif
/*
 * Copyright 1997-2006 Sun Microsystems, Inc.  All Rights Reserved.
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

#include "incls/_precompiled.incl"
#include "incls/_assembler.cpp.incl"


// Implementation of AbstractAssembler
//
// The AbstractAssembler is generating code into a CodeBuffer. To make code generation faster,
// the assembler keeps a copy of the code buffers boundaries & modifies them when
// emitting bytes rather than using the code buffers accessor functions all the time.
// The code buffer is updated via set_code_end(...) after emiting a whole instruction.

AbstractAssembler::AbstractAssembler(CodeBuffer* code) {
  if (code == NULL)  return;
  CodeSection* cs = code->insts();
  cs->clear_mark();   // new assembler kills old mark
  _code_section = cs;
  _code_begin  = cs->start();
  _code_limit  = cs->limit();
  _code_pos    = cs->end();
  _oop_recorder= code->oop_recorder();
  if (_code_begin == NULL)  {
    vm_exit_out_of_memory1(0, "CodeCache: no room for %s", code->name());
  }
}

void AbstractAssembler::set_code_section(CodeSection* cs) {
  assert(cs->outer() == code_section()->outer(), "sanity");
  assert(cs->is_allocated(), "need to pre-allocate this section");
  cs->clear_mark();  // new assembly into this section kills old mark
  _code_section = cs;
  _code_begin  = cs->start();
  _code_limit  = cs->limit();
  _code_pos    = cs->end();
}

// Inform CodeBuffer that incoming code and relocation will be for stubs
address AbstractAssembler::start_a_stub(int required_space) {
  CodeBuffer*  cb = code();
  CodeSection* cs = cb->stubs();
  assert(_code_section == cb->insts(), "not in insts?");
  sync();
  if (cs->maybe_expand_to_ensure_remaining(required_space)
      && cb->blob() == NULL) {
    return NULL;
  }
  set_code_section(cs);
  return pc();
}

// Inform CodeBuffer that incoming code and relocation will be code
// Should not be called if start_a_stub() returned NULL
void AbstractAssembler::end_a_stub() {
  assert(_code_section == code()->stubs(), "not in stubs?");
  sync();
  set_code_section(code()->insts());
}

// Inform CodeBuffer that incoming code and relocation will be for stubs
address AbstractAssembler::start_a_const(int required_space, int required_align) {
  CodeBuffer*  cb = code();
  CodeSection* cs = cb->consts();
  assert(_code_section == cb->insts(), "not in insts?");
  sync();
  address end = cs->end();
  int pad = -(intptr_t)end & (required_align-1);
  if (cs->maybe_expand_to_ensure_remaining(pad + required_space)) {
    if (cb->blob() == NULL)  return NULL;
    end = cs->end();  // refresh pointer
  }
  if (pad > 0) {
    while (--pad >= 0) { *end++ = 0; }
    cs->set_end(end);
  }
  set_code_section(cs);
  return end;
}

// Inform CodeBuffer that incoming code and relocation will be code
// Should not be called if start_a_const() returned NULL
void AbstractAssembler::end_a_const() {
  assert(_code_section == code()->consts(), "not in consts?");
  sync();
  set_code_section(code()->insts());
}


void AbstractAssembler::flush() {
  sync();
  ICache::invalidate_range(addr_at(0), offset());
}


void AbstractAssembler::a_byte(int x) {
  emit_byte(x);
}


void AbstractAssembler::a_long(jint x) {
  emit_long(x);
}

// Labels refer to positions in the (to be) generated code.  There are bound
// and unbound
//
// Bound labels refer to known positions in the already generated code.
// offset() is the position the label refers to.
//
// Unbound labels refer to unknown positions in the code to be generated; it
// may contain a list of unresolved displacements that refer to it
#ifndef PRODUCT
void AbstractAssembler::print(Label& L) {
  if (L.is_bound()) {
    tty->print_cr("bound label to %d|%d", L.loc_pos(), L.loc_sect());
  } else if (L.is_unbound()) {
    L.print_instructions((MacroAssembler*)this);
  } else {
    tty->print_cr("label in inconsistent state (loc = %d)", L.loc());
  }
}
#endif // PRODUCT


void AbstractAssembler::bind(Label& L) {
  if (L.is_bound()) {
    // Assembler can bind a label more than once to the same place.
    guarantee(L.loc() == locator(), "attempt to redefine label");
    return;
  }
  L.bind_loc(locator());
  L.patch_instructions((MacroAssembler*)this);
}

void AbstractAssembler::generate_stack_overflow_check( int frame_size_in_bytes) {
  if (UseStackBanging) {
    // Each code entry causes one stack bang n pages down the stack where n
    // is configurable by StackBangPages.  The setting depends on the maximum
    // depth of VM call stack or native before going back into java code,
    // since only java code can raise a stack overflow exception using the
    // stack banging mechanism.  The VM and native code does not detect stack
    // overflow.
    // The code in JavaCalls::call() checks that there is at least n pages
    // available, so all entry code needs to do is bang once for the end of
    // this shadow zone.
    // The entry code may need to bang additional pages if the framesize
    // is greater than a page.

    const int page_size = os::vm_page_size();
    int bang_end = StackShadowPages*page_size;

    // This is how far the previous frame's stack banging extended.
    const int bang_end_safe = bang_end;

    if (frame_size_in_bytes > page_size) {
      bang_end += frame_size_in_bytes;
    }

    int bang_offset = bang_end_safe;
    while (bang_offset <= bang_end) {
      // Need at least one stack bang at end of shadow zone.
      bang_stack_with_offset(bang_offset);
      bang_offset += page_size;
    }
  } // end (UseStackBanging)
}

void Label::add_patch_at(CodeBuffer* cb, int branch_loc) {
  assert(_loc == -1, "Label is unbound");
  if (_patch_index < PatchCacheSize) {
    _patches[_patch_index] = branch_loc;
  } else {
    if (_patch_overflow == NULL) {
      _patch_overflow = cb->create_patch_overflow();
    }
    _patch_overflow->push(branch_loc);
  }
  ++_patch_index;
}

void Label::patch_instructions(MacroAssembler* masm) {
  assert(is_bound(), "Label is bound");
  CodeBuffer* cb = masm->code();
  int target_sect = CodeBuffer::locator_sect(loc());
  address target = cb->locator_address(loc());
  while (_patch_index > 0) {
    --_patch_index;
    int branch_loc;
    if (_patch_index >= PatchCacheSize) {
      branch_loc = _patch_overflow->pop();
    } else {
      branch_loc = _patches[_patch_index];
    }
    int branch_sect = CodeBuffer::locator_sect(branch_loc);
    address branch = cb->locator_address(branch_loc);
    if (branch_sect == CodeBuffer::SECT_CONSTS) {
      // The thing to patch is a constant word.
      *(address*)branch = target;
      continue;
    }

#ifdef ASSERT
    // Cross-section branches only work if the
    // intermediate section boundaries are frozen.
    if (target_sect != branch_sect) {
      for (int n = MIN2(target_sect, branch_sect),
               nlimit = (target_sect + branch_sect) - n;
           n < nlimit; n++) {
        CodeSection* cs = cb->code_section(n);
        assert(cs->is_frozen(), "cross-section branch needs stable offsets");
      }
    }
#endif //ASSERT

    // Push the target offset into the branch instruction.
    masm->pd_patch_instruction(branch, target);
  }
}


void AbstractAssembler::block_comment(const char* comment) {
  if (sect() == CodeBuffer::SECT_INSTS) {
    code_section()->outer()->block_comment(offset(), comment);
  }
}


#ifndef PRODUCT
void Label::print_instructions(MacroAssembler* masm) const {
  CodeBuffer* cb = masm->code();
  for (int i = 0; i < _patch_index; ++i) {
    int branch_loc;
    if (i >= PatchCacheSize) {
      branch_loc = _patch_overflow->at(i - PatchCacheSize);
    } else {
      branch_loc = _patches[i];
    }
    int branch_pos  = CodeBuffer::locator_pos(branch_loc);
    int branch_sect = CodeBuffer::locator_sect(branch_loc);
    address branch = cb->locator_address(branch_loc);
    tty->print_cr("unbound label");
    tty->print("@ %d|%d ", branch_pos, branch_sect);
    if (branch_sect == CodeBuffer::SECT_CONSTS) {
      tty->print_cr(PTR_FORMAT, *(address*)branch);
      continue;
    }
    masm->pd_print_patched_instruction(branch);
    tty->cr();
  }
}
#endif // ndef PRODUCT