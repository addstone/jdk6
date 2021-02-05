/*
 * Copyright 2004 Sun Microsystems, Inc.  All Rights Reserved.
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

package sun.jvmstat.perfdata.monitor;

import sun.jvmstat.monitor.*;
import sun.management.counter.Units;
import sun.management.counter.Variability;
import java.nio.IntBuffer;

/**
 * Class for monitoring a PerfData Integer instrument.
 *
 * @author Brian Doherty
 * @version %I%, %G%
 * @since 1.5
 */
public class PerfIntegerMonitor extends AbstractMonitor
                                implements IntegerMonitor {

    /**
     * The buffer containing the data for the integer instrument.
     */
    IntBuffer ib;

    /**
     * Constructor to create an IntegerMonitor object for the integer
     *  instrument represented by the data in the given buffer.
     *
     * @param name the name of the integer instrument
     * @param u the units of measure attribute
     * @param v the variability attribute
     * @param supported support level indicator
     * @param ib the buffer containing the integer instrument data.
     */
    public PerfIntegerMonitor(String name, Units u, Variability v,
                              boolean supported, IntBuffer ib) {
        super(name, u, v, supported);
        this.ib = ib;
    }

    /**
     * {@inheritDoc}
     * The object returned contains an Integer object containing the
     * current value of the IntegerInstrument.
     *
     * @return Object - the current value of the the IntegerInstrument. The
     *                   return type is guaranteed to be of type Integer.
     */
    public Object getValue() {
        return new Integer(ib.get(0));
    }

    /**
     * Return the current value of the IntegerInstrument as an int.
     *
     * @return int - a the current value of the IntegerInstrument.
     */
    public int intValue() {
        return ib.get(0);
    }
}
