/*
 * Copyright 2003-2005 Sun Microsystems, Inc.  All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.  Sun designates this
 * particular file as subject to the "Classpath" exception as provided
 * by Sun in the LICENSE file that accompanied this code.
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

package java.util;

/**
 * Unchecked exception thrown when a character with an invalid Unicode code
 * point as defined by {@link Character#isValidCodePoint} is passed to the
 * {@link Formatter}.
 *
 * <p> Unless otherwise specified, passing a <tt>null</tt> argument to any
 * method or constructor in this class will cause a {@link
 * NullPointerException} to be thrown.
 *
 * @version 	%I%, %G%
 * @since 1.5
 */
public class IllegalFormatCodePointException extends IllegalFormatException {

    private static final long serialVersionUID = 19080630L;

    private int c;

    /**
     * Constructs an instance of this class with the specified illegal code
     * point as defined by {@link Character#isValidCodePoint}.
     *
     * @param  c
     *         The illegal Unicode code point
     */
    public IllegalFormatCodePointException(int c) {
	this.c = c;
    }

    /**
     * Returns the illegal code point as defined by {@link
     * Character#isValidCodePoint}.
     *
     * @return  The illegal Unicode code point
     */
    public int getCodePoint() {
	return c;
    }

    public String getMessage() {
	return String.format("Code point = %#x", c);
    }
}