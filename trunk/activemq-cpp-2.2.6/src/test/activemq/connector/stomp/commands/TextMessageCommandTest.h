/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _ACTIVEMQ_CONNECTOR_STOMP_COMMANDS_TEXTMESSAGECOMMANDTEST_H_
#define _ACTIVEMQ_CONNECTOR_STOMP_COMMANDS_TEXTMESSAGECOMMANDTEST_H_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <activemq/connector/stomp/StompTopic.h>
#include <activemq/util/Config.h>
#include <cms/Message.h>

#include <activemq/connector/stomp/commands/TextMessageCommand.h>

namespace activemq{
namespace connector{
namespace stomp{
namespace commands{

    class TextMessageCommandTest : public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE( TextMessageCommandTest );
        CPPUNIT_TEST( test );
        CPPUNIT_TEST_SUITE_END();

    protected:

        class TestAckHandler : public core::ActiveMQAckHandler
        {
        public:

            TestAckHandler(void) { wasAcked = false; }
            virtual ~TestAckHandler(void) {}

            virtual void acknowledgeMessage( const core::ActiveMQMessage* message AMQCPP_UNUSED)
                throw ( cms::CMSException )
            {
                wasAcked = true;
            }

        public:

            bool wasAcked;

        };

    public:

        TextMessageCommandTest() {}
        virtual ~TextMessageCommandTest() {}

        void test(void)
        {
            TestAckHandler ackHandler;
            TextMessageCommand cmd;

            CPPUNIT_ASSERT( cmd.getStompCommandId() ==
                            CommandConstants::SEND );

            CPPUNIT_ASSERT( cmd.isResponseRequired() == false );
            cmd.setResponseRequired( true );
            cmd.setCommandId( 123 );
            CPPUNIT_ASSERT( cmd.isResponseRequired() == false );
            CPPUNIT_ASSERT( cmd.getCommandId() == 0 );
            cmd.setCorrelationId( 99 );
            CPPUNIT_ASSERT( cmd.getCorrelationId() == 99 );
            CPPUNIT_ASSERT( cmd.getTransactionId() == "" );
            cmd.setTransactionId( "ID:123456" );
            CPPUNIT_ASSERT( std::string( cmd.getTransactionId() ) ==
                            "ID:123456" );
            StompTopic topic("testTopic");
            cmd.setCMSDestination( &topic );

            StompFrame* frame = cmd.marshal().clone();

            CPPUNIT_ASSERT( frame != NULL );

            TextMessageCommand cmd1( frame );

            CPPUNIT_ASSERT( cmd.getCommandId() == cmd1.getCommandId() );
            CPPUNIT_ASSERT( cmd.getStompCommandId() == cmd1.getStompCommandId() );
            CPPUNIT_ASSERT( cmd.isResponseRequired() == cmd1.isResponseRequired() );
            CPPUNIT_ASSERT( cmd.getCorrelationId() == cmd1.getCorrelationId() );
            CPPUNIT_ASSERT( std::string(cmd.getTransactionId()) == cmd1.getTransactionId() );

            cmd.setAckHandler( &ackHandler );
            cmd.acknowledge();
            CPPUNIT_ASSERT( ackHandler.wasAcked == true );

            CPPUNIT_ASSERT(
                cmd.getProperties().hasProperty( "test" ) == false );
            cmd.getProperties().setProperty( "test", "value" );
            CPPUNIT_ASSERT(
                cmd.getProperties().hasProperty( "test" ) == true );
            CPPUNIT_ASSERT(
                std::string( cmd.getProperties().getProperty( "test" ) ) ==
                "value" );

            CPPUNIT_ASSERT( cmd.getCMSCorrelationID() == "" );
            cmd.setCMSCorrelationID( "ID:1234567" );
            CPPUNIT_ASSERT( std::string( cmd.getCMSCorrelationID() ) ==
                            "ID:1234567" );
            CPPUNIT_ASSERT( cmd.getCMSDeliveryMode() ==
                            cms::DeliveryMode::PERSISTENT );
            cmd.setCMSDeliveryMode( cms::DeliveryMode::NON_PERSISTENT );
            CPPUNIT_ASSERT( cmd.getCMSDeliveryMode() ==
                            cms::DeliveryMode::NON_PERSISTENT );
            CPPUNIT_ASSERT( cmd.getCMSDestination()->toProviderString() ==
                            "/topic/testTopic" );
            CPPUNIT_ASSERT( cmd.getCMSExpiration() == 0 );
            cmd.setCMSExpiration( 123 );
            CPPUNIT_ASSERT( cmd.getCMSExpiration() == 123 );
            CPPUNIT_ASSERT( cmd.getCMSMessageID() == "" );
            cmd.setCMSMessageID( "ID:1234567" );
            CPPUNIT_ASSERT( std::string( cmd.getCMSMessageID() ) ==
                            "ID:1234567" );
            CPPUNIT_ASSERT( cmd.getCMSPriority() == 0 );
            cmd.setCMSPriority( 5 );
            CPPUNIT_ASSERT( cmd.getCMSPriority() == 5 );
            CPPUNIT_ASSERT( cmd.getCMSRedelivered() == false );
            cmd.setCMSRedelivered( true );
            CPPUNIT_ASSERT( cmd.getCMSRedelivered() == true );
            CPPUNIT_ASSERT( cmd.getCMSReplyTo() == NULL );
            cmd.setCMSReplyTo( &topic );
            CPPUNIT_ASSERT( cmd.getCMSReplyTo()->toProviderString() ==
                            "/topic/testTopic" );
            CPPUNIT_ASSERT( cmd.getCMSTimestamp() == 0 );
            cmd.setCMSTimestamp( 123 );
            CPPUNIT_ASSERT( cmd.getCMSTimestamp() == 123 );
            CPPUNIT_ASSERT( cmd.getCMSType() == "" );
            cmd.setCMSType( "text" );
            CPPUNIT_ASSERT( std::string( cmd.getCMSType() ) ==
                            "text" );
            CPPUNIT_ASSERT( cmd.getRedeliveryCount() == 0 );
            cmd.setRedeliveryCount( 123 );
            CPPUNIT_ASSERT( cmd.getRedeliveryCount() == 123 );

            CPPUNIT_ASSERT( cmd.getText() == "" );
            cmd.setText( "TESTMESSAGE" );
            CPPUNIT_ASSERT( std::string( cmd.getText() ) == "TESTMESSAGE" );

            cms::Message* cmd2 = cmd.clone();

            CPPUNIT_ASSERT( cmd.getCMSPriority() == cmd2->getCMSPriority() );
            CPPUNIT_ASSERT( cmd.getCMSTimestamp() == cmd2->getCMSTimestamp() );
            CPPUNIT_ASSERT( cmd.getCMSExpiration() == cmd2->getCMSExpiration() );
            CPPUNIT_ASSERT( cmd.getCMSDeliveryMode() == cmd2->getCMSDeliveryMode() );
            CPPUNIT_ASSERT( std::string(cmd.getCMSCorrelationID()) == cmd2->getCMSCorrelationID() );
            CPPUNIT_ASSERT( cmd.getCMSReplyTo()->toProviderString() == cmd2->getCMSReplyTo()->toProviderString() );
            CPPUNIT_ASSERT( std::string(cmd.getCMSType()) == cmd2->getCMSType() );
            CPPUNIT_ASSERT( std::string(cmd.getCMSMessageID()) == cmd2->getCMSMessageID() );

            core::ActiveMQMessage* message =
                dynamic_cast< core::ActiveMQMessage* >( cmd2 );

            CPPUNIT_ASSERT( message != NULL );
            CPPUNIT_ASSERT( cmd.getRedeliveryCount() ==
                            message->getRedeliveryCount() );

            StompCommand* cmd4 =
                dynamic_cast< StompCommand* >( cmd2 );

            CPPUNIT_ASSERT( cmd4 != NULL );
            CPPUNIT_ASSERT( cmd.getCommandId() == cmd4->getCommandId() );
            CPPUNIT_ASSERT( cmd.getStompCommandId() == cmd4->getStompCommandId() );
            CPPUNIT_ASSERT( cmd.isResponseRequired() == cmd4->isResponseRequired() );
            CPPUNIT_ASSERT( cmd.getCorrelationId() == cmd4->getCorrelationId() );
            CPPUNIT_ASSERT( std::string( cmd.getTransactionId() ) ==
                            cmd4->getTransactionId() );

            TextMessageCommand* cmd5 =
                dynamic_cast< TextMessageCommand* >(message);

            CPPUNIT_ASSERT( cmd5 != NULL );
            CPPUNIT_ASSERT( std::string( cmd.getText() ) == cmd5->getText() );

            delete cmd2;
        }

    };

}}}}

#endif /*_ACTIVEMQ_CONNECTOR_STOMP_COMMANDS_TEXTMESSAGECOMMANDTEST_H_*/
