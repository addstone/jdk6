/*
 * Copyright 2003-2004 Sun Microsystems, Inc.  All Rights Reserved.
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
 * @bug 4894125
 * @summary test that failover for KeyFactory works
 * @author Andreas Sterbenz
 */

import java.util.*;

import java.security.*;
import java.security.interfaces.*;
import java.security.spec.*;

public class Failover {
    
    public static void main(String[] args) throws Exception {
	Security.insertProviderAt(new ProviderFail(), 1);
	Security.addProvider(new ProviderPass());
	System.out.println(Arrays.asList(Security.getProviders()));

	KeyFactory kf;
	kf = KeyFactory.getInstance("FOO");
	kf.generatePublic(null);
	kf.generatePublic(null);
	kf.generatePrivate(null);

	kf = KeyFactory.getInstance("FOO");
	kf.generatePrivate(null);
	kf.getKeySpec(null, null);
	kf.translateKey(null);

	kf = KeyFactory.getInstance("FOO");
	kf.getKeySpec(null, null);
	kf.translateKey(null);

	kf = KeyFactory.getInstance("FOO");
	kf.translateKey(null);
	
	// somewhat more real tests using DSA
	System.out.println("DSA tests...");
	
	KeyPairGenerator kpg = KeyPairGenerator.getInstance("DSA");
	kpg.initialize(512);
	KeyPair kp = kpg.generateKeyPair();
	
	kf = KeyFactory.getInstance("DSA");
	System.out.println(kf.translateKey(kp.getPrivate()));
	
	kf = KeyFactory.getInstance("DSA");
	KeySpec spec = kf.getKeySpec(kp.getPublic(), DSAPublicKeySpec.class);
	
	kf = KeyFactory.getInstance("DSA");
	System.out.println(kf.generatePublic(spec));

	kf = KeyFactory.getInstance("DSA");
	try {
	    kf.generatePrivate(spec);
	    throw new Exception("no exception");
	} catch (InvalidKeySpecException e) {
	    System.out.println(e);
	}
    }
    
    private static class ProviderPass extends Provider {
	ProviderPass() {
	    super("Pass", 1.0d, "Pass");
	    put("KeyFactory.FOO" , "Failover$KeyFactoryPass");
	}
    }
    
    private static class ProviderFail extends Provider {
	ProviderFail() {
	    super("Fail", 1.0d, "Fail");
	    put("KeyFactory.FOO" , "Failover$KeyFactoryFail");
	    put("KeyFactory.DSA" , "Failover$KeyFactoryFail");
	}
    }
    
    public static class KeyFactoryPass extends KeyFactorySpi {
	
	protected PublicKey engineGeneratePublic(KeySpec keySpec) throws InvalidKeySpecException {
	    System.out.println("MyKeyFactoryPass.engineGeneratePublic()");
	    return null;
	}
    
	protected PrivateKey engineGeneratePrivate(KeySpec keySpec) throws InvalidKeySpecException {
	    System.out.println("MyKeyFactoryPass.engineGeneratePrivate()");
	    return null;
	}
    
	protected <T extends KeySpec> T
	    engineGetKeySpec(Key key, Class<T> keySpec) throws InvalidKeySpecException
	{
	    System.out.println("MyKeyFactoryPass.engineGetKeySpec()");
	    return null;
	}
    
	protected Key engineTranslateKey(Key key) throws InvalidKeyException {
	    System.out.println("MyKeyFactoryPass.engineTranslateKey()");
	    return null;
	}

    }
    
    public static class KeyFactoryFail extends KeyFactorySpi {

	protected PublicKey engineGeneratePublic(KeySpec keySpec) throws InvalidKeySpecException {
	    System.out.println("MyKeyFactoryFail.engineGeneratePublic()");
	    throw new InvalidKeySpecException();
	}
    
	protected PrivateKey engineGeneratePrivate(KeySpec keySpec) throws InvalidKeySpecException {
	    System.out.println("MyKeyFactoryFail.engineGeneratePrivate()");
	    throw new InvalidKeySpecException();
	}
    
	protected <T extends KeySpec> T engineGetKeySpec(Key key, Class<T> keySpec) throws InvalidKeySpecException {
	    System.out.println("MyKeyFactoryFail.engineGetKeySpec()");
	    throw new InvalidKeySpecException();
	}
    
	protected Key engineTranslateKey(Key key) throws InvalidKeyException {
	    System.out.println("MyKeyFactoryFail.engineTranslateKey()");
	    throw new InvalidKeyException();
	}

    }

}