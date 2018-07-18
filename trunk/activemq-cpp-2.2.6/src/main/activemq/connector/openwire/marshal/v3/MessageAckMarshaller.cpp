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

#include <activemq/connector/openwire/marshal/v3/MessageAckMarshaller.h>

#include <activemq/connector/openwire/commands/MessageAck.h>
#include <activemq/exceptions/ActiveMQException.h>

//
//     NOTE!: This file is autogenerated - do not modify!
//            if you need to make a change, please see the Java Classes in the
//            activemq-core module
//

using namespace std;
using namespace activemq;
using namespace activemq::exceptions;
using namespace activemq::connector;
using namespace activemq::connector::openwire;
using namespace activemq::connector::openwire::commands;
using namespace activemq::connector::openwire::marshal;
using namespace activemq::connector::openwire::utils;
using namespace activemq::connector::openwire::marshal::v3;
using namespace decaf;
using namespace decaf::io;

///////////////////////////////////////////////////////////////////////////////
DataStructure* MessageAckMarshaller::createObject() const {
    return new MessageAck();
}

///////////////////////////////////////////////////////////////////////////////
unsigned char MessageAckMarshaller::getDataStructureType() const {
    return MessageAck::ID_MESSAGEACK;
}

///////////////////////////////////////////////////////////////////////////////
void MessageAckMarshaller::tightUnmarshal( OpenWireFormat* wireFormat, DataStructure* dataStructure, DataInputStream* dataIn, BooleanStream* bs ) throw( decaf::io::IOException ) {

    try {

        BaseCommandMarshaller::tightUnmarshal( wireFormat, dataStructure, dataIn, bs );

        MessageAck* info =
            dynamic_cast<MessageAck*>( dataStructure );
        info->setDestination( dynamic_cast< ActiveMQDestination* >(
            tightUnmarshalCachedObject( wireFormat, dataIn, bs ) ) );
        info->setTransactionId( dynamic_cast< TransactionId* >(
            tightUnmarshalCachedObject( wireFormat, dataIn, bs ) ) );
        info->setConsumerId( dynamic_cast< ConsumerId* >(
            tightUnmarshalCachedObject( wireFormat, dataIn, bs ) ) );
        info->setAckType( dataIn->readByte() );
        info->setFirstMessageId( dynamic_cast< MessageId* >(
            tightUnmarshalNestedObject( wireFormat, dataIn, bs ) ) );
        info->setLastMessageId( dynamic_cast< MessageId* >(
            tightUnmarshalNestedObject( wireFormat, dataIn, bs ) ) );
        info->setMessageCount( dataIn->readInt() );
    }
    AMQ_CATCH_RETHROW( decaf::io::IOException )
    AMQ_CATCH_EXCEPTION_CONVERT( exceptions::ActiveMQException, decaf::io::IOException )
    AMQ_CATCHALL_THROW( decaf::io::IOException )
}

///////////////////////////////////////////////////////////////////////////////
int MessageAckMarshaller::tightMarshal1( OpenWireFormat* wireFormat, DataStructure* dataStructure, BooleanStream* bs ) throw( decaf::io::IOException ) {

    try {

        MessageAck* info =
            dynamic_cast<MessageAck*>( dataStructure );

        int rc = BaseCommandMarshaller::tightMarshal1( wireFormat, dataStructure, bs );
        rc += tightMarshalCachedObject1( wireFormat, info->getDestination(), bs );
        rc += tightMarshalCachedObject1( wireFormat, info->getTransactionId(), bs );
        rc += tightMarshalCachedObject1( wireFormat, info->getConsumerId(), bs );
        rc += tightMarshalNestedObject1( wireFormat, info->getFirstMessageId(), bs );
        rc += tightMarshalNestedObject1( wireFormat, info->getLastMessageId(), bs );

        return rc + 5;
    }
    AMQ_CATCH_RETHROW( decaf::io::IOException )
    AMQ_CATCH_EXCEPTION_CONVERT( exceptions::ActiveMQException, decaf::io::IOException )
    AMQ_CATCHALL_THROW( decaf::io::IOException )
}

///////////////////////////////////////////////////////////////////////////////
void MessageAckMarshaller::tightMarshal2( OpenWireFormat* wireFormat, DataStructure* dataStructure, DataOutputStream* dataOut, BooleanStream* bs ) throw( decaf::io::IOException ) {

    try {

        BaseCommandMarshaller::tightMarshal2( wireFormat, dataStructure, dataOut, bs );

        MessageAck* info =
            dynamic_cast<MessageAck*>( dataStructure );
        tightMarshalCachedObject2( wireFormat, info->getDestination(), dataOut, bs );
        tightMarshalCachedObject2( wireFormat, info->getTransactionId(), dataOut, bs );
        tightMarshalCachedObject2( wireFormat, info->getConsumerId(), dataOut, bs );
        dataOut->write( info->getAckType() );
        tightMarshalNestedObject2( wireFormat, info->getFirstMessageId(), dataOut, bs );
        tightMarshalNestedObject2( wireFormat, info->getLastMessageId(), dataOut, bs );
        dataOut->writeInt( info->getMessageCount() );
    }
    AMQ_CATCH_RETHROW( decaf::io::IOException )
    AMQ_CATCH_EXCEPTION_CONVERT( exceptions::ActiveMQException, decaf::io::IOException )
    AMQ_CATCHALL_THROW( decaf::io::IOException )
}

///////////////////////////////////////////////////////////////////////////////
void MessageAckMarshaller::looseUnmarshal( OpenWireFormat* wireFormat, DataStructure* dataStructure, DataInputStream* dataIn ) throw( decaf::io::IOException ) {

    try {

        BaseCommandMarshaller::looseUnmarshal( wireFormat, dataStructure, dataIn );
        MessageAck* info =
            dynamic_cast<MessageAck*>( dataStructure );
        info->setDestination( dynamic_cast< ActiveMQDestination* >( 
            looseUnmarshalCachedObject( wireFormat, dataIn ) ) );
        info->setTransactionId( dynamic_cast< TransactionId* >( 
            looseUnmarshalCachedObject( wireFormat, dataIn ) ) );
        info->setConsumerId( dynamic_cast< ConsumerId* >( 
            looseUnmarshalCachedObject( wireFormat, dataIn ) ) );
        info->setAckType( dataIn->readByte() );
        info->setFirstMessageId( dynamic_cast< MessageId* >( 
            looseUnmarshalNestedObject( wireFormat, dataIn ) ) );
        info->setLastMessageId( dynamic_cast< MessageId* >( 
            looseUnmarshalNestedObject( wireFormat, dataIn ) ) );
        info->setMessageCount( dataIn->readInt() );
    }
    AMQ_CATCH_RETHROW( decaf::io::IOException )
    AMQ_CATCH_EXCEPTION_CONVERT( exceptions::ActiveMQException, decaf::io::IOException )
    AMQ_CATCHALL_THROW( decaf::io::IOException )
}

///////////////////////////////////////////////////////////////////////////////
void MessageAckMarshaller::looseMarshal( OpenWireFormat* wireFormat, DataStructure* dataStructure, DataOutputStream* dataOut ) throw( decaf::io::IOException ) {

    try {

        MessageAck* info =
            dynamic_cast<MessageAck*>( dataStructure );
        BaseCommandMarshaller::looseMarshal( wireFormat, dataStructure, dataOut );

        looseMarshalCachedObject( wireFormat, info->getDestination(), dataOut );
        looseMarshalCachedObject( wireFormat, info->getTransactionId(), dataOut );
        looseMarshalCachedObject( wireFormat, info->getConsumerId(), dataOut );
        dataOut->write( info->getAckType() );
        looseMarshalNestedObject( wireFormat, info->getFirstMessageId(), dataOut );
        looseMarshalNestedObject( wireFormat, info->getLastMessageId(), dataOut );
        dataOut->writeInt( info->getMessageCount() );
    }
    AMQ_CATCH_RETHROW( decaf::io::IOException )
    AMQ_CATCH_EXCEPTION_CONVERT( exceptions::ActiveMQException, decaf::io::IOException )
    AMQ_CATCHALL_THROW( decaf::io::IOException )
}

