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
#include <activemq/connector/openwire/commands/ConnectionControl.h>
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
 *  Command and marshaling code for OpenWire format for ConnectionControl
 *
 *
 *  NOTE!: This file is autogenerated - do not modify!
 *         if you need to make a change, please see the Java Classes in the
 *         activemq-core module
 *
 */
////////////////////////////////////////////////////////////////////////////////
ConnectionControl::ConnectionControl()
{
    this->close = false;
    this->exit = false;
    this->faultTolerant = false;
    this->resume = false;
    this->suspend = false;
}

////////////////////////////////////////////////////////////////////////////////
ConnectionControl::~ConnectionControl()
{
}

////////////////////////////////////////////////////////////////////////////////
ConnectionControl* ConnectionControl::cloneDataStructure() const {
    std::auto_ptr<ConnectionControl> connectionControl( new ConnectionControl() );

    // Copy the data from the base class or classes
    connectionControl->copyDataStructure( this );

    return connectionControl.release();
}

////////////////////////////////////////////////////////////////////////////////
void ConnectionControl::copyDataStructure( const DataStructure* src ) {

    // Copy the data of the base class or classes
    BaseCommand<transport::Command>::copyDataStructure( src );

    const ConnectionControl* srcPtr = dynamic_cast<const ConnectionControl*>( src );

    if( srcPtr == NULL || src == NULL ) {

        throw decaf::lang::exceptions::NullPointerException(
            __FILE__, __LINE__,
            "ConnectionControl::copyDataStructure - src is NULL or invalid" );
    }
    this->setClose( srcPtr->isClose() );
    this->setExit( srcPtr->isExit() );
    this->setFaultTolerant( srcPtr->isFaultTolerant() );
    this->setResume( srcPtr->isResume() );
    this->setSuspend( srcPtr->isSuspend() );
}

////////////////////////////////////////////////////////////////////////////////
unsigned char ConnectionControl::getDataStructureType() const {
    return ConnectionControl::ID_CONNECTIONCONTROL;
}

////////////////////////////////////////////////////////////////////////////////
std::string ConnectionControl::toString() const {

    ostringstream stream;

    stream << "Begin Class = ConnectionControl" << std::endl;
    stream << " Value of ConnectionControl::ID_CONNECTIONCONTROL = 18" << std::endl;
    stream << " Value of Close = " << this->isClose() << std::endl;
    stream << " Value of Exit = " << this->isExit() << std::endl;
    stream << " Value of FaultTolerant = " << this->isFaultTolerant() << std::endl;
    stream << " Value of Resume = " << this->isResume() << std::endl;
    stream << " Value of Suspend = " << this->isSuspend() << std::endl;
    stream << BaseCommand<transport::Command>::toString();
    stream << "End Class = ConnectionControl" << std::endl;

    return stream.str();
}

////////////////////////////////////////////////////////////////////////////////
bool ConnectionControl::equals( const DataStructure* value ) const {
    const ConnectionControl* valuePtr = dynamic_cast<const ConnectionControl*>( value );

    if( valuePtr == NULL || value == NULL ) {
        return false;
    }
    if( this->isClose() != valuePtr->isClose() ) {
        return false;
    }
    if( this->isExit() != valuePtr->isExit() ) {
        return false;
    }
    if( this->isFaultTolerant() != valuePtr->isFaultTolerant() ) {
        return false;
    }
    if( this->isResume() != valuePtr->isResume() ) {
        return false;
    }
    if( this->isSuspend() != valuePtr->isSuspend() ) {
        return false;
    }
    if( !BaseCommand<transport::Command>::equals( value ) ) {
        return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool ConnectionControl::isClose() const {
    return close;
}

////////////////////////////////////////////////////////////////////////////////
void ConnectionControl::setClose(bool close ) {
    this->close = close;
}

////////////////////////////////////////////////////////////////////////////////
bool ConnectionControl::isExit() const {
    return exit;
}

////////////////////////////////////////////////////////////////////////////////
void ConnectionControl::setExit(bool exit ) {
    this->exit = exit;
}

////////////////////////////////////////////////////////////////////////////////
bool ConnectionControl::isFaultTolerant() const {
    return faultTolerant;
}

////////////////////////////////////////////////////////////////////////////////
void ConnectionControl::setFaultTolerant(bool faultTolerant ) {
    this->faultTolerant = faultTolerant;
}

////////////////////////////////////////////////////////////////////////////////
bool ConnectionControl::isResume() const {
    return resume;
}

////////////////////////////////////////////////////////////////////////////////
void ConnectionControl::setResume(bool resume ) {
    this->resume = resume;
}

////////////////////////////////////////////////////////////////////////////////
bool ConnectionControl::isSuspend() const {
    return suspend;
}

////////////////////////////////////////////////////////////////////////////////
void ConnectionControl::setSuspend(bool suspend ) {
    this->suspend = suspend;
}

