#ifdef USE_PRAGMA_IDENT_SRC
#pragma ident "%W% %E% %U% JVM"
#endif
/*
 * Copyright 2003-2007 Sun Microsystems, Inc.  All Rights Reserved.
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

// do not include  precompiled  header file
#include "incls/_os_linux_amd64.cpp.incl"

// put OS-includes here
#include <sys/types.h>
#include <sys/mman.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <pwd.h>
#include <poll.h>
#include <ucontext.h>

address os::current_stack_pointer()
{
  register void *rsp __asm__ ("rsp");
  return (address) rsp;
}

char* os::non_memory_address_word()
{
  // Must never look like an address returned by reserve_memory, even
  // in its subfields (as defined by the CPU immediate fields, if the
  // CPU splits constants across multiple instructions).

  return (char*) -1;
}

void os::initialize_thread()
{
// Nothing to do.
}

address os::Linux::ucontext_get_pc(ucontext_t* uc)
{
  return (address) uc->uc_mcontext.gregs[REG_RIP];
}

intptr_t* os::Linux::ucontext_get_sp(ucontext_t* uc)
{
  return (intptr_t*) uc->uc_mcontext.gregs[REG_RSP];
}

intptr_t* os::Linux::ucontext_get_fp(ucontext_t* uc)
{
  return (intptr_t*) uc->uc_mcontext.gregs[REG_RBP];
}

// For Forte Analyzer AsyncGetCallTrace profiling support - thread is
// currently interrupted by SIGPROF.
// os::Solaris::fetch_frame_from_ucontext() tries to skip nested
// signal frames. Currently we don't do that on Linux, so it's the
// same as os::fetch_frame_from_context().
ExtendedPC os::Linux::fetch_frame_from_ucontext(Thread* thread,
                                                ucontext_t* uc,
                                                intptr_t** ret_sp,
                                                intptr_t** ret_fp)
{
  assert(thread != NULL, "just checking");
  assert(ret_sp != NULL, "just checking");
  assert(ret_fp != NULL, "just checking");

  return os::fetch_frame_from_context(uc, ret_sp, ret_fp);
}

ExtendedPC os::fetch_frame_from_context(void* ucVoid,
                                        intptr_t** ret_sp,
                                        intptr_t** ret_fp)
{
  ucontext_t* uc = (ucontext_t*) ucVoid;
  ExtendedPC  epc;

  if (uc != NULL) {
    epc = ExtendedPC(os::Linux::ucontext_get_pc(uc));
    if (ret_sp) {
      *ret_sp = os::Linux::ucontext_get_sp(uc);
    }
    if (ret_fp) {
      *ret_fp = os::Linux::ucontext_get_fp(uc);
    }
  } else {
    // construct empty ExtendedPC for return value checking
    epc = ExtendedPC(NULL);
    if (ret_sp) {
      *ret_sp = (intptr_t*) NULL;
    }
    if (ret_fp) {
      *ret_fp = (intptr_t*) NULL;
    }
  }

  return epc;
}

frame os::fetch_frame_from_context(void* ucVoid)
{
  intptr_t* sp;
  intptr_t* fp;
  ExtendedPC epc = fetch_frame_from_context(ucVoid, &sp, &fp);
  return frame(sp, fp, epc.pc());
}

// XXX
// By default, gcc always saves frame pointer (%rbp) on stack. It may get
// turned off by -fomit-frame-pointer,
frame os::get_sender_for_C_frame(frame* fr) {
  return frame(fr->sender_sp(), fr->link(), fr->sender_pc());
}


intptr_t* _get_previous_fp() {
  register intptr_t **ebp __asm__ ("rbp");
  return (intptr_t*) *ebp;   // we want what it points to.
}

frame os::current_frame()
{
  intptr_t* fp = _get_previous_fp();
  frame myframe((intptr_t*) os::current_stack_pointer(),
                (intptr_t*) fp,
                CAST_FROM_FN_PTR(address, os::current_frame));
  if (os::is_first_C_frame(&myframe)) {
    // stack is not walkable
    return frame(NULL, NULL, NULL);
  } else {
    return os::get_sender_for_C_frame(&myframe);
  }
}


// Utility functions

julong os::allocatable_physical_memory(julong size)
{
  return size;
}

extern "C" void Fetch32PFI () ; 
extern "C" void Fetch32Resume () ; 
extern "C" void FetchNPFI () ; 
extern "C" void FetchNResume () ; 



extern "C" int 
JVM_handle_linux_signal(int sig,
                        siginfo_t* info,
                        void* ucVoid,
                        int abort_if_unrecognized)
{
  ucontext_t* uc = (ucontext_t*) ucVoid;

  Thread* t = ThreadLocalStorage::get_thread_slow();

  SignalHandlerMark shm(t);

  // Note: it's not uncommon that JNI code uses signal/sigset to install
  // then restore certain signal handler (e.g. to temporarily block SIGPIPE,
  // or have a SIGILL handler when detecting CPU type). When that happens,
  // JVM_handle_linux_signal() might be invoked with junk info/ucVoid. To 
  // avoid unnecessary crash when libjsig is not preloaded, try handle signals 
  // that do not require siginfo/ucontext first.

  if (sig == SIGPIPE || sig == SIGXFSZ) {
    // allow chained handler to go first
    if (os::Linux::chained_handler(sig, info, ucVoid)) {
      return true;
    } else {
      if (PrintMiscellaneous && (WizardMode || Verbose)) {
        char buf[64];
        warning("Ignoring %s - see bugs 4229104 or 646499219", 
                os::exception_name(sig, buf, sizeof(buf)));
      }
      return true;
    }
  }

  JavaThread* thread = NULL;
  VMThread* vmthread = NULL;
  if (os::Linux::signal_handlers_are_installed) {
    if (t != NULL ){
      if(t->is_Java_thread()) {
        thread = (JavaThread*)t;
      }
      else if(t->is_VM_thread()){
        vmthread = (VMThread *)t;
      }
    }
  }

  // decide if this trap can be handled by a stub
  address stub = NULL;

  address pc = NULL;

  //%note os_trap_1
  if (info != NULL && uc != NULL && thread != NULL) {
    pc = os::Linux::ucontext_get_pc(uc);

    if (pc == (address) Fetch32PFI) { 
       uc->uc_mcontext.gregs[REG_RIP] = intptr_t (Fetch32Resume) ; 
       return 1 ; 
    } 
    if (pc == (address) FetchNPFI) { 
       uc->uc_mcontext.gregs[REG_RIP] = intptr_t (FetchNResume) ; 
       return 1 ; 
    } 

    // Handle ALL stack overflow variations here
    if (sig == SIGSEGV) {
      address addr = (address) info->si_addr;

      // check if fault address is within thread stack
      if (addr < thread->stack_base() &&
          addr >= thread->stack_base() - thread->stack_size()) {
        // stack overflow
        if (thread->in_stack_yellow_zone(addr)) {
          thread->disable_stack_yellow_zone();
          if (thread->thread_state() == _thread_in_Java) {
            // Throw a stack overflow exception.  Guard pages will be reenabled
            // while unwinding the stack.
            stub =
              SharedRuntime::
              continuation_for_implicit_exception(thread,
                                                  pc,
                                                  SharedRuntime::STACK_OVERFLOW);
          } else {
            // Thread was in the vm or native code.  Return and try to finish.
            return 1;
          }
        } else if (thread->in_stack_red_zone(addr)) {
          // Fatal red zone violation.  Disable the guard pages and fall through
          // to handle_unexpected_exception way down below.
          thread->disable_stack_red_zone();
          tty->print_raw_cr("An irrecoverable stack overflow has occurred.");
        } else {
          // Accessing stack address below sp may cause SEGV if current
          // thread has MAP_GROWSDOWN stack. This should only happen when
          // current thread was created by user code with MAP_GROWSDOWN flag
          // and then attached to VM. See notes in os_linux.cpp.
          if (thread->osthread()->expanding_stack() == 0) {
             thread->osthread()->set_expanding_stack();
             if (os::Linux::manually_expand_stack(thread, addr)) {
               thread->osthread()->clear_expanding_stack();
               return 1;
             }
             thread->osthread()->clear_expanding_stack();
          } else {
             fatal("recursive segv. expanding stack.");
          }
        } 
      }
    }

    if (thread->thread_state() == _thread_in_Java) {
      // Java thread running in Java code => find exception handler if any
      // a fault inside compiled code, the interpreter, or a stub

      if (sig == SIGSEGV && (address) info->si_addr == os::get_polling_page()) {
        stub = SharedRuntime::get_poll_stub(pc);
      } else if (sig == SIGBUS /*&& info->si_code == BUS_OBJERR */) {
	// BugId 4454115: A read from a MappedByteBuffer can fault
	// here if the underlying file has been truncated.
	// Do not crash the VM in such a case.
        CodeBlob* cb = CodeCache::find_blob_unsafe(pc);
        nmethod* nm = cb->is_nmethod() ? (nmethod*)cb : NULL;
        if (nm != NULL && nm->has_unsafe_access()) {
	  stub = StubRoutines::handler_for_unsafe_access();
	}
      } else

      if (sig == SIGFPE  && 
          (info->si_code == FPE_INTDIV || info->si_code == FPE_FLTDIV)) {
        stub = 
          SharedRuntime::
          continuation_for_implicit_exception(thread,
                                              pc, 
                                              SharedRuntime::
                                              IMPLICIT_DIVIDE_BY_ZERO);

      } else if (sig == SIGSEGV &&
               !MacroAssembler::needs_explicit_null_check((intptr_t)
                                                          info->si_addr)) {
        stub =
          SharedRuntime::
          continuation_for_implicit_exception(thread, pc, 
                                              SharedRuntime::
                                              IMPLICIT_NULL);
      }
    } else if (thread->thread_state() == _thread_in_vm &&
               sig == SIGBUS && /* info->si_code == BUS_OBJERR && */
               thread->doing_unsafe_access()) {
        stub = StubRoutines::handler_for_unsafe_access(); 
    }

    // jni_fast_Get<Primitive>Field can trap at certain pc's if a GC kicks in
    // and the heap gets shrunk before the field access.
    if ((sig == SIGSEGV) || (sig == SIGBUS)) {
      address addr = JNI_FastGetField::find_slowcase_pc(pc);
      if (addr != (address)-1) {
        stub = addr;
      }
    }

    // Check to see if we caught the safepoint code in the
    // process of write protecting the memory serialization page.
    // It write enables the page immediately after protecting it
    // so we can just return to retry the write.
    if ((sig == SIGSEGV) &&
        os::is_memory_serialize_page(thread, (address)info->si_addr)) {
      // Block current thread until the memory serialize page permission restored.
      os::block_on_serialize_page_trap();
      return true;
    }
  }

  if (stub != NULL) {
    // save all thread context in case we need to restore it
    thread->set_saved_exception_pc(pc);

    uc->uc_mcontext.gregs[REG_RIP] = (greg_t) stub;
    return true;
  }

  // signal-chaining
  if (os::Linux::chained_handler(sig, info, ucVoid)) {
     return true;
  }

  if (!abort_if_unrecognized) {
    // caller wants another chance, so give it to him
    return false;
  }

  if (pc == NULL && uc != NULL) {
    pc = os::Linux::ucontext_get_pc(uc);
  }

  // unmask current signal
  sigset_t newset;
  sigemptyset(&newset);
  sigaddset(&newset, sig);
  sigprocmask(SIG_UNBLOCK, &newset, NULL);

  VMError err(t, sig, pc, info, ucVoid);
  err.report_and_die();

  ShouldNotReachHere();
}

void os::Linux::init_thread_fpu_state(void)
{
  // Nothing to do
}

int os::Linux::get_fpu_control_word() {
  return 0;
}

void os::Linux::set_fpu_control_word(int fpu) {
  // nothing
}

///////////////////////////////////////////////////////////////////////////////
// thread stack

size_t os::Linux::min_stack_allowed  = 64 * K;

// amd64: pthread on amd64 is always in floating stack mode
bool os::Linux::supports_variable_stack_size() {  return true; }

// return default stack size for thr_type
size_t os::Linux::default_stack_size(os::ThreadType thr_type) {
  // default stack size (compiler thread needs larger stack)
  size_t s = (thr_type == os::compiler_thread ? 4 * M : 1 * M);
  return s;
} 
  
size_t os::Linux::default_guard_size(os::ThreadType thr_type) {
  // Creating guard page is very expensive. Java thread has HotSpot
  // guard page, only enable glibc guard page for non-Java threads.
  return (thr_type == java_thread ? 0 : page_size());
}

// Java thread:
//
//   Low memory addresses
//    +------------------------+
//    |                        |\  JavaThread created by VM does not have glibc
//    |    glibc guard page    | - guard, attached Java thread usually has
//    |                        |/  1 page glibc guard.
// P1 +------------------------+ Thread::stack_base() - Thread::stack_size()
//    |                        |\
//    |  HotSpot Guard Pages   | - red and yellow pages
//    |                        |/
//    +------------------------+ JavaThread::stack_yellow_zone_base()
//    |                        |\
//    |      Normal Stack      | -
//    |                        |/
// P2 +------------------------+ Thread::stack_base()
//
// Non-Java thread:
//
//   Low memory addresses
//    +------------------------+
//    |                        |\
//    |  glibc guard page      | - usually 1 page
//    |                        |/
// P1 +------------------------+ Thread::stack_base() - Thread::stack_size()
//    |                        |\
//    |      Normal Stack      | -
//    |                        |/
// P2 +------------------------+ Thread::stack_base()
//
// ** P1 (aka bottom) and size ( P2 = P1 - size) are the address and stack size returned from
//    pthread_attr_getstack()

static void current_stack_region(address* bottom, size_t* size) {
  if (os::Linux::is_initial_thread()) {
     // initial thread needs special handling because pthread_getattr_np()
     // may return bogus value.
     *bottom = os::Linux::initial_thread_stack_bottom();
     *size = os::Linux::initial_thread_stack_size();
  } else {
     pthread_attr_t attr;

     int rslt = pthread_getattr_np(pthread_self(), &attr);

     // JVM needs to know exact stack location, abort if it fails
     if (rslt != 0) {
       if (rslt == ENOMEM) {
         vm_exit_out_of_memory(0, "pthread_getattr_np");
       } else {
         fatal1("pthread_getattr_np failed with errno = %d", rslt);
       }
     }

     if (pthread_attr_getstack(&attr, (void **)bottom, size) != 0 ) {
         fatal("Can not locate current stack attributes!");
     }

     pthread_attr_destroy(&attr);

  }
  assert(os::current_stack_pointer() >= *bottom &&
         os::current_stack_pointer() < *bottom + *size, "just checking");
}

address os::current_stack_base()
{
  address bottom;
  size_t size;
  current_stack_region(&bottom, &size);
  return bottom + size;
}

size_t os::current_stack_size()
{
  // stack size includes normal stack and HotSpot guard pages
  address bottom;
  size_t size;
  current_stack_region(&bottom, &size);
  return size;
}

/////////////////////////////////////////////////////////////////////////////
// helper functions for fatal error handler

void os::print_context(outputStream* st, void* context)
{
  if (context == NULL) {
    return;
  }

  ucontext_t* uc = (ucontext_t*) context;
  st->print_cr("Registers:");
  st->print(  "RAX=" INTPTR_FORMAT, uc->uc_mcontext.gregs[REG_RAX]);
  st->print(", RBX=" INTPTR_FORMAT, uc->uc_mcontext.gregs[REG_RBX]);
  st->print(", RCX=" INTPTR_FORMAT, uc->uc_mcontext.gregs[REG_RCX]);
  st->print(", RDX=" INTPTR_FORMAT, uc->uc_mcontext.gregs[REG_RDX]);
  st->cr();
  st->print(  "RSP=" INTPTR_FORMAT, uc->uc_mcontext.gregs[REG_RSP]);
  st->print(", RBP=" INTPTR_FORMAT, uc->uc_mcontext.gregs[REG_RBP]);
  st->print(", RSI=" INTPTR_FORMAT, uc->uc_mcontext.gregs[REG_RSI]);
  st->print(", RDI=" INTPTR_FORMAT, uc->uc_mcontext.gregs[REG_RDI]);
  st->cr();
  st->print(  "R8 =" INTPTR_FORMAT, uc->uc_mcontext.gregs[REG_R8]);
  st->print(", R9 =" INTPTR_FORMAT, uc->uc_mcontext.gregs[REG_R9]);
  st->print(", R10=" INTPTR_FORMAT, uc->uc_mcontext.gregs[REG_R10]);
  st->print(", R11=" INTPTR_FORMAT, uc->uc_mcontext.gregs[REG_R11]);
  st->cr();
  st->print(  "R12=" INTPTR_FORMAT, uc->uc_mcontext.gregs[REG_R12]);
  st->print(", R13=" INTPTR_FORMAT, uc->uc_mcontext.gregs[REG_R13]);
  st->print(", R14=" INTPTR_FORMAT, uc->uc_mcontext.gregs[REG_R14]);
  st->print(", R15=" INTPTR_FORMAT, uc->uc_mcontext.gregs[REG_R15]);
  st->cr();
  st->print(  "RIP=" INTPTR_FORMAT, uc->uc_mcontext.gregs[REG_RIP]);
  st->print(", EFL=" INTPTR_FORMAT, uc->uc_mcontext.gregs[REG_EFL]);
  st->print(", CSGSFS=" INTPTR_FORMAT, uc->uc_mcontext.gregs[REG_CSGSFS]);
  st->print(", ERR=" INTPTR_FORMAT, uc->uc_mcontext.gregs[REG_ERR]);
  st->cr();
  st->print("  TRAPNO=" INTPTR_FORMAT, uc->uc_mcontext.gregs[REG_TRAPNO]);
  st->cr();
  st->cr();

  intptr_t* sp = (intptr_t*) os::Linux::ucontext_get_sp(uc);
  st->print_cr("Top of Stack: (sp=" PTR_FORMAT ")", sp);
  print_hex_dump(st, (address) sp, (address) (sp + 8*sizeof(intptr_t)), sizeof(intptr_t));
  st->cr();

  // Note: it may be unsafe to inspect memory near pc. For example, pc
  // may point to garbage if entry point in an nmethod is
  // corrupted. Leave this at the end, and hope for the best.
  address pc = os::Linux::ucontext_get_pc(uc);
  st->print_cr("Instructions: (pc=" PTR_FORMAT ")", pc);
  print_hex_dump(st, pc - 16, pc + 16, sizeof(char));
}