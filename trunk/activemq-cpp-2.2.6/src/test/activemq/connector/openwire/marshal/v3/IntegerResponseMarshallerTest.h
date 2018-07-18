/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _ACTIVEMQ_CONNECTOR_OPENWIRE_MARSAHAL_V3_INTEGERRESPONSEMARSHALLERTEST_H_
#define _ACTIVEMQ_CONNECTOR_OPENWIRE_MARSAHAL_V3_INTEGERRESPONSEMARSHALLERTEST_H_

// Turn off warning message for ignored exception specification
#ifdef _MSC_VER
#pragma warning( disable : 4290 )
#endif

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

namespace activemq{
namespace connector{
namespace openwire{
namespace marshal{
namespace v3{

    /**
     * Marshalling Test code for Open Wire Format for IntegerResponseMarshallerTest
     *
     *  NOTE!: This file is autogenerated - do not modify!
     *         if you need to make a change, please see the Java Classes
     *         in the activemq-openwire-generator module
     */
    class IntegerResponseMarshallerTest : public CppUnit::TestFixture {

        CPPUNIT_TEST_SUITE( IntegerResponseMarshallerTest );
        CPPUNIT_TEST( test );
        CPPUNIT_TEST( testLooseMarshal );
        CPPUNIT_TEST( testTightMarshal );
        CPPUNIT_TEST_SUITE_END();

    public:

        IntegerResponseMarshallerTest() {}
        virtual ~IntegerResponseMarshallerTest() {}

        /**
         * Test the marshaller and its marshalled type.
         */
        virtual void test();
        virtual void testLooseMarshal();
        virtual void testTightMarshal();

    };

}}}}}

#endif /*_ACTIVEMQ_CONNECTOR_OPENWIRE_MARSAHAL_V3_INTEGERRESPONSEMARSHALLERTEST_H_*/

