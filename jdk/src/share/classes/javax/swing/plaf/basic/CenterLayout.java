/*
 * Copyright 1998-2003 Sun Microsystems, Inc.  All Rights Reserved.
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

package javax.swing.plaf.basic;


import java.awt.*;
import java.io.*;

/**
  * Center-positioning layout manager.
  * @version %I% %G%
  * @author Tom Santos
  * @author Steve Wilson
  */
class CenterLayout implements LayoutManager, Serializable {
    public void addLayoutComponent(String name, Component comp) { }
    public void removeLayoutComponent(Component comp) { }

    public Dimension preferredLayoutSize( Container container ) {
	Component c = container.getComponent( 0 );
	if ( c != null ) {
            Dimension size = c.getPreferredSize();
            Insets insets = container.getInsets();

            return new Dimension(size.width + insets.left + insets.right,
                                 size.height + insets.top + insets.bottom);
	}
	else {
	    return new Dimension( 0, 0 );
	}
    }

    public Dimension minimumLayoutSize(Container cont) {
    	return preferredLayoutSize(cont);
    }

    public void layoutContainer(Container container) {
        if (container.getComponentCount() > 0) {
            Component c = container.getComponent(0);
            Dimension pref = c.getPreferredSize();
            int containerWidth = container.getWidth();
            int containerHeight = container.getHeight();
            Insets containerInsets = container.getInsets();

            containerWidth -= containerInsets.left +
                              containerInsets.right;
            containerHeight -= containerInsets.top +
                               containerInsets.bottom;

            int left = (containerWidth - pref.width) / 2 +
                            containerInsets.left;
            int right = (containerHeight - pref.height) / 2 +
                            containerInsets.top;

            c.setBounds(left, right, pref.width, pref.height);
        }
    }
}