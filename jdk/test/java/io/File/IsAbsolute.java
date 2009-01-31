/*
 * Copyright 1997-1998 Sun Microsystems, Inc.  All Rights Reserved.
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
   @bug 4022397
   @summary General test for isAbsolute
 */

import java.io.*;


public class IsAbsolute {

    private static void ck(String path, boolean ans) throws Exception {
	File f = new File(path);
	boolean x = f.isAbsolute();
	if (x != ans)
	    throw new Exception(path + ": expected " + ans + ", got " + x);
	System.err.println(path + " ==> " + x);
    }

    private static void testWin32() throws Exception {
	ck("/foo/bar", false);
	ck("\\foo\\bar", false);
	ck("c:\\foo\\bar", true);
	ck("c:/foo/bar", true);
	ck("c:foo\\bar", false);
	ck("\\\\foo\\bar", true);
    }

    private static void testUnix() throws Exception {
	ck("foo", false);
	ck("foo/bar", false);
	ck("/foo", true);
	ck("/foo/bar", true);
    }

    public static void main(String[] args) throws Exception {
	if (File.separatorChar == '\\') testWin32();
	if (File.separatorChar == '/') testUnix();
    }

}