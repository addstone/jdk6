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

package sun.tools.jstat;

import java.util.*;

/**
 * A class to represent the format for a column of data.
 *
 * @author Brian Doherty
 * @version %I%, %G%
 * @since 1.5
 */
public class ColumnFormat extends OptionFormat {
    private int number;
    private int width;
    private Alignment align = Alignment.CENTER;
    private Scale scale = Scale.RAW;
    private String format;
    private String header;
    private Expression expression;
    private Object previousValue;

    public ColumnFormat(int number) {
        super("Column" + number);
        this.number = number;
    }

    /*
     * method to apply various validation rules to the ColumnFormat object.
     */
    public void validate() throws ParserException {

        // if we allow column spanning, then this method must change. it
        // should allow null data statments

        if (expression == null) {
            // current policy is that a data statment must be specified
            throw new ParserException("Missing data statement in column " + number);
        }
        if (header == null) {
            // current policy is that if a header is not specified, then we
            // will use the last component of the name as the header and
            // insert the default anchor characters for center alignment..
            throw new ParserException("Missing header statement in column " + number);
        }
        if (format == null) {
            // if no formating is specified, then the format is set to output
            // the raw data.
            format="0";
        }
    }

    public void setWidth(int width) {
        this.width = width;
    }

    public void setAlignment(Alignment align) {
        this.align = align;
    }

    public void setScale(Scale scale) {
        this.scale = scale;
    }

    public void setFormat(String format) {
        this.format = format;
    }

    public void setHeader(String header) {
        this.header = header;
    }

    public String getHeader() {
        return header;
    }

    public String getFormat() {
        return format;
    }

    public int getWidth() {
        return width;
    }

    public Alignment getAlignment() {
        return align;
    }

    public Scale getScale() {
        return scale;
    }

    public Expression getExpression() {
        return expression;
    }

    public void setExpression(Expression e) {
        this.expression = e;
    }

    public void setPreviousValue(Object o) {
        this.previousValue = o;
    }

    public Object getPreviousValue() {
        return previousValue;
    }

    public void printFormat(int indentLevel) {
        String indentAmount = "  ";

        StringBuilder indent = new StringBuilder("");
        for (int j = 0; j < indentLevel; j++) {
            indent.append(indentAmount);
        }

        System.out.println(indent + name + " {");
        System.out.println(indent + indentAmount + "name=" + name
                + ";data=" + expression.toString() + ";header=" + header
                + ";format=" + format + ";width=" + width
                + ";scale=" + scale.toString() + ";align=" + align.toString());

        for (Iterator i = children.iterator();  i.hasNext(); /* empty */) {
            OptionFormat of = (OptionFormat)i.next();
            of.printFormat(indentLevel+1);
        }

        System.out.println(indent + "}");
    }

    public String getValue() {
        return null;
    }
}