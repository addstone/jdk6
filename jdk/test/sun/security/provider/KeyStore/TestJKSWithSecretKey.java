/*
 * Copyright 2003 Sun Microsystems, Inc.  All Rights Reserved.
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
 * @bug 4804972
 * @summary Ensure JKS keystore implementation fail early when users 
 * attempt to store SecretKeys.
 */
import javax.crypto.SecretKey;
import javax.crypto.spec.SecretKeySpec;
import java.security.KeyStore;
import java.security.Security;
import java.security.Key;
import java.security.KeyStoreException;
     
public class TestJKSWithSecretKey { 
 
    private static char[] passwd = { 'p','a','s','s','w','d'};

    public static void main (String[] args) throws Exception {
	SecretKey key = new SecretKeySpec(new byte[8], "DES");
 
        KeyStore ks = KeyStore.getInstance("JKS");
	ks.load(null, passwd);

	try {
            // store the SecretKey
            ks.setKeyEntry("test_encrypt_key", key, passwd, null);
	    throw new Exception("Should throw KeyStoreException when " + 
		"storing SecretKey into JKS keystores");
	} catch (KeyStoreException kse) {
	    // expected exception thrown; swallow
	}
    }
} 
