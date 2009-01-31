/*
 * Copyright 2004 Sun Microsystems, Inc.  All Rights Reserved.
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
 *  
 */

package sun.jvm.hotspot.ui.tree;

import java.io.*;
import sun.jvm.hotspot.debugger.*;
import sun.jvm.hotspot.oops.*;

/** Simple wrapper for displaying bad addresses in the Inspector */

public class BadAddressTreeNodeAdapter extends FieldTreeNodeAdapter {
  private boolean usingAddress;
  private Address addr;
  private long    addrValue;

  public BadAddressTreeNodeAdapter(Address addr, FieldIdentifier id) {
    this(addr, id, false);
  }

  /** The address may be null (for address fields of structures which
      are null); the FieldIdentifier may also be null (for the root
      node). */
  public BadAddressTreeNodeAdapter(Address addr, FieldIdentifier id, boolean treeTableMode) {
    super(id, treeTableMode);
    this.addr = addr;
    usingAddress = true;
  }

  public BadAddressTreeNodeAdapter(long addr, FieldIdentifier id) {
    this(addr, id, false);
  }
  
  /** He FieldIdentifier may be null (for the root node). */
  public BadAddressTreeNodeAdapter(long addrValue, FieldIdentifier id, boolean treeTableMode) {
    super(id, treeTableMode);
    this.addrValue = addrValue;
    usingAddress = false;
  }

  public int getChildCount() {
    return 0;
  }  

  public SimpleTreeNode getChild(int index) {
    throw new RuntimeException("Should not call this");
  }

  public boolean isLeaf() {
    return true;
  }

  public int getIndexOfChild(SimpleTreeNode child) {
    throw new RuntimeException("Should not call this");
  }

  public String getValue() {
    // FIXME: should have this better factored to not have to replicate this code
    String addrString = null;
    if (usingAddress) {
      if (addr == null) {
        addrString = "0x0";
      } else {
        addrString = addr.toString();
      }
    } else {
      addrString = "0x" + Long.toHexString(addrValue);
    }
    return "** BAD ADDRESS " + addrString + " **";
  }
}