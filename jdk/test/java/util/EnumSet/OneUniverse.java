/*
 * Copyright 2006-2007 Sun Microsystems, Inc.  All Rights Reserved.
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
 * @bug	    6276988
 * @summary All enum constants in a class should share a single "universe".
 */

import java.lang.reflect.Field;
import java.math.RoundingMode;
import java.util.EnumSet;

public class OneUniverse {

    private static final Field universeField;
    
    static {
        try {
	    universeField = EnumSet.class.getDeclaredField("universe");
        } catch (NoSuchFieldException e) {
	    throw new AssertionError(e);
        }
        universeField.setAccessible(true);
    }
    
    public static void main(String... args) {

	EnumSet<RoundingMode> noneSet = EnumSet.noneOf(RoundingMode.class);
	EnumSet<RoundingMode> allSet  = EnumSet.allOf(RoundingMode.class);
      
	if (getUniverse(noneSet) != getUniverse(allSet)) {
	    throw new AssertionError(
		    "Multiple universes detected.  Inform the bridge!");
	}
    }
    
    private static <E extends Enum<E>> Enum<E>[] getUniverse(EnumSet<E> set) {
	try {
	    return (Enum<E>[]) universeField.get(set);
	} catch (IllegalAccessException e) {
	    throw new AssertionError(e);
        }
    }
}