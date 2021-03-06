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
#include "LogManager.h"

#include <decaf/util/logging/PropertiesChangeListener.h>
#include <decaf/util/concurrent/Concurrent.h>
#include <decaf/util/Config.h>

#include <algorithm>

using namespace std;
using namespace decaf;
using namespace decaf::util;
using namespace decaf::util::logging;

////////////////////////////////////////////////////////////////////////////////
concurrent::Mutex LogManager::mutex;
LogManager* LogManager::instance = NULL;
unsigned int LogManager::refCount = 0;

////////////////////////////////////////////////////////////////////////////////
LogManager::~LogManager()
{
    // TODO - Delete all the loggers.
}

////////////////////////////////////////////////////////////////////////////////
void LogManager::setProperties( const Properties* properties )
{
    // Copy the properties
    this->properties.copy(properties);

    // Update the configuration of the loggers.
    // TODO
}

////////////////////////////////////////////////////////////////////////////////
void LogManager::addPropertyChangeListener(
    PropertyChangeListener* listener )
{
    if( find(listeners.begin(), listeners.end(), listener) == listeners.end() ) {
        listeners.push_back( listener );
    }
}

////////////////////////////////////////////////////////////////////////////////
void LogManager::removePropertyChangeListener(
    PropertyChangeListener* listener )
{
    listeners.remove( listener );
}

////////////////////////////////////////////////////////////////////////////////
Logger* LogManager::getLogger( const std::string& name DECAF_UNUSED ) {
    return NULL;
}

////////////////////////////////////////////////////////////////////////////////
int LogManager::getLoggerNames( const std::vector<std::string>& names  DECAF_UNUSED ) {
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
LogManager* LogManager::getInstance() {

    synchronized( &mutex ) {
        if( instance == NULL ) {
            instance = new LogManager();
        }

        refCount++;

        return instance;
    }

    return NULL;
}

////////////////////////////////////////////////////////////////////////////////
void LogManager::returnInstance() {

    synchronized( &mutex ) {
        if( refCount == 0 ) {
            return ;
        }

        refCount--;

        if( refCount == 0 ) {
            delete instance;
            instance = NULL;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void LogManager::destroy()
{
    if( instance != NULL ) {

        synchronized( &mutex ) {
            delete instance;
            instance = NULL;
            refCount = 0;
        }
    }
}
