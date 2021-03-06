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

#ifndef _ACTIVEMQ_TRANSPORT_TRANSPORTFACTORYMAP_H_
#define _ACTIVEMQ_TRANSPORT_TRANSPORTFACTORYMAP_H_

#include <activemq/transport/TransportFactory.h>
#include <activemq/util/Config.h>

#include <map>
#include <vector>
#include <string>

namespace activemq{
namespace transport{

    /**
     * The TransportFactoryMap contains keys that map to specific versions
     * of the TransportFactory class which create a particular type of
     * Transport.
     */
    class AMQCPP_API TransportFactoryMap{
    private:

        // Map of Factories
        std::map<std::string, TransportFactory*> factoryMap;

    private:

        // Hidden Contrustor, prevents instantiation
        TransportFactoryMap() {}

        // Hidden Destructor.
        virtual ~TransportFactoryMap() {}

        // Hidden Copy Constructore
        TransportFactoryMap( const TransportFactoryMap& factoryMap );

        // Hidden Assignment operator
        TransportFactoryMap& operator=( const TransportFactoryMap& factoryMap );

    public:

        /**
         * Gets a singleton instance of this class.
         */
        static TransportFactoryMap& getInstance();

        /**
         * Registers a new Transport Factory with this map
         * @param name to associate the factory with
         * @param factory to store.
         */
        void registerTransportFactory( const std::string& name,
                                       TransportFactory* factory );

        /**
         * Unregisters a Transport Factory with this map
         * @param name of the factory to remove
         */
        void unregisterTransportFactory( const std::string& name );

        /**
         * Lookup the named factory in the Map
         * @param name the factory name to lookup
         * @return the factory assciated with the name, or NULL
         */
        TransportFactory* lookup( const std::string& name );

        /**
         * Fetch a list of factory names that this Map contains
         * @param factoryList vector object to receive the list
         * @returns count of factories.
         */
        std::size_t getFactoryNames( std::vector< std::string >& factoryList );

    };

}}

#endif /*_ACTIVEMQ_TRANSPORT_TRANSPORTFACTORYMAP_H_*/
