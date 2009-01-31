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

package sun.jvm.hotspot.tools;

import java.io.PrintStream;
import java.util.*;

import sun.jvm.hotspot.runtime.*;

public class SysPropsDumper extends Tool {

   public void run() {
      Properties sysProps = VM.getVM().getSystemProperties();
      PrintStream out = System.out;
      if (sysProps != null) {
         Enumeration keys = sysProps.keys();
         while (keys.hasMoreElements()) {
            Object key = keys.nextElement();
            out.print(key);
            out.print(" = ");
            out.println(sysProps.get(key));
         }
      } else {
         out.println("System Properties info not available!");
      }
   }
   
   public static void main(String[] args) {
      SysPropsDumper pd = new SysPropsDumper();
      pd.start(args);
      pd.stop();
   }
}