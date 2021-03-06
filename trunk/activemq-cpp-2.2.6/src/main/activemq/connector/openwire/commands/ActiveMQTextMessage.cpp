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
#include <activemq/connector/openwire/commands/ActiveMQTextMessage.h>

#include <decaf/io/ByteArrayOutputStream.h>
#include <decaf/io/DataOutputStream.h>

using namespace std;
using namespace activemq;
using namespace activemq::exceptions;
using namespace activemq::connector;
using namespace activemq::connector::openwire;
using namespace activemq::connector::openwire::commands;
using namespace decaf::lang;

////////////////////////////////////////////////////////////////////////////////
ActiveMQTextMessage::ActiveMQTextMessage() :
    ActiveMQMessageBase<cms::TextMessage>() {
}

////////////////////////////////////////////////////////////////////////////////
ActiveMQTextMessage::~ActiveMQTextMessage() {
}

////////////////////////////////////////////////////////////////////////////////
unsigned char ActiveMQTextMessage::getDataStructureType() const {
    return ActiveMQTextMessage::ID_ACTIVEMQTEXTMESSAGE;
}

////////////////////////////////////////////////////////////////////////////////
std::string ActiveMQTextMessage::getText() const throw( cms::CMSException ) {

    try{
        if( getContent().size() <= 4 ) {
            return "";
        }

        return std::string( (const char*)&getContent()[4], getContent().size()-4 );
    }
    AMQ_CATCH_RETHROW( ActiveMQException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, ActiveMQException )
    AMQ_CATCHALL_THROW( ActiveMQException )
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQTextMessage::setText( const char* msg ) throw( cms::CMSException ) {
    try{
        setText( std::string(msg) );
    }
    AMQ_CATCH_RETHROW( ActiveMQException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, ActiveMQException )
    AMQ_CATCHALL_THROW( ActiveMQException )
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQTextMessage::setText( const std::string& msg ) throw( cms::CMSException ) {
    try{
        checkReadOnlyBody();
        std::vector<unsigned char>& content = getContent();
        content.clear();
        decaf::io::ByteArrayOutputStream bos( content );
        decaf::io::DataOutputStream dos( &bos );

        dos.writeInt( (int)msg.length() );
        dos.write( (const unsigned char*)msg.c_str(), 0, msg.length() );
    }
    AMQ_CATCH_RETHROW( ActiveMQException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, ActiveMQException )
    AMQ_CATCHALL_THROW( ActiveMQException )
}
