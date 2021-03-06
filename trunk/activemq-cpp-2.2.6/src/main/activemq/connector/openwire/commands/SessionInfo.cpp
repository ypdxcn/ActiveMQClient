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
#include <activemq/connector/openwire/commands/SessionInfo.h>
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
 *  Command and marshaling code for OpenWire format for SessionInfo
 *
 *
 *  NOTE!: This file is autogenerated - do not modify!
 *         if you need to make a change, please see the Java Classes in the
 *         activemq-core module
 *
 */
////////////////////////////////////////////////////////////////////////////////
SessionInfo::SessionInfo()
{
    this->sessionId = NULL;
}

////////////////////////////////////////////////////////////////////////////////
SessionInfo::~SessionInfo()
{
    delete this->sessionId;
}

////////////////////////////////////////////////////////////////////////////////
SessionInfo* SessionInfo::cloneDataStructure() const {
    std::auto_ptr<SessionInfo> sessionInfo( new SessionInfo() );

    // Copy the data from the base class or classes
    sessionInfo->copyDataStructure( this );

    return sessionInfo.release();
}

////////////////////////////////////////////////////////////////////////////////
void SessionInfo::copyDataStructure( const DataStructure* src ) {

    // Copy the data of the base class or classes
    BaseCommand<transport::Command>::copyDataStructure( src );

    const SessionInfo* srcPtr = dynamic_cast<const SessionInfo*>( src );

    if( srcPtr == NULL || src == NULL ) {

        throw decaf::lang::exceptions::NullPointerException(
            __FILE__, __LINE__,
            "SessionInfo::copyDataStructure - src is NULL or invalid" );
    }
    if( srcPtr->getSessionId() != NULL ) {
        this->setSessionId(
            dynamic_cast<SessionId*>(
                srcPtr->getSessionId()->cloneDataStructure() ) );
    }
}

////////////////////////////////////////////////////////////////////////////////
unsigned char SessionInfo::getDataStructureType() const {
    return SessionInfo::ID_SESSIONINFO;
}

////////////////////////////////////////////////////////////////////////////////
std::string SessionInfo::toString() const {

    ostringstream stream;

    stream << "Begin Class = SessionInfo" << std::endl;
    stream << " Value of SessionInfo::ID_SESSIONINFO = 4" << std::endl;
    stream << " Value of SessionId is Below:" << std::endl;
    if( this->getSessionId() != NULL ) {
        stream << this->getSessionId()->toString() << std::endl;
    } else {
        stream << "   Object is NULL" << std::endl;
    }
    stream << BaseCommand<transport::Command>::toString();
    stream << "End Class = SessionInfo" << std::endl;

    return stream.str();
}

////////////////////////////////////////////////////////////////////////////////
bool SessionInfo::equals( const DataStructure* value ) const {
    const SessionInfo* valuePtr = dynamic_cast<const SessionInfo*>( value );

    if( valuePtr == NULL || value == NULL ) {
        return false;
    }
    if( this->getSessionId() != NULL ) {
        if( !this->getSessionId()->equals( valuePtr->getSessionId() ) ) {
            return false;
        }
    } else if( valuePtr->getSessionId() != NULL ) {
        return false;
    }
    if( !BaseCommand<transport::Command>::equals( value ) ) {
        return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
const SessionId* SessionInfo::getSessionId() const {
    return sessionId;
}

////////////////////////////////////////////////////////////////////////////////
SessionId* SessionInfo::getSessionId() {
    return sessionId;
}

////////////////////////////////////////////////////////////////////////////////
void SessionInfo::setSessionId(SessionId* sessionId ) {
    this->sessionId = sessionId;
}

