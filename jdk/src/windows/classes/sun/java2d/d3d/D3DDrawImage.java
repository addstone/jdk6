/*
 * Copyright 2005-2007 Sun Microsystems, Inc.  All Rights Reserved.
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

package sun.java2d.d3d;

import java.awt.Color;
import java.awt.Image;
import java.awt.geom.AffineTransform;
import java.awt.image.AffineTransformOp;
import sun.java2d.SunGraphics2D;
import sun.java2d.SurfaceData;
import sun.java2d.loops.SurfaceType;
import sun.java2d.loops.TransformBlit;
import sun.java2d.pipe.DrawImage;

public class D3DDrawImage extends DrawImage {
    protected void renderImageXform(SunGraphics2D sg, Image img,
				    AffineTransform tx, int interpType,
				    int sx1, int sy1, int sx2, int sy2,
				    Color bgColor)
    {
        // punt to the MediaLib-based transformImage() in the superclass if:
        //     - bicubic interpolation is specified
        //     - a background color is specified and will be used
        //     - the source surface is not a texture
        //     - an appropriate TransformBlit primitive could not be found
        if (interpType != AffineTransformOp.TYPE_BICUBIC) {
            SurfaceData dstData = sg.surfaceData;
	    SurfaceData srcData =
		dstData.getSourceSurfaceData(img,
                                             sg.TRANSFORM_GENERIC,
                                             sg.imageComp,
                                             bgColor);

	    if (srcData != null &&
		!isBgOperation(srcData, bgColor) &&
		srcData.getSurfaceType() == D3DSurfaceData.D3DTexture)
	    {
                SurfaceType srcType = srcData.getSurfaceType();
                SurfaceType dstType = dstData.getSurfaceType();
		TransformBlit blit = TransformBlit.getFromCache(srcType,
								sg.imageComp,
								dstType);

		if (blit != null) {
		    blit.Transform(srcData, dstData,
				   sg.composite, sg.getCompClip(),
				   tx, interpType,
				   sx1, sy1, 0, 0, sx2-sx1, sy2-sy1);
		    return;
		}
	    }
	}

	super.renderImageXform(sg, img, tx, interpType,
			       sx1, sy1, sx2, sy2, bgColor);
    }
}
