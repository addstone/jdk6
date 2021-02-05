/*
 * Copyright 1998-2007 Sun Microsystems, Inc.  All Rights Reserved.
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

/*
 * @test
 * @bug 0000000
 * @summary test PBEKey
 * @author Jan Luehe
 */
import java.security.spec.*;
import javax.crypto.*;
import javax.crypto.spec.*;

public class PBEKeyTest {

    public static void main(String[] args) throws Exception {

	SecretKeyFactory fac = SecretKeyFactory.getInstance("PBEWithMD5AndDES");

	// Valid password
	char[] pass = new char[] { 'p', 'a', 's', 's', 'w', 'o', 'r', 'd' };
	PBEKeySpec spec = new PBEKeySpec(pass);
	SecretKey skey = fac.generateSecret(spec);
	KeySpec spec1 = fac.getKeySpec(skey, PBEKeySpec.class);
	SecretKey skey1 = fac.generateSecret(spec1);
	if (!skey.equals(skey1))
	    throw new Exception("Equal keys not equal");
	System.out.println(new String(((PBEKeySpec)spec1).getPassword()));

	// Invalid password
	pass = new char[] { 'p', 'a', 's', 's', 'w', 'o', 'r', '\u0019' };
	spec = new PBEKeySpec(pass);
	try {
	    skey = fac.generateSecret(spec);
	    throw new Exception("Expected exception not thrown");
	} catch (Exception e) {
	    System.out.println("Expected exception thrown");
	}
    }
}
