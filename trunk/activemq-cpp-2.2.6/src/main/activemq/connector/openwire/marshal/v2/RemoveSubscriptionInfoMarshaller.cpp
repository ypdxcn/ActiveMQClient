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

#include <activemq/connector/openwire/marshal/v2/RemoveSubscriptionInfoMarshaller.h>

#include <activemq/connector/openwire/commands/RemoveSubscriptionInfo.h>
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
using namespace activemq::connector::openwire::marshal::v2;
using namespace decaf;
using namespace decaf::io;

///////////////////////////////////////////////////////////////////////////////
DataStructure* RemoveSubscriptionInfoMarshaller::createObject() const {
    return new RemoveSubscriptionInfo();
}

///////////////////////////////////////////////////////////////////////////////
unsigned char RemoveSubscriptionInfoMarshaller::getDataStructureType() const {
    return RemoveSubscriptionInfo::ID_REMOVESUBSCRIPTIONINFO;
}

///////////////////////////////////////////////////////////////////////////////
void RemoveSubscriptionInfoMarshaller::tightUnmarshal( OpenWireFormat* wireFormat, DataStructure* dataStructure, DataInputStream* dataIn, BooleanStream* bs ) throw( decaf::io::IOException ) {

    try {

        BaseCommandMarshaller::tightUnmarshal( wireFormat, dataStructure, dataIn, bs );

        RemoveSubscriptionInfo* info =
            dynamic_cast<RemoveSubscriptionInfo*>( dataStructure );
        info->setConnectionId( dynamic_cast< ConnectionId* >(
            tightUnmarshalCachedObject( wireFormat, dataIn, bs ) ) );
        info->setSubcriptionName( tightUnmarshalString( dataIn, bs ) );
        info->setClientId( tightUnmarshalString( dataIn, bs ) );
    }
    AMQ_CATCH_RETHROW( decaf::io::IOException )
    AMQ_CATCH_EXCEPTION_CONVERT( exceptions::ActiveMQException, decaf::io::IOException )
    AMQ_CATCHALL_THROW( decaf::io::IOException )
}

///////////////////////////////////////////////////////////////////////////////
int RemoveSubscriptionInfoMarshaller::tightMarshal1( OpenWireFormat* wireFormat, DataStructure* dataStructure, BooleanStream* bs ) throw( decaf::io::IOException ) {

    try {

        RemoveSubscriptionInfo* info =
            dynamic_cast<RemoveSubscriptionInfo*>( dataStructure );

        int rc = BaseCommandMarshaller::tightMarshal1( wireFormat, dataStructure, bs );
        rc += tightMarshalCachedObject1( wireFormat, info->getConnectionId(), bs );
        rc += tightMarshalString1( info->getSubcriptionName(), bs );
        rc += tightMarshalString1( info->getClientId(), bs );

        return rc + 0;
    }
    AMQ_CATCH_RETHROW( decaf::io::IOException )
    AMQ_CATCH_EXCEPTION_CONVERT( exceptions::ActiveMQException, decaf::io::IOException )
    AMQ_CATCHALL_THROW( decaf::io::IOException )
}

///////////////////////////////////////////////////////////////////////////////
void RemoveSubscriptionInfoMarshaller::tightMarshal2( OpenWireFormat* wireFormat, DataStructure* dataStructure, DataOutputStream* dataOut, BooleanStream* bs ) throw( decaf::io::IOException ) {

    try {

        BaseCommandMarshaller::tightMarshal2( wireFormat, dataStructure, dataOut, bs );

        RemoveSubscriptionInfo* info =
            dynamic_cast<RemoveSubscriptionInfo*>( dataStructure );
        tightMarshalCachedObject2( wireFormat, info->getConnectionId(), dataOut, bs );
        tightMarshalString2( info->getSubcriptionName(), dataOut, bs );
        tightMarshalString2( info->getClientId(), dataOut, bs );
    }
    AMQ_CATCH_RETHROW( decaf::io::IOException )
    AMQ_CATCH_EXCEPTION_CONVERT( exceptions::ActiveMQException, decaf::io::IOException )
    AMQ_CATCHALL_THROW( decaf::io::IOException )
}

///////////////////////////////////////////////////////////////////////////////
void RemoveSubscriptionInfoMarshaller::looseUnmarshal( OpenWireFormat* wireFormat, DataStructure* dataStructure, DataInputStream* dataIn ) throw( decaf::io::IOException ) {

    try {

        BaseCommandMarshaller::looseUnmarshal( wireFormat, dataStructure, dataIn );
        RemoveSubscriptionInfo* info =
            dynamic_cast<RemoveSubscriptionInfo*>( dataStructure );
        info->setConnectionId( dynamic_cast< ConnectionId* >( 
            looseUnmarshalCachedObject( wireFormat, dataIn ) ) );
        info->setSubcriptionName( looseUnmarshalString( dataIn ) );
        info->setClientId( looseUnmarshalString( dataIn ) );
    }
    AMQ_CATCH_RETHROW( decaf::io::IOException )
    AMQ_CATCH_EXCEPTION_CONVERT( exceptions::ActiveMQException, decaf::io::IOException )
    AMQ_CATCHALL_THROW( decaf::io::IOException )
}

///////////////////////////////////////////////////////////////////////////////
void RemoveSubscriptionInfoMarshaller::looseMarshal( OpenWireFormat* wireFormat, DataStructure* dataStructure, DataOutputStream* dataOut ) throw( decaf::io::IOException ) {

    try {

        RemoveSubscriptionInfo* info =
            dynamic_cast<RemoveSubscriptionInfo*>( dataStructure );
        BaseCommandMarshaller::looseMarshal( wireFormat, dataStructure, dataOut );

        looseMarshalCachedObject( wireFormat, info->getConnectionId(), dataOut );
        looseMarshalString( info->getSubcriptionName(), dataOut );
        looseMarshalString( info->getClientId(), dataOut );
    }
    AMQ_CATCH_RETHROW( decaf::io::IOException )
    AMQ_CATCH_EXCEPTION_CONVERT( exceptions::ActiveMQException, decaf::io::IOException )
    AMQ_CATCHALL_THROW( decaf::io::IOException )
}

