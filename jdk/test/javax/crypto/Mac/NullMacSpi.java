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

/*
 * @test
 * @bug 4955833
 * @summary Make sure that there is no unexpected NPE when calling
 * getProvider() with null MacSpi object.
 * @author Valerie Peng
 */
import javax.crypto.*;
import java.security.Provider;

public class NullMacSpi {

    public static void main(String[] args) throws Exception {
	MyMac mm = new MyMac(null, null, null);
	if (mm.getProvider() == null) {
	    System.out.println("Test Passed");
	}
    }
}

class MyMac extends Mac {
    public MyMac(MacSpi macSpi, Provider provider,String algorithm) {
	super(macSpi, provider, algorithm);
    }
}