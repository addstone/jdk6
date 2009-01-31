/*
 * Copyright 2005 Sun Microsystems, Inc.  All Rights Reserved.
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

package sun.jvm.hotspot.runtime;

import java.io.*;
import java.util.*;
import sun.jvm.hotspot.memory.*;
import sun.jvm.hotspot.oops.*;

public class ConcurrentLocksPrinter {
    private Map locksMap = new HashMap(); // <JavaThread, List<Oop>>

    public ConcurrentLocksPrinter() {
        fillLocks();
    }

    public void print(JavaThread jthread, PrintStream tty) {
        List locks = (List) locksMap.get(jthread);    
        tty.println("Locked ownable synchronizers:");
        if (locks == null || locks.isEmpty()) {
            tty.println("    - None");
        } else {
            for (Iterator itr = locks.iterator(); itr.hasNext();) {
                Oop oop = (Oop) itr.next();
                tty.println("    - <" + oop.getHandle() + ">, (a " +
                       oop.getKlass().getName().asString() + ")");
            }
        }
    }

    //-- Internals only below this point
    private JavaThread getOwnerThread(Oop oop) {
        Oop threadOop = OopUtilities.abstractOwnableSynchronizerGetOwnerThread(oop);
        if (threadOop == null) {
            return null;
        } else {
            return OopUtilities.threadOopGetJavaThread(threadOop);
        }
    }

    private void fillLocks() {
        VM vm = VM.getVM();
        SystemDictionary sysDict = vm.getSystemDictionary();
        Klass absOwnSyncKlass = sysDict.getAbstractOwnableSynchronizerKlass();
        ObjectHeap heap = vm.getObjectHeap();
        // may be not loaded at all
        if (absOwnSyncKlass != null) {
            heap.iterateObjectsOfKlass(new DefaultHeapVisitor() {
                    public boolean doObj(Oop oop) {
                        JavaThread thread = getOwnerThread(oop);
                        if (thread != null) {
                            List locks = (List) locksMap.get(thread);
                            if (locks == null) { 
                                locks = new LinkedList();
                                locksMap.put(thread, locks);
                            }
                            locks.add(oop);
                        }
                        return false;
                    }

                }, absOwnSyncKlass, true);
        }
    }
}