/*
 * Copyright 1999-2001 Sun Microsystems, Inc.  All Rights Reserved.
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
 */

/* @test
   @bug 4203650
   @summary Ensure that File constructors and methods catch null arguments
   @run main/othervm NullArgs
 */


import java.io.File;

public class NullArgs {

    public static void main(String[] args) throws Exception {

	for (int i = 0;; i++) {
	    try {
		switch (i) {
		case 0:  new File((String)null);  break;
		case 1:  new File((String)null, null);  break;
		case 2:  new File((File)null, null);  break;
		case 3:  File.createTempFile(null, null, null);  break;
		case 4:  File.createTempFile(null, null);  break;
		case 5:  new File("foo").compareTo(null);  break;
		case 6:  new File("foo").renameTo(null);  break;
		default:
		    System.err.println();
		    return;
		}
	    } catch (NullPointerException x) {
		System.err.print(i + " ");
		continue;
	    }
	    throw new Exception("NullPointerException not thrown (case " +
				i + ")");
	}

    }

}