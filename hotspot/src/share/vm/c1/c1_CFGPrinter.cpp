#ifdef USE_PRAGMA_IDENT_SRC
#pragma ident "%W% %E% %U% JVM"
#endif
/*
 * Copyright 2005-2006 Sun Microsystems, Inc.  All Rights Reserved.
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
#include "incls/_c1_CFGPrinter.cpp.incl"

#ifndef PRODUCT


class CFGPrinterOutput : public CHeapObj {
 private:
  outputStream* _output;

  Compilation*  _compilation;
  bool _do_print_HIR;
  bool _do_print_LIR;

  class PrintBlockClosure: public BlockClosure {
    void block_do(BlockBegin* block) { if (block != NULL) CFGPrinter::output()->print_block(block); }
  };


  outputStream* output() { assert(_output != NULL, ""); return _output; }

  void inc_indent();
  void dec_indent();
  void print(const char* format, ...);
  void print_begin(const char* tag);
  void print_end(const char* tag);

  char* method_name(ciMethod* method, bool short_name = false);

 public:
  CFGPrinterOutput();

  void set_compilation(Compilation* compilation) { _compilation = compilation; }
  void set_print_flags(bool do_print_HIR, bool do_print_LIR) { _do_print_HIR = do_print_HIR; _do_print_LIR = do_print_LIR; }

  void print_compilation();
  void print_intervals(IntervalList* intervals, const char* name);

  void print_state(BlockBegin* block);
  void print_operand(Value instr);
  void print_HIR(Value instr);
  void print_HIR(BlockBegin* block);
  void print_LIR(BlockBegin* block);
  void print_block(BlockBegin* block);
  void print_cfg(BlockList* blocks, const char* name);
  void print_cfg(IR* blocks, const char* name);
};

CFGPrinterOutput* CFGPrinter::_output = NULL;




void CFGPrinter::print_compilation(Compilation* compilation) {
  if (_output == NULL) {
    _output = new CFGPrinterOutput();
  }
  output()->set_compilation(compilation);
  output()->print_compilation();
}

void CFGPrinter::print_cfg(BlockList* blocks, const char* name, bool do_print_HIR, bool do_print_LIR) {
  output()->set_print_flags(do_print_HIR, do_print_LIR);
  output()->print_cfg(blocks, name);
}

void CFGPrinter::print_cfg(IR* blocks, const char* name, bool do_print_HIR, bool do_print_LIR) {
  output()->set_print_flags(do_print_HIR, do_print_LIR);
  output()->print_cfg(blocks, name);
}


void CFGPrinter::print_intervals(IntervalList* intervals, const char* name) {
  output()->print_intervals(intervals, name);
}



CFGPrinterOutput::CFGPrinterOutput() 
 : _output(new(ResourceObj::C_HEAP) fileStream("output.cfg"))
{
}



void CFGPrinterOutput::inc_indent() {
  output()->inc();
  output()->inc();
}

void CFGPrinterOutput::dec_indent() {
  output()->dec();
  output()->dec();
}

void CFGPrinterOutput::print(const char* format, ...) { 
  output()->indent();

  va_list ap;
  va_start(ap, format);
  output()->vprint_cr(format, ap);
  va_end(ap);
}

void CFGPrinterOutput::print_begin(const char* tag) {
  output()->indent();
  output()->print_cr("begin_%s", tag);
  inc_indent();
}

void CFGPrinterOutput::print_end(const char* tag) {
  dec_indent();
  output()->indent();
  output()->print_cr("end_%s", tag);
}


char* CFGPrinterOutput::method_name(ciMethod* method, bool short_name) {
  stringStream name;
  if (short_name) {
    method->print_short_name(&name);
  } else {
    method->print_name(&name);
  }
  return name.as_string();

}


void CFGPrinterOutput::print_compilation() {
  print_begin("compilation");

  print("name \"%s\"", method_name(_compilation->method(), true));
  print("method \"%s\"", method_name(_compilation->method()));
  print("date "INT64_FORMAT, os::javaTimeMillis());

  print_end("compilation");
}





void CFGPrinterOutput::print_state(BlockBegin* block) {
  print_begin("states");

  InstructionPrinter ip(true, output());

  ValueStack* state = block->state();
  int index;
  Value value;

  if (state->stack_size() > 0) {
    print_begin("stack");
    print("size %d", state->stack_size());

    for_each_stack_value(state, index, value) {
      ip.print_phi(index, value, block);
      print_operand(value);
      output()->cr();
    }

    print_end("stack");
  }

  if (state->locks_size() > 0) {
    print_begin("locks");
    print("size %d", state->locks_size());

    for_each_lock_value(state, index, value) {
      ip.print_phi(index, value, block);
      print_operand(value);
      output()->cr();
    }
    print_end("locks");
  }

  for_each_state(state) {
    print_begin("locals");
    print("size %d", state->locals_size());
    print("method \"%s\"", method_name(state->scope()->method()));

    for_each_local_value(state, index, value) {
      ip.print_phi(index, value, block);
      print_operand(value);
      output()->cr();
    }
    print_end("locals");
  }

  print_end("states");
}


void CFGPrinterOutput::print_operand(Value instr) {
  if (instr->operand()->is_virtual()) {
    output()->print(" \"");
    instr->operand()->print(output());
    output()->print("\" ");
  }
}

void CFGPrinterOutput::print_HIR(Value instr) {
  InstructionPrinter ip(true, output());

  if (instr->is_pinned()) {
    output()->put('.');
  }
  output()->print("%d %d ", instr->bci(), instr->use_count());

  print_operand(instr);

  ip.print_temp(instr);
  output()->print(" ");
  ip.print_instr(instr);

  output()->print_cr(" <|@");
}

void CFGPrinterOutput::print_HIR(BlockBegin* block) {
  print_begin("HIR");

  Value cur = block->next();
  while (cur != NULL) {
    print_HIR(cur);
    cur = cur->next();
  }

  print_end("HIR");
}

void CFGPrinterOutput::print_LIR(BlockBegin* block) {
  print_begin("LIR");

  for (int i = 0; i < block->lir()->length(); i++) {
    block->lir()->at(i)->print_on(output());
    output()->print_cr(" <|@ ");
  }

  print_end("LIR");
}


void CFGPrinterOutput::print_block(BlockBegin* block) {
  print_begin("block");
  
  print("name \"B%d\"", block->block_id());

  print("from_bci %d", block->bci());
  print("to_bci %d", (block->end() == NULL ? -1 : block->end()->bci()));

  output()->indent();
  output()->print("predecessors ");
  int i;
  for (i = 0; i < block->number_of_preds(); i++) {
    output()->print("\"B%d\" ", block->pred_at(i)->block_id());
  }
  output()->cr();

  output()->indent();
  output()->print("successors ");
  for (i = 0; i < block->number_of_sux(); i++) {
    output()->print("\"B%d\" ", block->sux_at(i)->block_id());
  }
  output()->cr();

  output()->indent();
  output()->print("xhandlers");
  for (i = 0; i < block->number_of_exception_handlers(); i++) {
    output()->print("\"B%d\" ", block->exception_handler_at(i)->block_id());
  }
  output()->cr();

  output()->indent();
  output()->print("flags ");
  if (block->is_set(BlockBegin::std_entry_flag))                output()->print("\"std\" ");
  if (block->is_set(BlockBegin::osr_entry_flag))                output()->print("\"osr\" ");
  if (block->is_set(BlockBegin::exception_entry_flag))          output()->print("\"ex\" ");
  if (block->is_set(BlockBegin::subroutine_entry_flag))         output()->print("\"sr\" ");
  if (block->is_set(BlockBegin::backward_branch_target_flag))   output()->print("\"bb\" ");
  if (block->is_set(BlockBegin::parser_loop_header_flag))       output()->print("\"plh\" ");
  if (block->is_set(BlockBegin::critical_edge_split_flag))      output()->print("\"ces\" ");
  if (block->is_set(BlockBegin::linear_scan_loop_header_flag))  output()->print("\"llh\" ");
  if (block->is_set(BlockBegin::linear_scan_loop_end_flag))     output()->print("\"lle\" ");
  output()->cr();

  if (block->dominator() != NULL) {
    print("dominator \"B%d\"", block->dominator()->block_id());
  }
  if (block->loop_index() != -1) {
    print("loop_index %d", block->loop_index());
    print("loop_depth %d", block->loop_depth());
  }

  if (block->first_lir_instruction_id() != -1) {
    print("first_lir_id %d", block->first_lir_instruction_id());
    print("last_lir_id %d", block->last_lir_instruction_id());
  }

  if (_do_print_HIR) {
    print_state(block);
    print_HIR(block);
  }

  if (_do_print_LIR) {
    print_LIR(block);
  }

  print_end("block");
}



void CFGPrinterOutput::print_cfg(BlockList* blocks, const char* name) {
  print_begin("cfg");
  print("name \"%s\"", name);

  PrintBlockClosure print_block;
  blocks->iterate_forward(&print_block);

  print_end("cfg");
  output()->flush();
}

void CFGPrinterOutput::print_cfg(IR* blocks, const char* name) {
  print_begin("cfg");
  print("name \"%s\"", name);

  PrintBlockClosure print_block;
  blocks->iterate_preorder(&print_block);

  print_end("cfg");
  output()->flush();
}




void CFGPrinterOutput::print_intervals(IntervalList* intervals, const char* name) {
  print_begin("intervals");
  print("name \"%s\"", name);

  for (int i = 0; i < intervals->length(); i++) {
    if (intervals->at(i) != NULL) {
      intervals->at(i)->print(output());
    }
  }

  print_end("intervals");
  output()->flush();
}


#endif