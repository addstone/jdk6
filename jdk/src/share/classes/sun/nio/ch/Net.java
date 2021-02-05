/*
 * Copyright (c) 2000, 2011, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.  Oracle designates this
 * particular file as subject to the "Classpath" exception as provided
 * by Oracle in the LICENSE file that accompanied this code.
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
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 */

package sun.nio.ch;

import java.io.*;
import java.lang.reflect.*;
import java.net.*;
import java.nio.channels.*;
import java.security.AccessController;
import java.security.PrivilegedExceptionAction;

class Net {                                             // package-private

    private Net() { }


    // Value of jdk.net.revealLocalAddress
    private static boolean revealLocalAddress;

    // True if jdk.net.revealLocalAddress had been read
    private static volatile boolean propRevealLocalAddress;

    // set to true if exclusive binding is on for Windows
    private static final boolean exclusiveBind;

    static {
        int availLevel = isExclusiveBindAvailable();
        if (availLevel >= 0) {
            String exclBindProp =
                java.security.AccessController.doPrivileged(
                    new java.security.PrivilegedAction<String>() {
                        public String run() {
                            return System.getProperty(
                                    "sun.net.useExclusiveBind");
                        }
                    });
            if (exclBindProp != null) {
                exclusiveBind = exclBindProp.length() == 0 ?
                        true : Boolean.parseBoolean(exclBindProp);
            } else if (availLevel == 1) {
                exclusiveBind = true;
            } else {
                exclusiveBind = false;
            }
        } else {
            exclusiveBind = false;
        }
    }

    // -- Miscellaneous utilities --

    /**
     * Returns true if exclusive binding is on
     */
    static boolean useExclusiveBind() {
        return exclusiveBind;
    }

    static InetSocketAddress checkAddress(SocketAddress sa) {
        if (sa == null)
            throw new IllegalArgumentException();
        if (!(sa instanceof InetSocketAddress))
            throw new UnsupportedAddressTypeException(); // ## needs arg
        InetSocketAddress isa = (InetSocketAddress)sa;
        if (isa.isUnresolved())
            throw new UnresolvedAddressException(); // ## needs arg
        InetAddress addr = isa.getAddress();
        if (!(addr instanceof Inet4Address || addr instanceof Inet6Address))
            throw new IllegalArgumentException("Invalid address type");
        return isa;
    }

    static InetSocketAddress asInetSocketAddress(SocketAddress sa) {
        if (!(sa instanceof InetSocketAddress))
            throw new UnsupportedAddressTypeException();
        return (InetSocketAddress)sa;
    }

    static void translateToSocketException(Exception x)
        throws SocketException
    {
        if (x instanceof SocketException)
            throw (SocketException)x;
        Exception nx = x;
        if (x instanceof ClosedChannelException)
            nx = new SocketException("Socket is closed");
        else if (x instanceof AlreadyBoundException)
            nx = new SocketException("Already bound");
        else if (x instanceof NotYetBoundException)
            nx = new SocketException("Socket is not bound yet");
        else if (x instanceof UnsupportedAddressTypeException)
            nx = new SocketException("Unsupported address type");
        else if (x instanceof UnresolvedAddressException) {
            nx = new SocketException("Unresolved address");
        }
        if (nx != x)
            nx.initCause(x);

        if (nx instanceof SocketException)
            throw (SocketException)nx;
        else if (nx instanceof RuntimeException)
            throw (RuntimeException)nx;
        else
            throw new Error("Untranslated exception", nx);
    }

    static void translateException(Exception x,
                                   boolean unknownHostForUnresolved)
        throws IOException
    {
        if (x instanceof IOException)
            throw (IOException)x;
        // Throw UnknownHostException from here since it cannot
        // be thrown as a SocketException
        if (unknownHostForUnresolved &&
            (x instanceof UnresolvedAddressException))
        {
             throw new UnknownHostException();
        }
        translateToSocketException(x);
    }

    static void translateException(Exception x)
        throws IOException
    {
        translateException(x, false);
    }


    // -- Socket operations --

    static FileDescriptor socket(boolean stream) throws IOException {
        return IOUtil.newFD(socket0(stream, false));
    }

    static FileDescriptor serverSocket(boolean stream) {
        return IOUtil.newFD(socket0(stream, true));
    }

    /*
     * Returns 1 for Windows versions that support exclusive binding by default, 0
     * for those that do not, and -1 for Solaris/Linux/Mac OS
     */
    private static native int isExclusiveBindAvailable();

    // Due to oddities SO_REUSEADDR on windows reuse is ignored
    private static native int socket0(boolean stream, boolean reuse);

    static void bind(FileDescriptor fd, InetAddress addr, int port)
        throws IOException {
        bind0(fd, exclusiveBind, addr, port);
    }

    private static native void bind0(FileDescriptor fd, boolean useExclBind, InetAddress addr, int port)
        throws IOException;

    static native int connect(FileDescriptor fd,
                              InetAddress remote,
                              int remotePort,
                              int trafficClass)
        throws IOException;


    private static native int localPort(FileDescriptor fd)
        throws IOException;

    private static native InetAddress localInetAddress(FileDescriptor fd)
        throws IOException;

    static InetSocketAddress localAddress(FileDescriptor fd) {
        try {
            return new InetSocketAddress(localInetAddress(fd),
                                         localPort(fd));
        } catch (IOException x) {
            throw new Error(x);         // Can't happen
        }
    }

    static int localPortNumber(FileDescriptor fd) {
        try {
            return localPort(fd);
        } catch (IOException x) {
            throw new Error(x);         // Can't happen
        }
    }

    private static native int getIntOption0(FileDescriptor fd, int opt)
        throws IOException;

    static int getIntOption(FileDescriptor fd, int opt)
        throws IOException
    {
        return getIntOption0(fd, opt);
    }


    private static native void setIntOption0(FileDescriptor fd,
                                             int opt, int arg)
        throws IOException;

    static void setIntOption(FileDescriptor fd, int opt, int arg)
        throws IOException
    {
        setIntOption0(fd, opt, arg);
    }

    private static native void initIDs();

    static {
        Util.load();
        initIDs();
    }

     /**
     * Returns the local address after performing a SecurityManager#checkConnect.
     */
    static InetSocketAddress getRevealedLocalAddress(InetSocketAddress addr) {
        SecurityManager sm = System.getSecurityManager();
        if (addr == null || sm == null)
            return addr;

        if (!getRevealLocalAddress()) {
            // Return loopback address only if security check fails
            try{
                sm.checkConnect(addr.getAddress().getHostAddress(), -1);
                //Security check passed
            } catch (SecurityException e) {
                //Return loopback address
                addr = getLoopbackAddress(addr.getPort());
            }
        }
        return addr;
    }

    static String getRevealedLocalAddressAsString(InetSocketAddress addr) {
        if (!getRevealLocalAddress() && System.getSecurityManager() != null)
            addr = getLoopbackAddress(addr.getPort());
        return addr.toString();
    }

    private static boolean getRevealLocalAddress() {
        if (!propRevealLocalAddress) {
            try {
                revealLocalAddress = Boolean.parseBoolean(
                      AccessController.doPrivileged(
                          new PrivilegedExceptionAction<String>() {
                              public String run() {
                                  return System.getProperty(
                                      "jdk.net.revealLocalAddress");
                              }
                          }));

            } catch (Exception e) {
                // revealLocalAddress is false
            }
            propRevealLocalAddress = true;
        }
        return revealLocalAddress;
    }

    private static InetSocketAddress getLoopbackAddress(int port) {
	try {
	    return new InetSocketAddress(InetAddress.getByName(null),
					 port);
	} catch (UnknownHostException e) { 
	    throw new InternalError("Shouldn't reach here.");
	}
    }

}
