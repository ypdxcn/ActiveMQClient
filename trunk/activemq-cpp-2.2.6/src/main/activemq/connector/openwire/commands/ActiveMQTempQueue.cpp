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
#include <activemq/connector/openwire/commands/ActiveMQTempQueue.h>

using namespace std;
using namespace activemq;
using namespace activemq::connector;
using namespace activemq::connector::openwire;
using namespace activemq::connector::openwire::commands;

////////////////////////////////////////////////////////////////////////////////
ActiveMQTempQueue::ActiveMQTempQueue() : ActiveMQTempDestination() {
}

////////////////////////////////////////////////////////////////////////////////
ActiveMQTempQueue::ActiveMQTempQueue( const std::string& name ) :
    ActiveMQTempDestination( name )
{}

////////////////////////////////////////////////////////////////////////////////
ActiveMQTempQueue::~ActiveMQTempQueue() {
    try {
        this->close();
    }
    AMQ_CATCHALL_NOTHROW()
}

////////////////////////////////////////////////////////////////////////////////
unsigned char ActiveMQTempQueue::getDataStructureType() const {
    return ActiveMQTempQueue::ID_ACTIVEMQTEMPQUEUE;
}
