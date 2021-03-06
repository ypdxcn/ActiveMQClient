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

#include <activemq/util/Config.h>
#include <activemq/connector/stomp/StompConnector.h>
#include <activemq/transport/Transport.h>
#include <activemq/connector/stomp/StompTopic.h>
#include <activemq/connector/stomp/StompQueue.h>
#include <activemq/connector/stomp/commands/ConnectCommand.h>
#include <activemq/connector/stomp/commands/ErrorCommand.h>
#include <activemq/connector/stomp/commands/BeginCommand.h>
#include <activemq/connector/stomp/commands/AbortCommand.h>
#include <activemq/connector/stomp/commands/AckCommand.h>
#include <activemq/connector/stomp/commands/CommitCommand.h>
#include <activemq/connector/stomp/commands/MessageCommand.h>
#include <activemq/connector/stomp/commands/BytesMessageCommand.h>
#include <activemq/connector/stomp/commands/TextMessageCommand.h>
#include <activemq/connector/stomp/commands/ConnectedCommand.h>
#include <activemq/connector/stomp/commands/DisconnectCommand.h>
#include <activemq/connector/stomp/StompProducerInfo.h>
#include <activemq/connector/stomp/StompTransactionInfo.h>
#include <activemq/connector/stomp/StompConnectionNegotiator.h>
#include <decaf/lang/Long.h>
#include <decaf/util/concurrent/Concurrent.h>

using namespace std;
using namespace activemq;
using namespace activemq::connector;
using namespace activemq::util;
using namespace activemq::transport;
using namespace activemq::exceptions;
using namespace activemq::connector::stomp;
using namespace activemq::connector::stomp::commands;

using namespace decaf;
using namespace decaf::util;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;

////////////////////////////////////////////////////////////////////////////////
LOGDECAF_INITIALIZE( logger, StompConnector, "activemq.connector.stomp.StompConnector" )

////////////////////////////////////////////////////////////////////////////////
StompConnector::StompConnector( Transport* transport,
                                const decaf::util::Properties& properties )
    throw ( IllegalArgumentException ) {

    if( transport == NULL ) {
        throw IllegalArgumentException(
            __FILE__, __LINE__,
            "StompConnector::StompConnector - Transport cannot be NULL");
    }

    this->state = CONNECTION_STATE_DISCONNECTED;
    this->exceptionListener = NULL;
    this->messageListener = NULL;
    this->sessionManager = NULL;
    this->properties.copy( &properties );

    // Create the connection negotiator and wrap our transport
    this->transport = new StompConnectionNegotiator( transport, false );

    // Observe the transport for events.
    this->transport->setCommandListener( this );
    this->transport->setTransportExceptionListener( this );

    // Setup the reader and writer in the transport.
    this->transport->setCommandReader( &reader );
    this->transport->setCommandWriter( &writer );

    // Register ourself for those commands that we process
    addCmdListener( CommandConstants::ERROR_CMD, this );
}

////////////////////////////////////////////////////////////////////////////////
StompConnector::~StompConnector() {

    try{

        close();

        delete transport;
        delete sessionManager;
    }
    AMQ_CATCH_NOTHROW( ActiveMQException )
    AMQ_CATCHALL_NOTHROW( )
}

////////////////////////////////////////////////////////////////////////////////
void StompConnector::enforceConnected() throw ( ConnectorException ) {

    if( state != CONNECTION_STATE_CONNECTED ) {
        throw StompConnectorException(
            __FILE__, __LINE__,
            "StompConnector::enforceConnected - Not Connected!" );
    }
}

////////////////////////////////////////////////////////////////////////////////
void StompConnector::addCmdListener(
    commands::CommandConstants::CommandId commandId,
    StompCommandListener* listener ) {
    cmdListenerMap.insert( make_pair( commandId, listener ) );
}

////////////////////////////////////////////////////////////////////////////////
void StompConnector::removeCmdListener(
    commands::CommandConstants::CommandId commandId ) {
    cmdListenerMap.erase( commandId );
}

////////////////////////////////////////////////////////////////////////////////
void StompConnector::start() throw( cms::CMSException ) {

    try{

        synchronized( &mutex ) {

            if( state == CONNECTION_STATE_CONNECTED ) {
                throw ActiveMQException(
                    __FILE__, __LINE__,
                    "StompConnector::start - already started" );
            }

            // Start the transport - this establishes the socket.
            transport->start();

            // Send the connect message to the broker.
            connect();
        }
    }
    AMQ_CATCH_RETHROW( ActiveMQException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, ActiveMQException )
    AMQ_CATCHALL_THROW( ActiveMQException );
}

////////////////////////////////////////////////////////////////////////////////
void StompConnector::close() throw( cms::CMSException ){

    try{

        if( state == CONNECTION_STATE_DISCONNECTED ){
            return;
        }

        synchronized( &mutex ) {

            // Send the disconnect message to the broker.
            disconnect();

            // Close the transport now that we've sent the last messages..
            transport->close();
        }
    }
    AMQ_CATCH_RETHROW( ActiveMQException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, ActiveMQException )
    AMQ_CATCHALL_THROW( ActiveMQException );
}

////////////////////////////////////////////////////////////////////////////////
void StompConnector::connect() {

    try{

        // Mark this connector as started.
        state = CONNECTION_STATE_CONNECTING;

        // Send the connect command to the broker
        ConnectCommand cmd;

        // Encode User Name and Password and Client ID
        string login = getUsername();
        if( login.length() > 0 ){
            cmd.setLogin( login );
        }
        string password = getPassword();
        if( password.length() > 0 ){
            cmd.setPassword( password );
        }
        string clientId = getClientId();
        if( clientId.length() > 0 ){
            cmd.setClientId( clientId );
        }

        Response* response = transport->request( &cmd );

        ConnectedCommand* connected =
            dynamic_cast< ConnectedCommand* >( response );

        if( connected == NULL ) {
            delete response;

            throw StompConnectorException(
                __FILE__, __LINE__,
                "StompConnector::connect - "
                "Response not a connected response" );
        }

        // Connected so we now create the SessionManager
        sessionManager = new StompSessionManager(
            connected->getSessionId(), this, transport );

        // Give our message listener to the session manager it will
        // notify all the interested clients
        sessionManager->setConsumerMessageListener( messageListener );

        // Add the Session Manager as the Command Listener for
        // Message commands so that it can route them to the
        // correct consumers.
        addCmdListener( CommandConstants::MESSAGE, sessionManager );

        // In Stomp, the client Id is the same as the session id that is
        // returned in the Connected response
        properties.setProperty(
            commands::CommandConstants::toString(
                commands::CommandConstants::HEADER_CLIENT_ID ),
            connected->getSessionId() );

        // Tag us in the Connected State now.
        state = CONNECTION_STATE_CONNECTED;

        // Clean up
        delete response;
    }
    AMQ_CATCH_RETHROW( ActiveMQException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, ActiveMQException )
    AMQ_CATCHALL_THROW( ActiveMQException )
}

////////////////////////////////////////////////////////////////////////////////
void StompConnector::disconnect() {

    try{

        // Mark state as no longer connected.
        state = CONNECTION_STATE_DISCONNECTED;

        // Send the disconnect command to the broker.
        DisconnectCommand cmd;
        transport->oneway( &cmd );

    } catch( CommandIOException& ex ){
        transport->close();
        throw ex;
    }
    AMQ_CATCH_RETHROW( ActiveMQException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, ActiveMQException )
    AMQ_CATCHALL_THROW( ActiveMQException );
}

////////////////////////////////////////////////////////////////////////////////
SessionInfo* StompConnector::createSession(
    cms::Session::AcknowledgeMode ackMode )
        throw( ConnectorException ) {

    try {
        enforceConnected();
        return sessionManager->createSession( ackMode );
    }
    AMQ_CATCH_RETHROW( ConnectorException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, ConnectorException )
    AMQ_CATCHALL_THROW( ConnectorException );
}

////////////////////////////////////////////////////////////////////////////////
ConsumerInfo* StompConnector::createConsumer(
    const cms::Destination* destination,
    SessionInfo* session,
    const std::string& selector,
    bool noLocal )
        throw ( ConnectorException ) {

    try{

        enforceConnected();

        return sessionManager->createConsumer(
            destination, session, selector, noLocal );
    }
    AMQ_CATCH_RETHROW( ConnectorException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, ConnectorException )
    AMQ_CATCHALL_THROW( ConnectorException );
}

////////////////////////////////////////////////////////////////////////////////
void StompConnector::startConsumer(ConsumerInfo* consumer )
        throw ( ConnectorException ) {

    try {
        enforceConnected();
        return sessionManager->startConsumer(consumer);
    }
    AMQ_CATCH_RETHROW( ConnectorException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, ConnectorException )
    AMQ_CATCHALL_THROW( ConnectorException );
}

////////////////////////////////////////////////////////////////////////////////
ConsumerInfo* StompConnector::createDurableConsumer(
    const cms::Topic* topic,
    SessionInfo* session,
    const std::string& name,
    const std::string& selector,
    bool noLocal )
        throw ( ConnectorException ) {

    try{

        enforceConnected();

        return sessionManager->createDurableConsumer(
            topic, session, name, selector, noLocal );
    }
    AMQ_CATCH_RETHROW( ConnectorException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, ConnectorException )
    AMQ_CATCHALL_THROW( ConnectorException );
}

////////////////////////////////////////////////////////////////////////////////
ProducerInfo* StompConnector::createProducer(
    const cms::Destination* destination,
    SessionInfo* session )
        throw ( ConnectorException ) {

    try{

        enforceConnected();

        ProducerInfo* producer = new StompProducerInfo( this );

        producer->setDestination( destination );
        producer->setProducerId( producerIds.getNextSequenceId() );
        producer->setSessionInfo( session );
        producer->setSendTimeout( 0 );

        return producer;
    }
    AMQ_CATCH_RETHROW( ConnectorException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, ConnectorException )
    AMQ_CATCHALL_THROW( ConnectorException );
}

////////////////////////////////////////////////////////////////////////////////
cms::Topic* StompConnector::createTopic( const std::string& name,
                                         SessionInfo* session AMQCPP_UNUSED)
    throw ( ConnectorException ) {

    try {
        enforceConnected();
        return new StompTopic( name );
    }
    AMQ_CATCH_RETHROW( ConnectorException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, ConnectorException )
    AMQ_CATCHALL_THROW( ConnectorException );
}

////////////////////////////////////////////////////////////////////////////////
cms::Queue* StompConnector::createQueue( const std::string& name,
                                         SessionInfo* session AMQCPP_UNUSED)
    throw ( ConnectorException ) {

    try {
        enforceConnected();
        return new StompQueue( name );
    }
    AMQ_CATCH_RETHROW( ConnectorException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, ConnectorException )
    AMQ_CATCHALL_THROW( ConnectorException );
}

////////////////////////////////////////////////////////////////////////////////
cms::TemporaryTopic* StompConnector::createTemporaryTopic(
    SessionInfo* session AMQCPP_UNUSED)
        throw ( ConnectorException, UnsupportedOperationException ) {

    try {
        throw UnsupportedOperationException(
            __FILE__, __LINE__,
            "StompConnector::createTemporaryTopic - No Stomp Support");
    }
    AMQ_CATCH_RETHROW( ConnectorException )
    AMQ_CATCH_RETHROW( UnsupportedOperationException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, ConnectorException )
    AMQ_CATCHALL_THROW( ConnectorException );
    return NULL;
}

////////////////////////////////////////////////////////////////////////////////
cms::TemporaryQueue* StompConnector::createTemporaryQueue(
    SessionInfo* session AMQCPP_UNUSED)
        throw ( ConnectorException, UnsupportedOperationException ) {

    try {
        throw UnsupportedOperationException(
            __FILE__, __LINE__,
            "StompConnector::createTemporaryQueue - No Stomp Support");
    }
    AMQ_CATCH_RETHROW( ConnectorException )
    AMQ_CATCH_RETHROW( UnsupportedOperationException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, ConnectorException )
    AMQ_CATCHALL_THROW( ConnectorException );
    return NULL;
}

////////////////////////////////////////////////////////////////////////////////
void StompConnector::send( cms::Message* message,
                           ProducerInfo* producerInfo )
    throw ( ConnectorException ) {

    try{

        enforceConnected();

        const SessionInfo* session = producerInfo->getSessionInfo();
        Command* command = dynamic_cast< transport::Command* >( message );

        if( command == NULL ) {
            throw StompConnectorException(
                __FILE__, __LINE__,
                "StompConnector::send - "
                "Message is not a valid stomp type.");
        }

        if( session->getAckMode() == cms::Session::SESSION_TRANSACTED ) {

            StompCommand* stompCommand =
                dynamic_cast< StompCommand* >( message );

            if( stompCommand == NULL ) {
                throw StompConnectorException(
                    __FILE__, __LINE__,
                    "StompConnector::send - "
                    "Message is not a valid stomp type.");
            }

            stompCommand->setTransactionId(
                Long::toString(
                    session->getTransactionInfo()->getTransactionId() ) );
        }

        // Send it
        transport->oneway( command );
    }
    catch( CommandIOException& ex ){
        transport->close();
        throw ConnectorException( __FILE__, __LINE__,
            ex.what() );
    }
    AMQ_CATCH_RETHROW( ConnectorException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, ConnectorException )
    AMQ_CATCHALL_THROW( ConnectorException );
}

////////////////////////////////////////////////////////////////////////////////
void StompConnector::send( std::list<cms::Message*>& messages,
                           ProducerInfo* producerInfo )
    throw ( ConnectorException ) {

    try{

        enforceConnected();

        list< cms::Message* >::const_iterator itr = messages.begin();

        for( ; itr != messages.end(); ++itr ) {
            this->send( *itr, producerInfo );
        }
    }
    AMQ_CATCH_RETHROW( ConnectorException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, ConnectorException )
    AMQ_CATCHALL_THROW( ConnectorException );
}

////////////////////////////////////////////////////////////////////////////////
void StompConnector::acknowledge( const SessionInfo* session,
                                  const ConsumerInfo* consumer AMQCPP_UNUSED,
                                  const cms::Message* message,
                                  AckType ackType AMQCPP_UNUSED)
    throw ( ConnectorException ) {

    try {

        enforceConnected();

        // Auto to Stomp means don't do anything, so we drop it here
        // for client acknowledge we have to send and ack.
        if( session->getAckMode() == cms::Session::CLIENT_ACKNOWLEDGE ) {

            AckCommand cmd;

            if( message->getCMSMessageID() == "" ) {
                throw StompConnectorException(
                    __FILE__, __LINE__,
                    "StompConnector::send - "
                    "Message has no Message Id, cannot ack.");
            }

            cmd.setMessageId( message->getCMSMessageID() );
            transport->oneway( &cmd );
        }
    }
    catch( CommandIOException& ex ){
        transport->close();
        throw ConnectorException( __FILE__, __LINE__,
            ex.what() );
    }
    AMQ_CATCH_RETHROW( ConnectorException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, ConnectorException )
    AMQ_CATCHALL_THROW( ConnectorException );
}

////////////////////////////////////////////////////////////////////////////////
void StompConnector::acknowledge( const SessionInfo* session,
                                  const ConsumerInfo* consumer,
                                  const std::list<const cms::Message*>& messages,
                                  AckType ackType )
    throw ( ConnectorException ) {

    try {

        enforceConnected();

        std::list<const cms::Message*>::const_iterator iter = messages.begin();

        for( ; iter != messages.end(); ++iter ) {
            this->acknowledge( session, consumer, *iter, ackType );
        }
    }
    AMQ_CATCH_RETHROW( ConnectorException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, ConnectorException )
    AMQ_CATCHALL_THROW( ConnectorException );
}

////////////////////////////////////////////////////////////////////////////////
TransactionInfo* StompConnector::startTransaction(
    SessionInfo* session )
        throw ( ConnectorException ) {

    try{

        enforceConnected();

        TransactionInfo* transaction = new StompTransactionInfo( this );

        transaction->setTransactionId( transactionIds.getNextSequenceId() );

        session->setTransactionInfo( transaction );

        BeginCommand cmd;

        cmd.setTransactionId(
                Long::toString( transaction->getTransactionId() ) );

        transport->oneway( &cmd );

        return transaction;
    }
    catch( CommandIOException& ex ){
        transport->close();
        throw ConnectorException( __FILE__, __LINE__,
            ex.what() );
    }
    AMQ_CATCH_RETHROW( ConnectorException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, ConnectorException )
    AMQ_CATCHALL_THROW( ConnectorException );
    return NULL;
}

////////////////////////////////////////////////////////////////////////////////
void StompConnector::commit( TransactionInfo* transaction,
                             SessionInfo* session AMQCPP_UNUSED)
    throw ( ConnectorException ) {

    try {

        enforceConnected();

        CommitCommand cmd;

        cmd.setTransactionId(
                Long::toString( transaction->getTransactionId() ) );

        transport->oneway( &cmd );
    }
    catch( CommandIOException& ex ){
        transport->close();
        throw ConnectorException( __FILE__, __LINE__,
            ex.what() );
    }
    AMQ_CATCH_RETHROW( ConnectorException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, ConnectorException )
    AMQ_CATCHALL_THROW( ConnectorException );
}

////////////////////////////////////////////////////////////////////////////////
void StompConnector::rollback( TransactionInfo* transaction,
                               SessionInfo* session AMQCPP_UNUSED)
    throw ( ConnectorException ) {

    try {

        enforceConnected();

        AbortCommand cmd;

        cmd.setTransactionId(
                Long::toString( transaction->getTransactionId() ) );

        transport->oneway( &cmd );
    }
    catch( CommandIOException& ex ){
        transport->close();
        throw ConnectorException( __FILE__, __LINE__, ex.what() );
    }
    AMQ_CATCH_RETHROW( ConnectorException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, ConnectorException )
    AMQ_CATCHALL_THROW( ConnectorException );
}

////////////////////////////////////////////////////////////////////////////////
cms::Message* StompConnector::createMessage(
    SessionInfo* session AMQCPP_UNUSED,
    TransactionInfo* transaction )
        throw ( ConnectorException ) {

    try{

        enforceConnected();

        MessageCommand* cmd = new MessageCommand();

        if( transaction != NULL ) {
            cmd->setTransactionId(
                Long::toString( transaction->getTransactionId() ) );
        }

        return cmd;
    }
    AMQ_CATCH_RETHROW( ConnectorException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, ConnectorException )
    AMQ_CATCHALL_THROW( ConnectorException );
}

////////////////////////////////////////////////////////////////////////////////
cms::BytesMessage* StompConnector::createBytesMessage(
    SessionInfo* session AMQCPP_UNUSED,
    TransactionInfo* transaction )
        throw ( ConnectorException ) {

    try{

        enforceConnected();

        BytesMessageCommand* cmd = new BytesMessageCommand();

        if( transaction != NULL ) {
            cmd->setTransactionId(
                Long::toString( transaction->getTransactionId() ) );
        }

        return cmd;
    }
    AMQ_CATCH_RETHROW( ConnectorException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, ConnectorException )
    AMQ_CATCHALL_THROW( ConnectorException );
}

////////////////////////////////////////////////////////////////////////////////
cms::TextMessage* StompConnector::createTextMessage(
    SessionInfo* session AMQCPP_UNUSED,
    TransactionInfo* transaction )
        throw ( ConnectorException ) {

    try{

        enforceConnected();

        TextMessageCommand* cmd = new TextMessageCommand;

        if( transaction != NULL ) {
            cmd->setTransactionId(
                Long::toString( transaction->getTransactionId() ) );
        }

        return cmd;
    }
    AMQ_CATCH_RETHROW( ConnectorException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, ConnectorException )
    AMQ_CATCHALL_THROW( ConnectorException );
}

////////////////////////////////////////////////////////////////////////////////
cms::MapMessage* StompConnector::createMapMessage(
    SessionInfo* session AMQCPP_UNUSED,
    TransactionInfo* transaction AMQCPP_UNUSED )
        throw ( ConnectorException, UnsupportedOperationException ) {

    try{
        throw UnsupportedOperationException(
            __FILE__, __LINE__,
            "StompConnector::createMapMessage - No Stomp Support");
    }
    AMQ_CATCH_RETHROW( ConnectorException )
    AMQ_CATCH_RETHROW( UnsupportedOperationException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, ConnectorException )
    AMQ_CATCHALL_THROW( ConnectorException );
    return NULL;
}

////////////////////////////////////////////////////////////////////////////////
void StompConnector::unsubscribe( const std::string& name AMQCPP_UNUSED )
    throw ( ConnectorException, UnsupportedOperationException ) {

    try {
        throw UnsupportedOperationException(
            __FILE__, __LINE__,
            "StompConnector::unsubscribe - No Stomp Support");
    }
    AMQ_CATCH_RETHROW( ConnectorException )
    AMQ_CATCH_RETHROW( UnsupportedOperationException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, ConnectorException )
    AMQ_CATCHALL_THROW( ConnectorException );
}

////////////////////////////////////////////////////////////////////////////////
void StompConnector::pullMessage( const connector::ConsumerInfo* info AMQCPP_UNUSED, long long timeout AMQCPP_UNUSED )
    throw ( ConnectorException, decaf::lang::exceptions::UnsupportedOperationException ) {

    try {
        throw UnsupportedOperationException(
            __FILE__, __LINE__,
            "StompConnector::pullMessage - No Stomp Support for Message Pull");
    }
    AMQ_CATCH_RETHROW( ConnectorException )
    AMQ_CATCH_RETHROW( UnsupportedOperationException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, ConnectorException )
    AMQ_CATCHALL_THROW( ConnectorException );
}

////////////////////////////////////////////////////////////////////////////////
void StompConnector::destroyDestination( const cms::Destination* destination )
    throw ( ConnectorException, decaf::lang::exceptions::UnsupportedOperationException ) {

    try {
        throw UnsupportedOperationException(
            __FILE__, __LINE__,
            "StompConnector::destroyDestination - No Stomp Support for Destroying Destinations");
    }
    AMQ_CATCH_RETHROW( ConnectorException )
    AMQ_CATCH_RETHROW( UnsupportedOperationException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, ConnectorException )
    AMQ_CATCHALL_THROW( ConnectorException );
}

////////////////////////////////////////////////////////////////////////////////
void StompConnector::closeResource( ConnectorResource* resource )
    throw ( ConnectorException ) {

    try{

        ConsumerInfo* consumer =
            dynamic_cast<ConsumerInfo*>(resource);
        SessionInfo* session =
            dynamic_cast<SessionInfo*>(resource);

        if( consumer != NULL) {

            try{
                sessionManager->removeConsumer( consumer );
            } catch( ConnectorException& ex ){

                // Make sure we delete the resource
                delete resource;

                ex.setMark( __FILE__, __LINE__ );
                throw ex;
            }

        } else if( session != NULL ) {

            try{
                sessionManager->removeSession( session );
            } catch( ConnectorException& ex ){

                // Make sure we delete the resource
                delete resource;

                ex.setMark( __FILE__, __LINE__ );
                throw ex;
            }
        }
    }
    AMQ_CATCH_RETHROW( ConnectorException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, ConnectorException )
    AMQ_CATCHALL_THROW( ConnectorException );
}

////////////////////////////////////////////////////////////////////////////////
void StompConnector::onCommand( transport::Command* command ) {

    try{

        StompCommand* stompCommand = dynamic_cast< StompCommand* >(command);

        if( stompCommand == NULL ) {
            fire( ConnectorException(
                __FILE__, __LINE__,
                "StompConnector::onCommand - Recieved an unknown Command") );
        }

        CmdListenerMap::iterator itr =
            cmdListenerMap.find( stompCommand->getStompCommandId() );

        if( itr == cmdListenerMap.end() ) {
            fire( ConnectorException(
                __FILE__, __LINE__,
                "StompConnector::onCommand - "
                "Recieved command with no listener") );

            // This isn't going an farther, so delete it.
            delete command;

            return;   // we are done
        }

        // Hand off
        itr->second->onStompCommand( stompCommand );
    }
    AMQ_CATCH_RETHROW( ConnectorException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, ConnectorException )
    AMQ_CATCHALL_THROW( ConnectorException );
}

////////////////////////////////////////////////////////////////////////////////
void StompConnector::onTransportException(
    transport::Transport* source AMQCPP_UNUSED,
    const decaf::lang::Exception& ex ) {

    try{

        // We're disconnected - the asynchronous error is expected.
        if( state == CONNECTION_STATE_DISCONNECTED ){
            return;
        }

        // We were not closing - log the stack trace.
        LOGDECAF_WARN( logger, ex.getStackTraceString() );

        // Inform the user of the error.
        ActiveMQException convert( ex );
        fire( convert );
    }
    AMQ_CATCH_RETHROW( ConnectorException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, ConnectorException )
    AMQ_CATCHALL_THROW( ConnectorException );
}

////////////////////////////////////////////////////////////////////////////////
void StompConnector::onStompCommand( commands::StompCommand* command )
    throw ( StompConnectorException ) {

    try{

        ErrorCommand* error =
            dynamic_cast<ErrorCommand*>( command );

        if( error != NULL ) {
            fire( StompConnectorException(
                  __FILE__, __LINE__,
                  ( string( "StompConnector::onStompCommand - " ) +
                            error->getErrorMessage() ).c_str() ) );

            // Shutdown
            close();
        }

        // command is done here, delete it.
        delete command;
    }
    AMQ_CATCH_RETHROW( StompConnectorException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, StompConnectorException )
    AMQ_CATCHALL_THROW( StompConnectorException );
}
