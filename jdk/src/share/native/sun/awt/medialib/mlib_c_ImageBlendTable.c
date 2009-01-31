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
  
#ifdef __SUNPRO_C
#pragma ident   "@(#)mlib_c_ImageBlendTable.c	1.12    98/06/26 SMI"
#endif /* __SUNPRO_C */

#include <mlib_image.h>
#include "mlib_c_ImageBlendTable.h"

const mlib_f32 mlib_c_blend_u8[] = {
    0.0f,   1.0f,   2.0f,   3.0f,   4.0f,   5.0f,   6.0f,   7.0f,
    8.0f,   9.0f,  10.0f,  11.0f,  12.0f,  13.0f,  14.0f,  15.0f,
   16.0f,  17.0f,  18.0f,  19.0f,  20.0f,  21.0f,  22.0f,  23.0f,
   24.0f,  25.0f,  26.0f,  27.0f,  28.0f,  29.0f,  30.0f,  31.0f,
   32.0f,  33.0f,  34.0f,  35.0f,  36.0f,  37.0f,  38.0f,  39.0f,
   40.0f,  41.0f,  42.0f,  43.0f,  44.0f,  45.0f,  46.0f,  47.0f,
   48.0f,  49.0f,  50.0f,  51.0f,  52.0f,  53.0f,  54.0f,  55.0f,
   56.0f,  57.0f,  58.0f,  59.0f,  60.0f,  61.0f,  62.0f,  63.0f,
   64.0f,  65.0f,  66.0f,  67.0f,  68.0f,  69.0f,  70.0f,  71.0f,
   72.0f,  73.0f,  74.0f,  75.0f,  76.0f,  77.0f,  78.0f,  79.0f,
   80.0f,  81.0f,  82.0f,  83.0f,  84.0f,  85.0f,  86.0f,  87.0f,
   88.0f,  89.0f,  90.0f,  91.0f,  92.0f,  93.0f,  94.0f,  95.0f,
   96.0f,  97.0f,  98.0f,  99.0f, 100.0f, 101.0f, 102.0f, 103.0f,
  104.0f, 105.0f, 106.0f, 107.0f, 108.0f, 109.0f, 110.0f, 111.0f,
  112.0f, 113.0f, 114.0f, 115.0f, 116.0f, 117.0f, 118.0f, 119.0f,
  120.0f, 121.0f, 122.0f, 123.0f, 124.0f, 125.0f, 126.0f, 127.0f,
  128.0f, 129.0f, 130.0f, 131.0f, 132.0f, 133.0f, 134.0f, 135.0f,
  136.0f, 137.0f, 138.0f, 139.0f, 140.0f, 141.0f, 142.0f, 143.0f,
  144.0f, 145.0f, 146.0f, 147.0f, 148.0f, 149.0f, 150.0f, 151.0f,
  152.0f, 153.0f, 154.0f, 155.0f, 156.0f, 157.0f, 158.0f, 159.0f,
  160.0f, 161.0f, 162.0f, 163.0f, 164.0f, 165.0f, 166.0f, 167.0f,
  168.0f, 169.0f, 170.0f, 171.0f, 172.0f, 173.0f, 174.0f, 175.0f,
  176.0f, 177.0f, 178.0f, 179.0f, 180.0f, 181.0f, 182.0f, 183.0f,
  184.0f, 185.0f, 186.0f, 187.0f, 188.0f, 189.0f, 190.0f, 191.0f,
  192.0f, 193.0f, 194.0f, 195.0f, 196.0f, 197.0f, 198.0f, 199.0f,
  200.0f, 201.0f, 202.0f, 203.0f, 204.0f, 205.0f, 206.0f, 207.0f,
  208.0f, 209.0f, 210.0f, 211.0f, 212.0f, 213.0f, 214.0f, 215.0f,
  216.0f, 217.0f, 218.0f, 219.0f, 220.0f, 221.0f, 222.0f, 223.0f,
  224.0f, 225.0f, 226.0f, 227.0f, 228.0f, 229.0f, 230.0f, 231.0f,
  232.0f, 233.0f, 234.0f, 235.0f, 236.0f, 237.0f, 238.0f, 239.0f,
  240.0f, 241.0f, 242.0f, 243.0f, 244.0f, 245.0f, 246.0f, 247.0f,
  248.0f, 249.0f, 250.0f, 251.0f, 252.0f, 253.0f, 254.0f, 255.0f,
  256.0f
};

const mlib_f32 mlib_c_blend_Q8[] = {
  0.00000000f, 0.00390625f, 0.00781250f, 0.01171875f,
  0.01562500f, 0.01953125f, 0.02343750f, 0.02734375f,
  0.03125000f, 0.03515625f, 0.03906250f, 0.04296875f,
  0.04687500f, 0.05078125f, 0.05468750f, 0.05859375f,
  0.06250000f, 0.06640625f, 0.07031250f, 0.07421875f,
  0.07812500f, 0.08203125f, 0.08593750f, 0.08984375f,
  0.09375000f, 0.09765625f, 0.10156250f, 0.10546875f,
  0.10937500f, 0.11328125f, 0.11718750f, 0.12109375f,
  0.12500000f, 0.12890625f, 0.13281250f, 0.13671875f,
  0.14062500f, 0.14453125f, 0.14843750f, 0.15234375f,
  0.15625000f, 0.16015625f, 0.16406250f, 0.16796875f,
  0.17187500f, 0.17578125f, 0.17968750f, 0.18359375f,
  0.18750000f, 0.19140625f, 0.19531250f, 0.19921875f,
  0.20312500f, 0.20703125f, 0.21093750f, 0.21484375f,
  0.21875000f, 0.22265625f, 0.22656250f, 0.23046875f,
  0.23437500f, 0.23828125f, 0.24218750f, 0.24609375f,
  0.25000000f, 0.25390625f, 0.25781250f, 0.26171875f,
  0.26562500f, 0.26953125f, 0.27343750f, 0.27734375f,
  0.28125000f, 0.28515625f, 0.28906250f, 0.29296875f,
  0.29687500f, 0.30078125f, 0.30468750f, 0.30859375f,
  0.31250000f, 0.31640625f, 0.32031250f, 0.32421875f,
  0.32812500f, 0.33203125f, 0.33593750f, 0.33984375f,
  0.34375000f, 0.34765625f, 0.35156250f, 0.35546875f,
  0.35937500f, 0.36328125f, 0.36718750f, 0.37109375f,
  0.37500000f, 0.37890625f, 0.38281250f, 0.38671875f,
  0.39062500f, 0.39453125f, 0.39843750f, 0.40234375f,
  0.40625000f, 0.41015625f, 0.41406250f, 0.41796875f,
  0.42187500f, 0.42578125f, 0.42968750f, 0.43359375f,
  0.43750000f, 0.44140625f, 0.44531250f, 0.44921875f,
  0.45312500f, 0.45703125f, 0.46093750f, 0.46484375f,
  0.46875000f, 0.47265625f, 0.47656250f, 0.48046875f,
  0.48437500f, 0.48828125f, 0.49218750f, 0.49609375f,
  0.50000000f, 0.50390625f, 0.50781250f, 0.51171875f,
  0.51562500f, 0.51953125f, 0.52343750f, 0.52734375f,
  0.53125000f, 0.53515625f, 0.53906250f, 0.54296875f,
  0.54687500f, 0.55078125f, 0.55468750f, 0.55859375f,
  0.56250000f, 0.56640625f, 0.57031250f, 0.57421875f,
  0.57812500f, 0.58203125f, 0.58593750f, 0.58984375f,
  0.59375000f, 0.59765625f, 0.60156250f, 0.60546875f,
  0.60937500f, 0.61328125f, 0.61718750f, 0.62109375f,
  0.62500000f, 0.62890625f, 0.63281250f, 0.63671875f,
  0.64062500f, 0.64453125f, 0.64843750f, 0.65234375f,
  0.65625000f, 0.66015625f, 0.66406250f, 0.66796875f,
  0.67187500f, 0.67578125f, 0.67968750f, 0.68359375f,
  0.68750000f, 0.69140625f, 0.69531250f, 0.69921875f,
  0.70312500f, 0.70703125f, 0.71093750f, 0.71484375f,
  0.71875000f, 0.72265625f, 0.72656250f, 0.73046875f,
  0.73437500f, 0.73828125f, 0.74218750f, 0.74609375f,
  0.75000000f, 0.75390625f, 0.75781250f, 0.76171875f,
  0.76562500f, 0.76953125f, 0.77343750f, 0.77734375f,
  0.78125000f, 0.78515625f, 0.78906250f, 0.79296875f,
  0.79687500f, 0.80078125f, 0.80468750f, 0.80859375f,
  0.81250000f, 0.81640625f, 0.82031250f, 0.82421875f,
  0.82812500f, 0.83203125f, 0.83593750f, 0.83984375f,
  0.84375000f, 0.84765625f, 0.85156250f, 0.85546875f,
  0.85937500f, 0.86328125f, 0.86718750f, 0.87109375f,
  0.87500000f, 0.87890625f, 0.88281250f, 0.88671875f,
  0.89062500f, 0.89453125f, 0.89843750f, 0.90234375f,
  0.90625000f, 0.91015625f, 0.91406250f, 0.91796875f,
  0.92187500f, 0.92578125f, 0.92968750f, 0.93359375f,
  0.93750000f, 0.94140625f, 0.94531250f, 0.94921875f,
  0.95312500f, 0.95703125f, 0.96093750f, 0.96484375f,
  0.96875000f, 0.97265625f, 0.97656250f, 0.98046875f,
  0.98437500f, 0.98828125f, 0.99218750f, 0.99609375f,
  1.00000000f, 1.00390625f, 1.00781250f, 1.01171875f,
  1.01562500f, 1.01953125f, 1.02343750f, 1.02734375f,
  1.03125000f, 1.03515625f, 1.03906250f, 1.04296875f,
  1.04687500f, 1.05078125f, 1.05468750f, 1.05859375f,
  1.06250000f, 1.06640625f, 1.07031250f, 1.07421875f,
  1.07812500f, 1.08203125f, 1.08593750f, 1.08984375f,
  1.09375000f, 1.09765625f, 1.10156250f, 1.10546875f,
  1.10937500f, 1.11328125f, 1.11718750f, 1.12109375f,
  1.12500000f, 1.12890625f, 1.13281250f, 1.13671875f,
  1.14062500f, 1.14453125f, 1.14843750f, 1.15234375f,
  1.15625000f, 1.16015625f, 1.16406250f, 1.16796875f,
  1.17187500f, 1.17578125f, 1.17968750f, 1.18359375f,
  1.18750000f, 1.19140625f, 1.19531250f, 1.19921875f,
  1.20312500f, 1.20703125f, 1.21093750f, 1.21484375f,
  1.21875000f, 1.22265625f, 1.22656250f, 1.23046875f,
  1.23437500f, 1.23828125f, 1.24218750f, 1.24609375f,
  1.25000000f, 1.25390625f, 1.25781250f, 1.26171875f,
  1.26562500f, 1.26953125f, 1.27343750f, 1.27734375f,
  1.28125000f, 1.28515625f, 1.28906250f, 1.29296875f,
  1.29687500f, 1.30078125f, 1.30468750f, 1.30859375f,
  1.31250000f, 1.31640625f, 1.32031250f, 1.32421875f,
  1.32812500f, 1.33203125f, 1.33593750f, 1.33984375f,
  1.34375000f, 1.34765625f, 1.35156250f, 1.35546875f,
  1.35937500f, 1.36328125f, 1.36718750f, 1.37109375f,
  1.37500000f, 1.37890625f, 1.38281250f, 1.38671875f,
  1.39062500f, 1.39453125f, 1.39843750f, 1.40234375f,
  1.40625000f, 1.41015625f, 1.41406250f, 1.41796875f,
  1.42187500f, 1.42578125f, 1.42968750f, 1.43359375f,
  1.43750000f, 1.44140625f, 1.44531250f, 1.44921875f,
  1.45312500f, 1.45703125f, 1.46093750f, 1.46484375f,
  1.46875000f, 1.47265625f, 1.47656250f, 1.48046875f,
  1.48437500f, 1.48828125f, 1.49218750f, 1.49609375f,
  1.50000000f, 1.50390625f, 1.50781250f, 1.51171875f,
  1.51562500f, 1.51953125f, 1.52343750f, 1.52734375f,
  1.53125000f, 1.53515625f, 1.53906250f, 1.54296875f,
  1.54687500f, 1.55078125f, 1.55468750f, 1.55859375f,
  1.56250000f, 1.56640625f, 1.57031250f, 1.57421875f,
  1.57812500f, 1.58203125f, 1.58593750f, 1.58984375f,
  1.59375000f, 1.59765625f, 1.60156250f, 1.60546875f,
  1.60937500f, 1.61328125f, 1.61718750f, 1.62109375f,
  1.62500000f, 1.62890625f, 1.63281250f, 1.63671875f,
  1.64062500f, 1.64453125f, 1.64843750f, 1.65234375f,
  1.65625000f, 1.66015625f, 1.66406250f, 1.66796875f,
  1.67187500f, 1.67578125f, 1.67968750f, 1.68359375f,
  1.68750000f, 1.69140625f, 1.69531250f, 1.69921875f,
  1.70312500f, 1.70703125f, 1.71093750f, 1.71484375f,
  1.71875000f, 1.72265625f, 1.72656250f, 1.73046875f,
  1.73437500f, 1.73828125f, 1.74218750f, 1.74609375f,
  1.75000000f, 1.75390625f, 1.75781250f, 1.76171875f,
  1.76562500f, 1.76953125f, 1.77343750f, 1.77734375f,
  1.78125000f, 1.78515625f, 1.78906250f, 1.79296875f,
  1.79687500f, 1.80078125f, 1.80468750f, 1.80859375f,
  1.81250000f, 1.81640625f, 1.82031250f, 1.82421875f,
  1.82812500f, 1.83203125f, 1.83593750f, 1.83984375f,
  1.84375000f, 1.84765625f, 1.85156250f, 1.85546875f,
  1.85937500f, 1.86328125f, 1.86718750f, 1.87109375f,
  1.87500000f, 1.87890625f, 1.88281250f, 1.88671875f,
  1.89062500f, 1.89453125f, 1.89843750f, 1.90234375f,
  1.90625000f, 1.91015625f, 1.91406250f, 1.91796875f,
  1.92187500f, 1.92578125f, 1.92968750f, 1.93359375f,
  1.93750000f, 1.94140625f, 1.94531250f, 1.94921875f,
  1.95312500f, 1.95703125f, 1.96093750f, 1.96484375f,
  1.96875000f, 1.97265625f, 1.97656250f, 1.98046875f,
  1.98437500f, 1.98828125f, 1.99218750f, 1.99609375f,
  2.00000000f
};

const mlib_f32 mlib_c_blend_u8_sat[] = {
           0.0f,    8388608.0f,   16777216.0f,   25165824.0f,
    33554432.0f,   41943040.0f,   50331648.0f,   58720256.0f,
    67108864.0f,   75497472.0f,   83886080.0f,   92274688.0f,
   100663296.0f,  109051904.0f,  117440512.0f,  125829120.0f,
   134217728.0f,  142606336.0f,  150994944.0f,  159383552.0f,
   167772160.0f,  176160768.0f,  184549376.0f,  192937984.0f,
   201326592.0f,  209715200.0f,  218103808.0f,  226492416.0f,
   234881024.0f,  243269632.0f,  251658240.0f,  260046848.0f,
   268435456.0f,  276824064.0f,  285212672.0f,  293601280.0f,
   301989888.0f,  310378496.0f,  318767104.0f,  327155712.0f,
   335544320.0f,  343932928.0f,  352321536.0f,  360710144.0f,
   369098752.0f,  377487360.0f,  385875968.0f,  394264576.0f,
   402653184.0f,  411041792.0f,  419430400.0f,  427819008.0f,
   436207616.0f,  444596224.0f,  452984832.0f,  461373440.0f,
   469762048.0f,  478150656.0f,  486539264.0f,  494927872.0f,
   503316480.0f,  511705088.0f,  520093696.0f,  528482304.0f,
   536870912.0f,  545259520.0f,  553648128.0f,  562036736.0f,
   570425344.0f,  578813952.0f,  587202560.0f,  595591168.0f,
   603979776.0f,  612368384.0f,  620756992.0f,  629145600.0f,
   637534208.0f,  645922816.0f,  654311424.0f,  662700032.0f,
   671088640.0f,  679477248.0f,  687865856.0f,  696254464.0f,
   704643072.0f,  713031680.0f,  721420288.0f,  729808896.0f,
   738197504.0f,  746586112.0f,  754974720.0f,  763363328.0f,
   771751936.0f,  780140544.0f,  788529152.0f,  796917760.0f,
   805306368.0f,  813694976.0f,  822083584.0f,  830472192.0f,
   838860800.0f,  847249408.0f,  855638016.0f,  864026624.0f,
   872415232.0f,  880803840.0f,  889192448.0f,  897581056.0f,
   905969664.0f,  914358272.0f,  922746880.0f,  931135488.0f,
   939524096.0f,  947912704.0f,  956301312.0f,  964689920.0f,
   973078528.0f,  981467136.0f,  989855744.0f,  998244352.0f,
  1006632960.0f, 1015021568.0f, 1023410176.0f, 1031798784.0f,
  1040187392.0f, 1048576000.0f, 1056964608.0f, 1065353216.0f,
  1073741824.0f, 1082130432.0f, 1090519040.0f, 1098907648.0f,
  1107296256.0f, 1115684864.0f, 1124073472.0f, 1132462080.0f,
  1140850688.0f, 1149239296.0f, 1157627904.0f, 1166016512.0f,
  1174405120.0f, 1182793728.0f, 1191182336.0f, 1199570944.0f,
  1207959552.0f, 1216348160.0f, 1224736768.0f, 1233125376.0f,
  1241513984.0f, 1249902592.0f, 1258291200.0f, 1266679808.0f,
  1275068416.0f, 1283457024.0f, 1291845632.0f, 1300234240.0f,
  1308622848.0f, 1317011456.0f, 1325400064.0f, 1333788672.0f,
  1342177280.0f, 1350565888.0f, 1358954496.0f, 1367343104.0f,
  1375731712.0f, 1384120320.0f, 1392508928.0f, 1400897536.0f,
  1409286144.0f, 1417674752.0f, 1426063360.0f, 1434451968.0f,
  1442840576.0f, 1451229184.0f, 1459617792.0f, 1468006400.0f,
  1476395008.0f, 1484783616.0f, 1493172224.0f, 1501560832.0f,
  1509949440.0f, 1518338048.0f, 1526726656.0f, 1535115264.0f,
  1543503872.0f, 1551892480.0f, 1560281088.0f, 1568669696.0f,
  1577058304.0f, 1585446912.0f, 1593835520.0f, 1602224128.0f,
  1610612736.0f, 1619001344.0f, 1627389952.0f, 1635778560.0f,
  1644167168.0f, 1652555776.0f, 1660944384.0f, 1669332992.0f,
  1677721600.0f, 1686110208.0f, 1694498816.0f, 1702887424.0f,
  1711276032.0f, 1719664640.0f, 1728053248.0f, 1736441856.0f,
  1744830464.0f, 1753219072.0f, 1761607680.0f, 1769996288.0f,
  1778384896.0f, 1786773504.0f, 1795162112.0f, 1803550720.0f,
  1811939328.0f, 1820327936.0f, 1828716544.0f, 1837105152.0f,
  1845493760.0f, 1853882368.0f, 1862270976.0f, 1870659584.0f,
  1879048192.0f, 1887436800.0f, 1895825408.0f, 1904214016.0f,
  1912602624.0f, 1920991232.0f, 1929379840.0f, 1937768448.0f,
  1946157056.0f, 1954545664.0f, 1962934272.0f, 1971322880.0f,
  1979711488.0f, 1988100096.0f, 1996488704.0f, 2004877312.0f,
  2013265920.0f, 2021654528.0f, 2030043136.0f, 2038431744.0f,
  2046820352.0f, 2055208960.0f, 2063597568.0f, 2071986176.0f,
  2080374784.0f, 2088763392.0f, 2097152000.0f, 2105540608.0f,
  2113929216.0f, 2122317824.0f, 2130706432.0f, 2139095040.0f,
  2147483648.0f
};