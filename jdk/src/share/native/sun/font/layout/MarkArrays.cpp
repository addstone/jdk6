/*
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
 *
 */

/*
 * @(#)MarkArrays.cpp	1.9 06/12/13
 *
 * (C) Copyright IBM Corp. 1998-2004 - All Rights Reserved
 *
 */

#include "LETypes.h"
#include "LEFontInstance.h"
#include "OpenTypeTables.h"
#include "AnchorTables.h"
#include "MarkArrays.h"
#include "LESwaps.h"

le_int32 MarkArray::getMarkClass(LEGlyphID glyphID, le_int32 coverageIndex, const LEFontInstance *fontInstance,
                              LEPoint &anchor) const
{
    le_int32 markClass = -1;

    if (coverageIndex >= 0) {
        le_uint16 mCount = SWAPW(markCount);

        if (coverageIndex < mCount) {
            const MarkRecord *markRecord = &markRecordArray[coverageIndex];
            Offset anchorTableOffset = SWAPW(markRecord->markAnchorTableOffset);
            const AnchorTable *anchorTable = (AnchorTable *) ((char *) this + anchorTableOffset);

            anchorTable->getAnchor(glyphID, fontInstance, anchor);
            markClass = SWAPW(markRecord->markClass);
        }

        // XXXX If we get here, the table is mal-formed
    }

    return markClass;
}