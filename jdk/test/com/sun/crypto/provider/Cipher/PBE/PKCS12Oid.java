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
 */

/**
 * @test
 * @bug 4898810
 * @summary ensure PBEWithSHA1AndDESede and PBEWithSHA1AndRC2_40
 * is registered under correct OID.
 * @author Valerie Peng
 */

import java.io.*;
import java.util.*;
import java.security.*;
import javax.crypto.*;
import javax.crypto.spec.*;
import javax.crypto.interfaces.PBEKey;

public class PKCS12Oid {
    private static String OID_PBEWithSHAAnd40BitRC2CBC = "1.2.840.113549.1.12.1.6";
    private static String OID_PBEWithSHAAnd3KeyTripleDESCBC = "1.2.840.113549.1.12.1.3";

    public static void main(String[] argv) throws Exception {
	Cipher c = Cipher.getInstance(OID_PBEWithSHAAnd40BitRC2CBC, "SunJCE");
	c = Cipher.getInstance(OID_PBEWithSHAAnd3KeyTripleDESCBC, "SunJCE");
	System.out.println("All tests passed");
    }
}