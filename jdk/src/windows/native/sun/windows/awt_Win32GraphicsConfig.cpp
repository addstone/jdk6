/*
 * Copyright 1999-2003 Sun Microsystems, Inc.  All Rights Reserved.
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

#include <windows.h>
#include "awt.h"
#include <sun_awt_Win32GraphicsConfig.h>
#include "awt_Win32GraphicsConfig.h"
#include "awt_Canvas.h"
#include "awt_Win32GraphicsDevice.h"

//Info for building a ColorModel
#include "java_awt_image_DataBuffer.h"


//Local utility functions
static int shiftToMask(int numBits, int shift);

/*
 * AwtWin32GraphicsConfig fields
 */

jfieldID AwtWin32GraphicsConfig::win32GCVisualID;

/*
 * Class:     sun_awt_Win32GraphicsConfig
 * Method:    initIDs
 * Signature: ()V
 */

JNIEXPORT void JNICALL
Java_sun_awt_Win32GraphicsConfig_initIDs
    (JNIEnv *env, jclass thisCls)
{
    TRY;
    AwtWin32GraphicsConfig::win32GCVisualID = env->GetFieldID(thisCls,
	 "visual", "I");
    DASSERT(AwtWin32GraphicsConfig::win32GCVisualID);
	CATCH_BAD_ALLOC;
}

/*
 *  shiftToMask:
 *  This function converts between cXXXBits and cXXXShift
 *  fields in the Windows GDI PIXELFORMATDESCRIPTOR and the mask
 *  fields passed to the DirectColorModel constructor.
 */
inline int shiftToMask(int numBits, int shift) {
    int mask = 0xFFFFFFFF;

    //Shift in numBits 0s
    mask = mask << numBits;
    mask = ~mask;
    //shift left by value of shift
    mask = mask << shift;
    return mask;
}

/*
 * Class:     sun_awt_Win32GraphicsConfig
 * Method:    getBounds
 * Signature: ()Ljava/awt/Rectangle
 */
JNIEXPORT jobject JNICALL
    Java_sun_awt_Win32GraphicsConfig_getBounds(JNIEnv *env, jobject thisobj,
                                               jint screen)
{
    jclass clazz;
    jmethodID mid;
    jobject bounds = NULL;
    
    clazz = env->FindClass("java/awt/Rectangle");
    mid = env->GetMethodID(clazz, "<init>", "(IIII)V");
    if (mid != 0) {
        RECT    rRW     = {0,0,0,0};
        if( TRUE == ::MonitorBounds(AwtWin32GraphicsDevice::GetMonitor(screen), &rRW) ) {
            bounds = env->NewObject(clazz, mid,
                                    rRW.left, rRW.top,
                                    rRW.right - rRW.left,
                                    rRW.bottom - rRW.top);
        }
        else {
            // 4910760 - don't return a null bounds, return the bounds of the
            // primary screen
            bounds = env->NewObject(clazz, mid,
                                    0, 0,
                                    ::GetSystemMetrics(SM_CXSCREEN),
                                    ::GetSystemMetrics(SM_CYSCREEN));
        }
        if (safe_ExceptionOccurred(env)) {
           return 0;
        }
    }
    return bounds;
}