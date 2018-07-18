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

#include <activemq/connector/openwire/marshal/v2/ConnectionInfoMarshaller.h>

#include <activemq/connector/openwire/commands/ConnectionInfo.h>
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
DataStructure* ConnectionInfoMarshaller::createObject() const {
    return new ConnectionInfo();
}

///////////////////////////////////////////////////////////////////////////////
unsigned char ConnectionInfoMarshaller::getDataStructureType() const {
    return ConnectionInfo::ID_CONNECTIONINFO;
}

///////////////////////////////////////////////////////////////////////////////
void ConnectionInfoMarshaller::tightUnmarshal( OpenWireFormat* wireFormat, DataStructure* dataStructure, DataInputStream* dataIn, BooleanStream* bs ) throw( decaf::io::IOException ) {

    try {

        BaseCommandMarshaller::tightUnmarshal( wireFormat, dataStructure, dataIn, bs );

        ConnectionInfo* info =
            dynamic_cast<ConnectionInfo*>( dataStructure );
        info->setConnectionId( dynamic_cast< ConnectionId* >(
            tightUnmarshalCachedObject( wireFormat, dataIn, bs ) ) );
        info->setClientId( tightUnmarshalString( dataIn, bs ) );
        info->setPassword( tightUnmarshalString( dataIn, bs ) );
        info->setUserName( tightUnmarshalString( dataIn, bs ) );

        if( bs->readBoolean() ) {
            short size = dataIn->readShort();
            info->getBrokerPath().reserve( size );
            for( int i = 0; i < size; i++ ) {
                info->getBrokerPath().push_back( dynamic_cast< BrokerId* >(
                    tightUnmarshalNestedObject( wireFormat, dataIn, bs ) ) );
            }
        }
        else {
            info->getBrokerPath().clear();
        }
        info->setBrokerMasterConnector( bs->readBoolean() );
        info->setManageable( bs->readBoolean() );
        info->setClientMaster( bs->readBoolean() );
    }
    AMQ_CATCH_RETHROW( decaf::io::IOException )
    AMQ_CATCH_EXCEPTION_CONVERT( exceptions::ActiveMQException, decaf::io::IOException )
    AMQ_CATCHALL_THROW( decaf::io::IOException )
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionInfoMarshaller::tightMarshal1( OpenWireFormat* wireFormat, DataStructure* dataStructure, BooleanStream* bs ) throw( decaf::io::IOException ) {

    try {

        ConnectionInfo* info =
            dynamic_cast<ConnectionInfo*>( dataStructure );

        int rc = BaseCommandMarshaller::tightMarshal1( wireFormat, dataStructure, bs );
        rc += tightMarshalCachedObject1( wireFormat, info->getConnectionId(), bs );
        rc += tightMarshalString1( info->getClientId(), bs );
        rc += tightMarshalString1( info->getPassword(), bs );
        rc += tightMarshalString1( info->getUserName(), bs );
        rc += tightMarshalObjectArray1( wireFormat, info->getBrokerPath(), bs );
        bs->writeBoolean( info->isBrokerMasterConnector() );
        bs->writeBoolean( info->isManageable() );
        bs->writeBoolean( info->isClientMaster() );

        return rc + 0;
    }
    AMQ_CATCH_RETHROW( decaf::io::IOException )
    AMQ_CATCH_EXCEPTION_CONVERT( exceptions::ActiveMQException, decaf::io::IOException )
    AMQ_CATCHALL_THROW( decaf::io::IOException )
}

///////////////////////////////////////////////////////////////////////////////
void ConnectionInfoMarshaller::tightMarshal2( OpenWireFormat* wireFormat, DataStructure* dataStructure, DataOutputStream* dataOut, BooleanStream* bs ) throw( decaf::io::IOException ) {

    try {

        BaseCommandMarshaller::tightMarshal2( wireFormat, dataStructure, dataOut, bs );

        ConnectionInfo* info =
            dynamic_cast<ConnectionInfo*>( dataStructure );
        tightMarshalCachedObject2( wireFormat, info->getConnectionId(), dataOut, bs );
        tightMarshalString2( info->getClientId(), dataOut, bs );
        tightMarshalString2( info->getPassword(), dataOut, bs );
        tightMarshalString2( info->getUserName(), dataOut, bs );
        tightMarshalObjectArray2( wireFormat, info->getBrokerPath(), dataOut, bs );
        bs->readBoolean();
        bs->readBoolean();
        bs->readBoolean();
    }
    AMQ_CATCH_RETHROW( decaf::io::IOException )
    AMQ_CATCH_EXCEPTION_CONVERT( exceptions::ActiveMQException, decaf::io::IOException )
    AMQ_CATCHALL_THROW( decaf::io::IOException )
}

///////////////////////////////////////////////////////////////////////////////
void ConnectionInfoMarshaller::looseUnmarshal( OpenWireFormat* wireFormat, DataStructure* dataStructure, DataInputStream* dataIn ) throw( decaf::io::IOException ) {

    try {

        BaseCommandMarshaller::looseUnmarshal( wireFormat, dataStructure, dataIn );
        ConnectionInfo* info =
            dynamic_cast<ConnectionInfo*>( dataStructure );
        info->setConnectionId( dynamic_cast< ConnectionId* >( 
            looseUnmarshalCachedObject( wireFormat, dataIn ) ) );
        info->setClientId( looseUnmarshalString( dataIn ) );
        info->setPassword( looseUnmarshalString( dataIn ) );
        info->setUserName( looseUnmarshalString( dataIn ) );

        if( dataIn->readBoolean() ) {
            short size = dataIn->readShort();
            info->getBrokerPath().reserve( size );
            for( int i = 0; i < size; i++ ) {
                info->getBrokerPath().push_back( dynamic_cast<BrokerId* >(
                    looseUnmarshalNestedObject( wireFormat, dataIn ) ) );
            }
        }
        else {
            info->getBrokerPath().clear();
        }
        info->setBrokerMasterConnector( dataIn->readBoolean() );
        info->setManageable( dataIn->readBoolean() );
        info->setClientMaster( dataIn->readBoolean() );
    }
    AMQ_CATCH_RETHROW( decaf::io::IOException )
    AMQ_CATCH_EXCEPTION_CONVERT( exceptions::ActiveMQException, decaf::io::IOException )
    AMQ_CATCHALL_THROW( decaf::io::IOException )
}

///////////////////////////////////////////////////////////////////////////////
void ConnectionInfoMarshaller::looseMarshal( OpenWireFormat* wireFormat, DataStructure* dataStructure, DataOutputStream* dataOut ) throw( decaf::io::IOException ) {

    try {

        ConnectionInfo* info =
            dynamic_cast<ConnectionInfo*>( dataStructure );
        BaseCommandMarshaller::looseMarshal( wireFormat, dataStructure, dataOut );

        looseMarshalCachedObject( wireFormat, info->getConnectionId(), dataOut );
        looseMarshalString( info->getClientId(), dataOut );
        looseMarshalString( info->getPassword(), dataOut );
        looseMarshalString( info->getUserName(), dataOut );
        looseMarshalObjectArray( wireFormat, info->getBrokerPath(), dataOut );
        dataOut->writeBoolean( info->isBrokerMasterConnector() );
        dataOut->writeBoolean( info->isManageable() );
        dataOut->writeBoolean( info->isClientMaster() );
    }
    AMQ_CATCH_RETHROW( decaf::io::IOException )
    AMQ_CATCH_EXCEPTION_CONVERT( exceptions::ActiveMQException, decaf::io::IOException )
    AMQ_CATCHALL_THROW( decaf::io::IOException )
}

