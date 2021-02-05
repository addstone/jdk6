#ifdef USE_PRAGMA_IDENT_HDR
#pragma ident "@(#)preserveException.hpp	1.20 07/05/05 17:07:11 JVM"
#endif
/*
 * Copyright 1998-2006 Sun Microsystems, Inc.  All Rights Reserved.
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

// This file provides more support for exception handling; see also exceptions.hpp
class PreserveExceptionMark {
 private:
  Thread*     _thread;
  Handle      _preserved_exception_oop;
  int         _preserved_exception_line;
  const char* _preserved_exception_file;

 public:
  PreserveExceptionMark(Thread*& thread);
  ~PreserveExceptionMark();
};


// This is a clone of PreserveExceptionMark which asserts instead
// of failing when what it wraps generates a pending exception.
// It also addresses bug 6431341.
class CautiouslyPreserveExceptionMark {
 private:
  Thread*     _thread;
  Handle      _preserved_exception_oop;
  int         _preserved_exception_line;
  const char* _preserved_exception_file;

 public:
  CautiouslyPreserveExceptionMark(Thread* thread);
  ~CautiouslyPreserveExceptionMark();
};


// Like PreserveExceptionMark but allows new exceptions to be generated in
// the body of the mark. If a new exception is generated then the original one
// is discarded.
class WeakPreserveExceptionMark {
private:
  Thread*     _thread;
  Handle      _preserved_exception_oop;
  int         _preserved_exception_line;
  const char* _preserved_exception_file;
  
  void        preserve();
  void        restore();
  
  public:
    WeakPreserveExceptionMark(Thread* pThread) :  _thread(pThread), _preserved_exception_oop()  {
      if (pThread->has_pending_exception()) {
        preserve();
      }
    }
    ~WeakPreserveExceptionMark() {
      if (_preserved_exception_oop.not_null()) {
        restore();
      }
    }
};



// use global exception mark when allowing pending exception to be set and
// saving and restoring them 
#define PRESERVE_EXCEPTION_MARK                    Thread* THREAD; PreserveExceptionMark __em(THREAD);
