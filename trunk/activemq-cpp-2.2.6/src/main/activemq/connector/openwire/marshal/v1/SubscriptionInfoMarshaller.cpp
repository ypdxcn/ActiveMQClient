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

#include <activemq/connector/openwire/marshal/v1/SubscriptionInfoMarshaller.h>

#include <activemq/connector/openwire/commands/SubscriptionInfo.h>
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
using namespace activemq::connector::openwire::marshal::v1;
using namespace decaf;
using namespace decaf::io;

///////////////////////////////////////////////////////////////////////////////
DataStructure* SubscriptionInfoMarshaller::createObject() const {
    return new SubscriptionInfo();
}

///////////////////////////////////////////////////////////////////////////////
unsigned char SubscriptionInfoMarshaller::getDataStructureType() const {
    return SubscriptionInfo::ID_SUBSCRIPTIONINFO;
}

///////////////////////////////////////////////////////////////////////////////
void SubscriptionInfoMarshaller::tightUnmarshal( OpenWireFormat* wireFormat, DataStructure* dataStructure, DataInputStream* dataIn, BooleanStream* bs ) throw( decaf::io::IOException ) {

    try {

        BaseDataStreamMarshaller::tightUnmarshal( wireFormat, dataStructure, dataIn, bs );

        SubscriptionInfo* info =
            dynamic_cast<SubscriptionInfo*>( dataStructure );
        info->setClientId( tightUnmarshalString( dataIn, bs ) );
        info->setDestination( dynamic_cast< ActiveMQDestination* >(
            tightUnmarshalCachedObject( wireFormat, dataIn, bs ) ) );
        info->setSelector( tightUnmarshalString( dataIn, bs ) );
        info->setSubcriptionName( tightUnmarshalString( dataIn, bs ) );
        info->setSubscribedDestination( dynamic_cast< ActiveMQDestination* >(
            tightUnmarshalNestedObject( wireFormat, dataIn, bs ) ) );
    }
    AMQ_CATCH_RETHROW( decaf::io::IOException )
    AMQ_CATCH_EXCEPTION_CONVERT( exceptions::ActiveMQException, decaf::io::IOException )
    AMQ_CATCHALL_THROW( decaf::io::IOException )
}

///////////////////////////////////////////////////////////////////////////////
int SubscriptionInfoMarshaller::tightMarshal1( OpenWireFormat* wireFormat, DataStructure* dataStructure, BooleanStream* bs ) throw( decaf::io::IOException ) {

    try {

        SubscriptionInfo* info =
            dynamic_cast<SubscriptionInfo*>( dataStructure );

        int rc = BaseDataStreamMarshaller::tightMarshal1( wireFormat, dataStructure, bs );
        rc += tightMarshalString1( info->getClientId(), bs );
        rc += tightMarshalCachedObject1( wireFormat, info->getDestination(), bs );
        rc += tightMarshalString1( info->getSelector(), bs );
        rc += tightMarshalString1( info->getSubcriptionName(), bs );
        rc += tightMarshalNestedObject1( wireFormat, info->getSubscribedDestination(), bs );

        return rc + 0;
    }
    AMQ_CATCH_RETHROW( decaf::io::IOException )
    AMQ_CATCH_EXCEPTION_CONVERT( exceptions::ActiveMQException, decaf::io::IOException )
    AMQ_CATCHALL_THROW( decaf::io::IOException )
}

///////////////////////////////////////////////////////////////////////////////
void SubscriptionInfoMarshaller::tightMarshal2( OpenWireFormat* wireFormat, DataStructure* dataStructure, DataOutputStream* dataOut, BooleanStream* bs ) throw( decaf::io::IOException ) {

    try {

        BaseDataStreamMarshaller::tightMarshal2( wireFormat, dataStructure, dataOut, bs );

        SubscriptionInfo* info =
            dynamic_cast<SubscriptionInfo*>( dataStructure );
        tightMarshalString2( info->getClientId(), dataOut, bs );
        tightMarshalCachedObject2( wireFormat, info->getDestination(), dataOut, bs );
        tightMarshalString2( info->getSelector(), dataOut, bs );
        tightMarshalString2( info->getSubcriptionName(), dataOut, bs );
        tightMarshalNestedObject2( wireFormat, info->getSubscribedDestination(), dataOut, bs );
    }
    AMQ_CATCH_RETHROW( decaf::io::IOException )
    AMQ_CATCH_EXCEPTION_CONVERT( exceptions::ActiveMQException, decaf::io::IOException )
    AMQ_CATCHALL_THROW( decaf::io::IOException )
}

///////////////////////////////////////////////////////////////////////////////
void SubscriptionInfoMarshaller::looseUnmarshal( OpenWireFormat* wireFormat, DataStructure* dataStructure, DataInputStream* dataIn ) throw( decaf::io::IOException ) {

    try {

        BaseDataStreamMarshaller::looseUnmarshal( wireFormat, dataStructure, dataIn );
        SubscriptionInfo* info =
            dynamic_cast<SubscriptionInfo*>( dataStructure );
        info->setClientId( looseUnmarshalString( dataIn ) );
        info->setDestination( dynamic_cast< ActiveMQDestination* >( 
            looseUnmarshalCachedObject( wireFormat, dataIn ) ) );
        info->setSelector( looseUnmarshalString( dataIn ) );
        info->setSubcriptionName( looseUnmarshalString( dataIn ) );
        info->setSubscribedDestination( dynamic_cast< ActiveMQDestination* >( 
            looseUnmarshalNestedObject( wireFormat, dataIn ) ) );
    }
    AMQ_CATCH_RETHROW( decaf::io::IOException )
    AMQ_CATCH_EXCEPTION_CONVERT( exceptions::ActiveMQException, decaf::io::IOException )
    AMQ_CATCHALL_THROW( decaf::io::IOException )
}

///////////////////////////////////////////////////////////////////////////////
void SubscriptionInfoMarshaller::looseMarshal( OpenWireFormat* wireFormat, DataStructure* dataStructure, DataOutputStream* dataOut ) throw( decaf::io::IOException ) {

    try {

        SubscriptionInfo* info =
            dynamic_cast<SubscriptionInfo*>( dataStructure );
        BaseDataStreamMarshaller::looseMarshal( wireFormat, dataStructure, dataOut );

        looseMarshalString( info->getClientId(), dataOut );
        looseMarshalCachedObject( wireFormat, info->getDestination(), dataOut );
        looseMarshalString( info->getSelector(), dataOut );
        looseMarshalString( info->getSubcriptionName(), dataOut );
        looseMarshalNestedObject( wireFormat, info->getSubscribedDestination(), dataOut );
    }
    AMQ_CATCH_RETHROW( decaf::io::IOException )
    AMQ_CATCH_EXCEPTION_CONVERT( exceptions::ActiveMQException, decaf::io::IOException )
    AMQ_CATCHALL_THROW( decaf::io::IOException )
}

