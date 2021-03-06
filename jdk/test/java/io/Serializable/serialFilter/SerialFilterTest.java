/*
 * Copyright (c) 2016, 2017, Oracle and/or its affiliates. All rights reserved.
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
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 */

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.EOFException;
import java.io.IOException;
import java.io.InvalidClassException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Proxy;
import java.util.*;

import javax.lang.model.SourceVersion;

import org.testng.Assert;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import sun.misc.ObjectInputFilter;

/* @test
 * @build SerialFilterTest
 * @run testng/othervm  SerialFilterTest
 *
 * @summary Test ObjectInputFilters
 */
@Test
public class SerialFilterTest implements Serializable {

    private static final long serialVersionUID = -6999613679881262446L;

    private static class Container {
        final Object o;
        final String pattern;
        final boolean bool;

        Container(String pattern, Object o, boolean bool) {
            this.o = o;
            this.pattern = pattern;
            this.bool = bool;
        }
    }

    /**
     * Misc object to use that should always be accepted.
     */
    private static final Object otherObject = Integer.valueOf(0);

    /**
     * DataProvider for the individual patterns to test.
     * Expand the patterns into cases for each of the Std and Compatibility APIs.
     * @return an array of arrays of the parameters including factories
     */
    @DataProvider(name="Patterns")
    static Object[][] patterns() {
        Object[][] patterns = new Object[][]{
            {"java.util.Hashtable"},
            {"java.util.Hash*"},
            {"javax.lang.model.*"},
            {"javax.lang.**"},
            {"*"},
            {"maxarray=47"},
            {"maxdepth=5"},
            {"maxrefs=10"},
            {"maxbytes=100"},
            {"maxbytes=72"},
            {"maxbytes=+1024"},
        };
        return patterns;
    }

    @DataProvider(name="InvalidPatterns")
    static Object[][] invalidPatterns() {
        return new Object [][] {
            {"maxrefs=-1"},
            {"maxdepth=-1"},
            {"maxbytes=-1"},
            {"maxarray=-1"},
            {"xyz=0"},
            {"xyz=-1"},
            {"maxrefs=0xabc"},
            {"maxrefs=abc"},
            {"maxrefs="},
            {"maxrefs=+"},
            {".*"},
            {".**"},
            {"!"},
            {"/java.util.Hashtable"},
            {"java.base/"},
            {"/"},
        };
    }

    @DataProvider(name="Limits")
    static Object[][] limits() {
        // The numbers are arbitrary > 1
        return new Object[][]{
            {"maxrefs", 10},
            {"maxdepth", 5},
            {"maxbytes", 100},
            {"maxarray", 16},
        };
    }

    /**
     * DataProvider of individual objects. Used to check the information
     * available to the filter.
     * @return  Arrays of parameters with objects
     */
    @DataProvider(name="Objects")
    static Object[][] objects() {
        byte[] byteArray = new byte[0];
        Object[] objArray = new Object[7];
        objArray[objArray.length - 1] = objArray;

        List<Class<?>> classList = new ArrayList<>();
        classList.add(HashSet.class);
        classList.addAll(Collections.nCopies(21, Map.Entry[].class));

        Object[][] objects = {
                { null, 0, -1, 0, 0, 0,
                        Arrays.asList()},        // no callback, no values
                { objArray, 3, 7, 9, 2, 55,
                        Arrays.asList(objArray.getClass(), objArray.getClass())},
                { Object[].class, 1, -1, 1, 1, 38,
                        Arrays.asList(Object[].class)},
                { new SerialFilterTest(), 1, -1, 1, 1, 35,
                        Arrays.asList(SerialFilterTest.class)},
                { new byte[14], 2, 14, 2, 1, 27,
                        Arrays.asList(byteArray.getClass(), byteArray.getClass())},
                // maxarray changed for Java 6 from "4" to "16"
                // HashSet.readObject isn't optimal, see JDK-8016252
                { deepHashSet(10), 69, 16, 50, 11, 619, classList },
        };
        return objects;
    }

    @DataProvider(name="Arrays")
    static Object[][] arrays() {
        return new Object[][]{
            {new Object[16], 16},
            {new boolean[16], 16},
            {new byte[16], 16},
            {new char[16], 16},
            {new int[16], 16},
            {new long[16], 16},
            {new short[16], 16},
            {new float[16], 16},
            {new double[16], 16},
        };
    }


    /**
     * Test each object and verify the classes identified by the filter,
     * the count of calls to the filter, the max array size, max refs, max depth,
     * max bytes.
     * This test ignores/is not dependent on the global filter settings.
     *
     * @param object a Serializable object
     * @param count the expected count of calls to the filter
     * @param maxArray the maximum array size
     * @param maxRefs the maximum references
     * @param maxDepth the maximum depth
     * @param maxBytes the maximum stream size
     * @param classes  the expected (unique) classes
     * @throws IOException
     */
    @Test(dataProvider="Objects")
    public static void t1(Object object,
            long count, long maxArray, long maxRefs, long maxDepth, long maxBytes,
            List<Class<?>> classes) throws IOException {
        byte[] bytes = writeObjects(object);
        Validator validator = new Validator();
        validate(bytes, validator);
        System.out.printf("v: %s%n", validator);

        Assert.assertEquals(validator.count, count, "callback count wrong");
        Assert.assertEquals(validator.classes, classes, "classes mismatch");
        Assert.assertEquals(validator.maxArray, maxArray, "maxArray mismatch");
        Assert.assertEquals(validator.maxRefs, maxRefs, "maxRefs wrong");
        Assert.assertEquals(validator.maxDepth, maxDepth, "depth wrong");
        Assert.assertEquals(validator.maxBytes, maxBytes, "maxBytes wrong");
    }

    /**
     * Test each pattern with an appropriate object.
     * A filter is created from the pattern and used to serialize and
     * deserialize a generated object with both the positive and negative case.
     * This test ignores/is not dependent on the global filter settings.
     *
     * @param pattern a pattern
     */
    @Test(dataProvider="Patterns")
    static void testPatterns(String pattern) {
        List<Container> containers = genPatternObjects(pattern);
        for (Container c: containers) {
            testPatterns(c.pattern, c.o, c.bool);
        }
    }

    /**
     * Test that the filter on a OIS can be set only on a fresh OIS,
     * before deserializing any objects.
     * This test is agnostic the global filter being set or not.
     */
    @Test
    static void nonResettableFilter() {
        Validator validator1 = new Validator();
        Validator validator2 = new Validator();

        try {
            byte[] bytes = writeObjects("text1");    // an object

            ByteArrayInputStream bais = null;
            ObjectInputStream ois = null;
            try {
                bais = new ByteArrayInputStream(bytes);
                ois = new ObjectInputStream(bais);

                // Check the initial filter is the global filter; may be null
                ObjectInputFilter global = ObjectInputFilter.Config.getSerialFilter();
                ObjectInputFilter initial = ObjectInputFilter.Config.getObjectInputFilter(ois);
                Assert.assertEquals(global, initial, "initial filter should be the global filter");

                // Check if it can be set to null
                ObjectInputFilter.Config.setObjectInputFilter(ois, null);
                ObjectInputFilter filter = ObjectInputFilter.Config.getObjectInputFilter(ois);
                Assert.assertNull(filter, "set to null should be null");

                ObjectInputFilter.Config.setObjectInputFilter(ois, validator1);
                Object o = ois.readObject();
                try {
                    ObjectInputFilter.Config.setObjectInputFilter(ois, validator2);
                    Assert.fail("Should not be able to set filter twice");
                } catch (IllegalStateException ise) {
                    // success, the exception was expected
                }
            } catch (EOFException eof) {
                Assert.fail("Should not reach end-of-file", eof);
            } catch (ClassNotFoundException cnf) {
                Assert.fail("Deserializing", cnf);
            } finally {
                try {
                    if (bais != null) {
                        bais.close();
                    }
                    if (ois != null) {
                        ois.close();
                    }
                } catch (IOException ioe) {
                    ioe.printStackTrace();
                }
            }
        } catch (IOException ex) {
            Assert.fail("Unexpected IOException", ex);
        }
    }

    /**
     * Test that if an Objects readReadResolve method returns an array
     * that the callback to the filter includes the proper array length.
     * @throws IOException if an error occurs
     */
    @Test(dataProvider="Arrays")
    static void testReadResolveToArray(Object array, int length) throws IOException {
        ReadResolveToArray object = new ReadResolveToArray(array, length);
        byte[] bytes = writeObjects(object);
        Object o = validate(bytes, object);    // the object is its own filter
        Assert.assertEquals(o.getClass(), array.getClass(), "Filter not called with the array");
    }

    /**
     * Test repeated limits use the last value.
     * Construct a filter with the limit and the limit repeated -1.
     * Invoke the filter with the limit to make sure it is rejected.
     * Invoke the filter with the limit -1 to make sure it is accepted.
     * @param name the name of the limit to test
     * @param value a test value
     */
    @Test(dataProvider="Limits")
    static void testLimits(String name, int value) {
        Class<?> arrayClass = new int[0].getClass();
        String pattern = String.format("%s=%d;%s=%d", name, value, name, value - 1);
        ObjectInputFilter filter = ObjectInputFilter.Config.createFilter(pattern);
        Assert.assertEquals(
                filter.checkInput(new FilterValues(arrayClass, value, value, value, value)),
                ObjectInputFilter.Status.REJECTED,
                "last limit value not used: " + filter);
        Assert.assertEquals(
                filter.checkInput(new FilterValues(arrayClass, value-1, value-1, value-1, value-1)),
                ObjectInputFilter.Status.UNDECIDED,
                "last limit value not used: " + filter);
    }

    /**
     * Test that returning null from a filter causes deserialization to fail.
     */
    @Test(expectedExceptions=InvalidClassException.class)
    static void testNullStatus() throws IOException {
        byte[] bytes = writeObjects(0); // an Integer
        try {
            Object o = validate(bytes, new ObjectInputFilter() {
                @Override
                public ObjectInputFilter.Status checkInput(ObjectInputFilter.FilterInfo f) {
                    return null;
                }
            });
        } catch (InvalidClassException ice) {
            System.out.printf("Success exception: %s%n", ice);
            throw ice;
        }
    }

    /**
     * Verify that malformed patterns throw IAE.
     * @param pattern pattern from the data source
     */
    @Test(dataProvider="InvalidPatterns", expectedExceptions=IllegalArgumentException.class)
    static void testInvalidPatterns(String pattern) {
        try {
            ObjectInputFilter.Config.createFilter(pattern);
        } catch (IllegalArgumentException iae) {
            System.out.printf("    success exception: %s%n", iae);
            throw iae;
        }
    }

    /**
     * Test that Config.create returns null if the argument does not contain any patterns or limits.
     */
    @Test()
    static void testEmptyPattern() {
        ObjectInputFilter filter = ObjectInputFilter.Config.createFilter("");
        Assert.assertNull(filter, "empty pattern did not return null");

        filter = ObjectInputFilter.Config.createFilter(";;;;");
        Assert.assertNull(filter, "pattern with only delimiters did not return null");
    }

    /**
     * Read objects from the serialized stream, validated with the filter.
     *
     * @param bytes a byte array to read objects from
     * @param filter the ObjectInputFilter
     * @return the object deserialized if any
     * @throws IOException can be thrown
     */
    static Object validate(byte[] bytes,
            ObjectInputFilter filter) throws IOException {
        ByteArrayInputStream bais = null;
        ObjectInputStream ois = null;
        try {
            bais = new ByteArrayInputStream(bytes);
            ois = new ObjectInputStream(bais);
            ObjectInputFilter.Config.setObjectInputFilter(ois, filter);

            Object o = ois.readObject();
            return o;
        } catch (EOFException eof) {
            // normal completion
        } catch (ClassNotFoundException cnf) {
            Assert.fail("Deserializing", cnf);
        } finally {
            try {
                if (bais != null) {
                    bais.close();
                }
                if (ois != null) {
                    ois.close();
                }
            } catch (IOException ioe) {
                ioe.printStackTrace();
            }
        }
        return null;
    }

    /**
     * Write objects and return a byte array with the bytes.
     *
     * @param objects zero or more objects to serialize
     * @return the byte array of the serialized objects
     * @throws IOException if an exception occurs
     */
    static byte[] writeObjects(Object... objects)  throws IOException {
        byte[] bytes;
        ByteArrayOutputStream baos = null;
        ObjectOutputStream oos = null;
        try {
            baos = new ByteArrayOutputStream();
            oos = new ObjectOutputStream(baos);
            for (Object o : objects) {
                oos.writeObject(o);
            }
            bytes = baos.toByteArray();
        } finally {
            try {
                if (baos != null) {
                    baos.close();
                }
                if (oos != null) {
                    oos.close();
                }
            } catch (IOException ioe) {
                ioe.printStackTrace();
            }
        }
        return bytes;
    }

    /**
     * A filter that accumulates information about the checkInput callbacks
     * that can be checked after readObject completes.
     */
    static class Validator implements ObjectInputFilter {
        long count;          // Count of calls to checkInput
        List<Class<?>> classes = new ArrayList<>();
        long maxArray = -1;
        long maxRefs;
        long maxDepth;
        long maxBytes;

        Validator() {
        }

        @Override
        public ObjectInputFilter.Status checkInput(FilterInfo filter) {
            Class<?> serialClass = filter.serialClass();
            System.out.printf("     checkInput: class: %s, arrayLen: %d, refs: %d, depth: %d, bytes; %d%n",
                    serialClass, filter.arrayLength(), filter.references(),
                    filter.depth(), filter.streamBytes());
            count++;
            if (filter.serialClass() != null) {
                if (Proxy.isProxyClass(serialClass)) {
                    classes.add(Proxy.class);
                } else {
                    classes.add(serialClass);
                }

            }
            this.maxArray = Math.max(this.maxArray, filter.arrayLength());
            this.maxRefs = Math.max(this.maxRefs, filter.references());
            this.maxDepth = Math.max(this.maxDepth, filter.depth());
            this.maxBytes = Math.max(this.maxBytes, filter.streamBytes());
            return ObjectInputFilter.Status.UNDECIDED;
        }

        @Override
        public String toString(){
            return "count: " + count
                    + ", classes: " + classes.toString()
                    + ", maxArray: " + maxArray
                    + ", maxRefs: " + maxRefs
                    + ", maxDepth: " + maxDepth
                    + ", maxBytes: " + maxBytes;
        }
    }


    /**
     * Create a filter from a pattern and API factory, then serialize and
     * deserialize an object and check allowed or reject.
     *
     * @param pattern the pattern
     * @param object the test object
     * @param allowed the expected result from ObjectInputStream (exception or not)
     */
    static void testPatterns(String pattern, Object object, boolean allowed) {
        try {
            byte[] bytes = SerialFilterTest.writeObjects(object);
            ObjectInputFilter filter = ObjectInputFilter.Config.createFilter(pattern);
            validate(bytes, filter);
            Assert.assertTrue(allowed, "filter should have thrown an exception");
        } catch (IllegalArgumentException iae) {
            Assert.fail("bad format pattern", iae);
        } catch (InvalidClassException ice) {
            Assert.assertFalse(allowed, "filter should not have thrown an exception: " + ice);
        } catch (IOException ioe) {
            Assert.fail("Unexpected IOException", ioe);
        }
    }

    /**
     * For a filter pattern, generate and apply a test object to the action.
     * @param pattern a pattern
     * @param action an action to perform on positive and negative cases
     */
    static List<Container> genPatternObjects(String pattern) {
        List<Container> containers = new ArrayList<Container>();
        Object o = genTestObject(pattern, true);
        Assert.assertNotNull(o, "success generation failed");
        containers.add(new Container(pattern, o, true));

        // Test the negative pattern
        o = genTestObject(pattern, false);
        Assert.assertNotNull(o, "fail generation failed");
        String negPattern = pattern.contains("=") ? pattern : "!" + pattern;
        containers.add(new Container(negPattern, o, false));
        return containers;
    }

    /**
     * Generate a test object based on the pattern.
     * Handles each of the forms of the pattern, wildcards,
     * class name, various limit forms.
     * @param pattern a pattern
     * @param allowed a boolean indicating to generate the allowed or disallowed case
     * @return an object or {@code null} to indicate no suitable object could be generated
     */
    static Object genTestObject(String pattern, boolean allowed) {
        if (pattern.contains("=")) {
            return genTestLimit(pattern, allowed);
        } else if (pattern.endsWith("*")) {
            return genTestObjectWildcard(pattern, allowed);
        } else {
            // class
            try {
                Class<?> clazz = Class.forName(pattern);
                Constructor<?> cons = clazz.getConstructor();
                return cons.newInstance();
            } catch (ClassNotFoundException ex) {
                Assert.fail("no such class available: " + pattern);
            } catch (InvocationTargetException ex1) {
                Assert.fail("newInstance: " + ex1);
            } catch (NoSuchMethodException ex2) {
                Assert.fail("newInstance: " + ex2);
            } catch (InstantiationException ex3) {
                Assert.fail("newInstance: " + ex3);
            } catch (IllegalAccessException ex4) {
                Assert.fail("newInstance: " + ex4);
            }
        }
        return null;
    }

    /**
     * Generate an object to be used with the various wildcard pattern forms.
     * Explicitly supports only specific package wildcards with specific objects.
     * @param pattern a wildcard pattern ending in "*"
     * @param allowed a boolean indicating to generate the allowed or disallowed case
     * @return an object within or outside the wildcard
     */
    static Object genTestObjectWildcard(String pattern, boolean allowed) {
        if (pattern.endsWith(".**")) {
            // package hierarchy wildcard
            if (pattern.startsWith("javax.lang.")) {
                return SourceVersion.RELEASE_5;
            }
            if (pattern.startsWith("java.")) {
                return 4;
            }
            if (pattern.startsWith("javax.")) {
                return SourceVersion.RELEASE_6;
            }
            return otherObject;
        } else if (pattern.endsWith(".*")) {
            // package wildcard
            if (pattern.startsWith("javax.lang.model")) {
                return SourceVersion.RELEASE_6;
            }
        } else {
            // class wildcard
            if (pattern.equals("*")) {
                return otherObject; // any object will do
            }
            if (pattern.startsWith("java.util.Hash")) {
                return new Hashtable<String, String>();
            }
        }
        Assert.fail("Object could not be generated for pattern: "
                + pattern
                + ", allowed: " + allowed);
        return null;
    }

    /**
     * Generate a limit test object for the pattern.
     * For positive cases, the object exactly hits the limit.
     * For negative cases, the object is 1 greater than the limit
     * @param pattern the pattern, containing "=" and a maxXXX keyword
     * @param allowed a boolean indicating to generate the allowed or disallowed case
     * @return a suitable object
     */
    static Object genTestLimit(String pattern, boolean allowed) {
        int ndx = pattern.indexOf('=');
        Assert.assertNotEquals(ndx, -1, "missing value in limit");
        long value = parseUnsignedLong(pattern.substring(ndx+1));
        if (pattern.startsWith("maxdepth=")) {
            // Return an object with the requested depth (or 1 greater)
            long depth = allowed ? value : value + 1;
            Object[] array = new Object[1];
            for (int i = 1; i < depth; i++) {
                Object[] n = new Object[1];
                n[0] = array;
                array = n;
            }
            return array;
        } else if (pattern.startsWith("maxbytes=")) {
            // Return a byte array that when written to OOS creates
            // a stream of exactly the size requested.
            return genMaxBytesObject(allowed, value);
        } else if (pattern.startsWith("maxrefs=")) {
            // 4 references to classes in addition to the array contents
            Object[] array = new Object[allowed ? (int)value - 4 : (int)value - 3];
            for (int i = 0; i < array.length; i++) {
                array[i] = otherObject;
            }
            return array;
        } else if (pattern.startsWith("maxarray=")) {
            return allowed ? new int[(int)value] : new int[(int)value+1];
        }
        Assert.fail("Object could not be generated for pattern: "
                + pattern
                + ", allowed: " + allowed);
        return null;
    }

    static long parseUnsignedLong(String s) throws NumberFormatException {
        return parseUnsignedLong(s, 10);
    }

    static long parseUnsignedLong(String s, int radix)
            throws NumberFormatException {
        if (s == null)  {
            throw new NumberFormatException("null");
        }

        int len = s.length();
        if (len > 0) {
            char firstChar = s.charAt(0);
            if (firstChar == '-') {
                throw new
                NumberFormatException(String.format("Illegal leading minus sign " +
                        "on unsigned string %s.", s));
            } else {
                if (len <= 12 || // Long.MAX_VALUE in Character.MAX_RADIX is 13 digits
                        (radix == 10 && len <= 18) ) { // Long.MAX_VALUE in base 10 is 19 digits
                    return Long.parseLong(s, radix);
                }

                // No need for range checks on len due to testing above.
                long first = Long.parseLong(s.substring(0, len - 1), radix);
                int second = Character.digit(s.charAt(len - 1), radix);
                if (second < 0) {
                    throw new NumberFormatException("Bad digit at end of " + s);
                }
                long result = first * radix + second;
                if (compareUnsigned(result, first) < 0) {
                    /*
                     * The maximum unsigned value, (2^64)-1, takes at
                     * most one more digit to represent than the
                     * maximum signed value, (2^63)-1.  Therefore,
                     * parsing (len - 1) digits will be appropriately
                     * in-range of the signed parsing.  In other
                     * words, if parsing (len -1) digits overflows
                     * signed parsing, parsing len digits will
                     * certainly overflow unsigned parsing.
                     *
                     * The compareUnsigned check above catches
                     * situations where an unsigned overflow occurs
                     * incorporating the contribution of the final
                     * digit.
                     */
                    throw new NumberFormatException(String.format("String value %s exceeds " +
                            "range of unsigned long.", s));
                }
                return result;
            }
        } else {
            throw new NumberFormatException(s);
        }
    }

    public static int compareUnsigned(long x, long y) {
        return Long.compare(x + Long.MIN_VALUE, y + Long.MIN_VALUE);
    }

    /**
     * Generate an an object that will be serialized to some number of bytes.
     * Or 1 greater if allowed is false.
     * It returns a two element Object array holding a byte array sized
     * to achieve the desired total size.
     * @param allowed true if the stream should be allowed at that size,
     *                false if the stream should be larger
     * @param maxBytes the number of bytes desired in the stream;
     *                 should not be less than 72 (due to protocol overhead).
     * @return a object that will be serialized to the length requested
     */
    private static Object genMaxBytesObject(boolean allowed, long maxBytes) {
        Object[] holder = new Object[2];
        long desiredSize = allowed ? maxBytes : maxBytes + 1;
        long actualSize = desiredSize;
        long byteSize = desiredSize - 72;  // estimate needed array size
        do {
            byteSize += (desiredSize - actualSize);
            byte[] a = new byte[(int)byteSize];
            holder[0] = a;
            holder[1] = a;
            ByteArrayOutputStream baos = null;
            ObjectOutputStream os = null;
            try {
                baos = new ByteArrayOutputStream();
                os = new ObjectOutputStream(baos);
                os.writeObject(holder);
                os.flush();
                actualSize = baos.size();
            } catch (IOException ie) {
                Assert.fail("exception generating stream", ie);
            } finally {
                try {
                    if (baos != null) {
                        baos.close();
                    }
                    if (os != null) {
                        os.close();
                    }
                } catch (IOException ioe) {
                    ioe.printStackTrace();
                }
            }
        } while (actualSize != desiredSize);
        return holder;
    }

    /**
     * Returns a HashSet of a requested depth.
     * @param depth the depth
     * @return a HashSet of HashSets...
     */
    static HashSet<Object> deepHashSet(int depth) {
        HashSet<Object> hashSet = new HashSet<Object>();
        HashSet<Object> s1 = hashSet;
        HashSet<Object> s2 = new HashSet<Object>();
        for (int i = 0; i < depth; i++ ) {
            HashSet<Object> t1 = new HashSet<Object>();
            HashSet<Object> t2 = new HashSet<Object>();
            // make t1 not equal to t2
            t1.add("by Jimminy");
            s1.add(t1);
            s1.add(t2);
            s2.add(t1);
            s2.add(t2);
            s1 = t1;
            s2 = t2;
        }
        return hashSet;
    }


    /**
     * Class that returns an array from readResolve and also implements
     * the ObjectInputFilter to check that it has the expected length.
     */
    static class ReadResolveToArray implements Serializable, ObjectInputFilter {
        private static final long serialVersionUID = 123456789L;

        private final Object array;
        private final int length;

        ReadResolveToArray(Object array, int length) {
            this.array = array;
            this.length = length;
        }

        Object readResolve() {
            return array;
        }

        @Override
        public ObjectInputFilter.Status checkInput(FilterInfo filter) {
            if (ReadResolveToArray.class.isAssignableFrom(filter.serialClass())) {
                return ObjectInputFilter.Status.ALLOWED;
            }
            if (filter.serialClass() != array.getClass() ||
                    (filter.arrayLength() >= 0 && filter.arrayLength() != length)) {
                return ObjectInputFilter.Status.REJECTED;
            }
            return ObjectInputFilter.Status.UNDECIDED;
        }

    }

    /**
     * Hold a snapshot of values to be passed to an ObjectInputFilter.
     */
    static class FilterValues implements ObjectInputFilter.FilterInfo {
        private final Class<?> clazz;
        private final long arrayLength;
        private final long depth;
        private final long references;
        private final long streamBytes;

        public FilterValues(Class<?> clazz, long arrayLength, long depth, long references, long streamBytes) {
            this.clazz = clazz;
            this.arrayLength = arrayLength;
            this.depth = depth;
            this.references = references;
            this.streamBytes = streamBytes;
        }

        @Override
        public Class<?> serialClass() {
            return clazz;
        }

        @Override
        public long arrayLength() {
            return arrayLength;
        }

        @Override
        public long depth() {
            return depth;
        }

        @Override
        public long references() {
            return references;
        }

        @Override
        public long streamBytes() {
            return streamBytes;
        }
    }

    // Deeper superclass hierarchy
    static class A implements Serializable {
        private static final long serialVersionUID = 1L;
    };
    static class B extends A {
        private static final long serialVersionUID = 2L;
    }
    static class C extends B {
        private static final long serialVersionUID = 3L;
    }
    static class D extends C {
        private static final long serialVersionUID = 4L;
    }
    static class E extends D {
        private static final long serialVersionUID = 5L;
    }
    static class F extends E {
        private static final long serialVersionUID = 6L;
    }

}
