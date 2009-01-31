/*
 * Copyright 1998 Sun Microsystems, Inc.  All Rights Reserved.
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

import java.io.*;
/*
 * @test
 * @bug 4135031
 * @summary Test boostrap problem when a URLStreamHandlerFactory is loaded
 *          by the application class loader.
 * 
 */
import java.net.*;

public class HandlerLoop {

    public static void main(String args[]) throws Exception {
	URL.setURLStreamHandlerFactory(
	    new HandlerFactory("sun.net.www.protocol"));
	URL url = new URL("file://bogus/index.html");
	System.out.println("url = " + url);
	url.openConnection();
    }

    private static class HandlerFactory implements URLStreamHandlerFactory {
	private String pkg;

	HandlerFactory(String pkg) {
	    this.pkg = pkg;
	}

	public URLStreamHandler createURLStreamHandler(String protocol) {
	    String name = pkg + "." + protocol + ".Handler";
	    System.out.println("Loading handler class: " + name);
	    // Loading this dummy class demonstrates the bootstrap
	    // problem as the stream handler factory will be reentered
	    // over and over again if the application class loader
	    // shares the same stream handler factory.
	    new Dummy();
	    try {
	        Class c = Class.forName(name);
		return (URLStreamHandler)c.newInstance();
	    } catch (ClassNotFoundException e) {
		e.printStackTrace();
	    } catch (IllegalAccessException e) {
		e.printStackTrace();
	    } catch (InstantiationException e) {
		e.printStackTrace();
	    }
	    return null;
	}
    }

    private static class Dummy {
    }
}