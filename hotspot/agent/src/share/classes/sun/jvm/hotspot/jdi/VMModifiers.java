/*
 * Copyright 2002-2003 Sun Microsystems, Inc.  All Rights Reserved.
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

package sun.jvm.hotspot.jdi;

import sun.jvm.hotspot.runtime.ClassConstants;
import com.sun.jdi.*;

public interface VMModifiers extends ClassConstants {
    int PUBLIC       = (int) JVM_ACC_PUBLIC;       /* visible to everyone */
    int PRIVATE      = (int) JVM_ACC_PRIVATE;      /* visible only to the defining class */
    int PROTECTED    = (int) JVM_ACC_PROTECTED;    /* visible to subclasses */
    int STATIC       = (int) JVM_ACC_STATIC;       /* instance variable is static */
    int FINAL        = (int) JVM_ACC_FINAL;        /* no further subclassing, overriding */
    int SYNCHRONIZED = (int) JVM_ACC_SYNCHRONIZED; /* wrap method call in monitor lock */
    int VOLATILE     = (int) JVM_ACC_VOLATILE;     /* can cache in registers */
    int BRIDGE       = (int) JVM_ACC_BRIDGE;       /* bridge method generated by compiler */
    int TRANSIENT    = (int) JVM_ACC_TRANSIENT;    /* not persistant */
    int VARARGS      = (int) JVM_ACC_VARARGS;      /* method declared with variable number of args */
    int IS_ENUM_CONSTANT = (int) JVM_ACC_ENUM;     /* field is declared as element of enum */
    int NATIVE       = (int) JVM_ACC_NATIVE;       /* implemented in C */
    int INTERFACE    = (int) JVM_ACC_INTERFACE;    /* class is an interface */
    int ABSTRACT     = (int) JVM_ACC_ABSTRACT;     /* no definition provided */
    int SYNTHETIC    = (int) JVM_ACC_SYNTHETIC;    /* not in source code */
}
