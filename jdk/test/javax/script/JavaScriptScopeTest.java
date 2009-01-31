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
 */

/*
 * @test
 * @bug 6346733
 * @summary Verify that independent Bindings instances don't
 * get affected by default scope assignments. Also, verify
 * that script globals can be created and accessed from Java 
 * as well as JavaScript.
 */

import javax.script.*;

public class JavaScriptScopeTest {

	public static void main(String[] args) throws Exception {
	    ScriptEngineManager manager = new ScriptEngineManager();
	    ScriptEngine jsengine = manager.getEngineByName("js");
            if (jsengine == null) {
                throw new RuntimeException("no js engine found");
            }
            jsengine.eval("var v = 'hello';");
            // Create a new scope
            Bindings b = jsengine.createBindings();
            // b is newly created scope. We don't expect 'v' there.
            // we expect b to be empty...
            if (b.keySet().size() != 0) {
                throw new RuntimeException("no variables expected in new scope");
            }

            // verify that we can create new variable from Java
            jsengine.put("fromJava", "hello world");
            // below should execute without problems..
            jsengine.eval(" if (fromJava != 'hello world') throw 'unexpected'");

            // verify that script globals are exposed to Java
            // we have created 'v' and 'fromJava' already.
            if (! jsengine.get("v").equals("hello")) {
                throw new RuntimeException("unexpected value of 'v'");
            }

            if (! jsengine.get("fromJava").equals("hello world")) {
                throw new RuntimeException("unexpected value of 'fromJava'");
            }
	}
}