/*
 * Copyright 2001-2002 Sun Microsystems, Inc.  All Rights Reserved.
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

package java.nio.charset;


/**
 * A typesafe enumeration for coding-error actions.
 *
 * <p> Instances of this class are used to specify how malformed-input and
 * unmappable-character errors are to be handled by charset <a
 * href="CharsetDecoder.html#cae">decoders</a> and <a
 * href="CharsetEncoder.html#cae">encoders</a>.  </p>
 *
 *
 * @author Mark Reinhold
 * @author JSR-51 Expert Group
 * @version %I%, %E%
 * @since 1.4
 */

public class CodingErrorAction {

    private String name;

    private CodingErrorAction(String name) {
	this.name = name;
    }

    /**
     * Action indicating that a coding error is to be handled by dropping the
     * erroneous input and resuming the coding operation.  </p>
     */
    public static final CodingErrorAction IGNORE
	= new CodingErrorAction("IGNORE");

    /**
     * Action indicating that a coding error is to be handled by dropping the
     * erroneous input, appending the coder's replacement value to the output
     * buffer, and resuming the coding operation.  </p>
     */
    public static final CodingErrorAction REPLACE
	= new CodingErrorAction("REPLACE");

    /**
     * Action indicating that a coding error is to be reported, either by
     * returning a {@link CoderResult} object or by throwing a {@link
     * CharacterCodingException}, whichever is appropriate for the method
     * implementing the coding process.
     */
    public static final CodingErrorAction REPORT
	= new CodingErrorAction("REPORT");

    /**
     * Returns a string describing this action.  </p>
     *
     * @return  A descriptive string
     */
    public String toString() {
	return name;
    }

}