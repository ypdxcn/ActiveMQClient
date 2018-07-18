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
#include <activemq/connector/openwire/commands/JournalTopicAck.h>
#include <activemq/exceptions/ActiveMQException.h>
#include <decaf/lang/exceptions/NullPointerException.h>
#include <memory>

using namespace std;
using namespace activemq;
using namespace activemq::exceptions;
using namespace activemq::connector;
using namespace activemq::connector::openwire;
using namespace activemq::connector::openwire::commands;
using namespace decaf::lang::exceptions;

/*
 *
 *  Command and marshaling code for OpenWire format for JournalTopicAck
 *
 *
 *  NOTE!: This file is autogenerated - do not modify!
 *         if you need to make a change, please see the Java Classes in the
 *         activemq-core module
 *
 */
////////////////////////////////////////////////////////////////////////////////
JournalTopicAck::JournalTopicAck()
{
    this->destination = NULL;
    this->messageId = NULL;
    this->messageSequenceId = 0;
    this->subscritionName = "";
    this->clientId = "";
    this->transactionId = NULL;
}

////////////////////////////////////////////////////////////////////////////////
JournalTopicAck::~JournalTopicAck()
{
    delete this->destination;
    delete this->messageId;
    delete this->transactionId;
}

////////////////////////////////////////////////////////////////////////////////
JournalTopicAck* JournalTopicAck::cloneDataStructure() const {
    std::auto_ptr<JournalTopicAck> journalTopicAck( new JournalTopicAck() );

    // Copy the data from the base class or classes
    journalTopicAck->copyDataStructure( this );

    return journalTopicAck.release();
}

////////////////////////////////////////////////////////////////////////////////
void JournalTopicAck::copyDataStructure( const DataStructure* src ) {

    // Copy the data of the base class or classes
    BaseDataStructure::copyDataStructure( src );

    const JournalTopicAck* srcPtr = dynamic_cast<const JournalTopicAck*>( src );

    if( srcPtr == NULL || src == NULL ) {

        throw decaf::lang::exceptions::NullPointerException(
            __FILE__, __LINE__,
            "JournalTopicAck::copyDataStructure - src is NULL or invalid" );
    }
    if( srcPtr->getDestination() != NULL ) {
        this->setDestination(
            dynamic_cast<ActiveMQDestination*>(
                srcPtr->getDestination()->cloneDataStructure() ) );
    }
    if( srcPtr->getMessageId() != NULL ) {
        this->setMessageId(
            dynamic_cast<MessageId*>(
                srcPtr->getMessageId()->cloneDataStructure() ) );
    }
    this->setMessageSequenceId( srcPtr->getMessageSequenceId() );
    this->setSubscritionName( srcPtr->getSubscritionName() );
    this->setClientId( srcPtr->getClientId() );
    if( srcPtr->getTransactionId() != NULL ) {
        this->setTransactionId(
            dynamic_cast<TransactionId*>(
                srcPtr->getTransactionId()->cloneDataStructure() ) );
    }
}

////////////////////////////////////////////////////////////////////////////////
unsigned char JournalTopicAck::getDataStructureType() const {
    return JournalTopicAck::ID_JOURNALTOPICACK;
}

////////////////////////////////////////////////////////////////////////////////
std::string JournalTopicAck::toString() const {

    ostringstream stream;

    stream << "Begin Class = JournalTopicAck" << std::endl;
    stream << " Value of JournalTopicAck::ID_JOURNALTOPICACK = 50" << std::endl;
    stream << " Value of Destination is Below:" << std::endl;
    if( this->getDestination() != NULL ) {
        stream << this->getDestination()->toString() << std::endl;
    } else {
        stream << "   Object is NULL" << std::endl;
    }
    stream << " Value of MessageId is Below:" << std::endl;
    if( this->getMessageId() != NULL ) {
        stream << this->getMessageId()->toString() << std::endl;
    } else {
        stream << "   Object is NULL" << std::endl;
    }
    stream << " Value of MessageSequenceId = " << this->getMessageSequenceId() << std::endl;
    stream << " Value of SubscritionName = " << this->getSubscritionName() << std::endl;
    stream << " Value of ClientId = " << this->getClientId() << std::endl;
    stream << " Value of TransactionId is Below:" << std::endl;
    if( this->getTransactionId() != NULL ) {
        stream << this->getTransactionId()->toString() << std::endl;
    } else {
        stream << "   Object is NULL" << std::endl;
    }
    stream << BaseDataStructure::toString();
    stream << "End Class = JournalTopicAck" << std::endl;

    return stream.str();
}

////////////////////////////////////////////////////////////////////////////////
bool JournalTopicAck::equals( const DataStructure* value ) const {
    const JournalTopicAck* valuePtr = dynamic_cast<const JournalTopicAck*>( value );

    if( valuePtr == NULL || value == NULL ) {
        return false;
    }
    if( this->getDestination() != NULL ) {
        if( !this->getDestination()->equals( valuePtr->getDestination() ) ) {
            return false;
        }
    } else if( valuePtr->getDestination() != NULL ) {
        return false;
    }
    if( this->getMessageId() != NULL ) {
        if( !this->getMessageId()->equals( valuePtr->getMessageId() ) ) {
            return false;
        }
    } else if( valuePtr->getMessageId() != NULL ) {
        return false;
    }
    if( this->getMessageSequenceId() != valuePtr->getMessageSequenceId() ) {
        return false;
    }
    if( this->getSubscritionName() != valuePtr->getSubscritionName() ) {
        return false;
    }
    if( this->getClientId() != valuePtr->getClientId() ) {
        return false;
    }
    if( this->getTransactionId() != NULL ) {
        if( !this->getTransactionId()->equals( valuePtr->getTransactionId() ) ) {
            return false;
        }
    } else if( valuePtr->getTransactionId() != NULL ) {
        return false;
    }
    if( !BaseDataStructure::equals( value ) ) {
        return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
const ActiveMQDestination* JournalTopicAck::getDestination() const {
    return destination;
}

////////////////////////////////////////////////////////////////////////////////
ActiveMQDestination* JournalTopicAck::getDestination() {
    return destination;
}

////////////////////////////////////////////////////////////////////////////////
void JournalTopicAck::setDestination(ActiveMQDestination* destination ) {
    this->destination = destination;
}

////////////////////////////////////////////////////////////////////////////////
const MessageId* JournalTopicAck::getMessageId() const {
    return messageId;
}

////////////////////////////////////////////////////////////////////////////////
MessageId* JournalTopicAck::getMessageId() {
    return messageId;
}

////////////////////////////////////////////////////////////////////////////////
void JournalTopicAck::setMessageId(MessageId* messageId ) {
    this->messageId = messageId;
}

////////////////////////////////////////////////////////////////////////////////
long long JournalTopicAck::getMessageSequenceId() const {
    return messageSequenceId;
}

////////////////////////////////////////////////////////////////////////////////
void JournalTopicAck::setMessageSequenceId(long long messageSequenceId ) {
    this->messageSequenceId = messageSequenceId;
}

////////////////////////////////////////////////////////////////////////////////
const std::string& JournalTopicAck::getSubscritionName() const {
    return subscritionName;
}

////////////////////////////////////////////////////////////////////////////////
std::string& JournalTopicAck::getSubscritionName() {
    return subscritionName;
}

////////////////////////////////////////////////////////////////////////////////
void JournalTopicAck::setSubscritionName(const std::string& subscritionName ) {
    this->subscritionName = subscritionName;
}

////////////////////////////////////////////////////////////////////////////////
const std::string& JournalTopicAck::getClientId() const {
    return clientId;
}

////////////////////////////////////////////////////////////////////////////////
std::string& JournalTopicAck::getClientId() {
    return clientId;
}

////////////////////////////////////////////////////////////////////////////////
void JournalTopicAck::setClientId(const std::string& clientId ) {
    this->clientId = clientId;
}

////////////////////////////////////////////////////////////////////////////////
const TransactionId* JournalTopicAck::getTransactionId() const {
    return transactionId;
}

////////////////////////////////////////////////////////////////////////////////
TransactionId* JournalTopicAck::getTransactionId() {
    return transactionId;
}

////////////////////////////////////////////////////////////////////////////////
void JournalTopicAck::setTransactionId(TransactionId* transactionId ) {
    this->transactionId = transactionId;
}

