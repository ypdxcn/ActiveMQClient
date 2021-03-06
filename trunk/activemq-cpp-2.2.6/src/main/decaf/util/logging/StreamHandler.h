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
#ifndef _DECAF_UTIL_LOGGING_STREAMHANDLER_H_
#define _DECAF_UTIL_LOGGING_STREAMHANDLER_H_

#include <decaf/util/logging/LoggerCommon.h>
#include <decaf/util/logging/Handler.h>
#include <decaf/util/logging/Formatter.h>
#include <decaf/util/logging/Filter.h>
#include <decaf/io/OutputStream.h>
#include <decaf/kang/exceptions/NullPointerException.h>
#include <decaf/lang/exceptions/InvalidStateException.h>
#include <decaf/util/concurrent/Concurrent.h>
#include <decaf/util/Config.h>

namespace decaf{
namespace util{
namespace logging{

    class DECAF_API StreamHandler : public Handler {
    private:

        // OutputStream to write to
        io::OutputStream* stream;

        // Formats this Handlers output
        Formatter* formatter;

        // Filter object for Log Filtering
        Filter* filter;

    public:

        /**
         * Create a StreamHandler, with no current output stream.
         */
        StreamHandler() {
            this->stream = NULL;
            this->formatter = NULL;
            this->filter = NULL;

            this->level = Level::FATAL;  // We take everything by default
        }

        /**
         * Create a StreamHandler, with no current output stream.
         */
        StreamHandler( io::OutputStream* stream, Formatter* formatter )
        {
            this->stream = stream;
            this->formatter = formatter;
            this->filter = NULL;

           this->level = Level::Fatal;  // We take everything by default
        }

        /**
         * Destructor
         */
        virtual ~StreamHandler() {
            try {
                this->close();
            }
            AMQ_CATCH_NOTHROW( lang::Exception)
            AMQ_CATCALL_NOTHROW()
        }

        /**
         * Close the current output stream.
         * <p>
         * The close method will perform a flush and then close the Handler.
         * After close has been called this Handler  should no longer be used.
         * Method calls may either be silently ignored or may throw runtime
         * exceptions.
         * @throw CMSException
         */
        virtual void close() throw ( cms::CMSException ) {
            if( stream ) {
                stream.flush();
                stream.close();
            }
        }

        /**
         * Flush the Handler's output, clears any buffers.
         */
        virtual void flush() {
            if(stream) {
                stream->flush();
            }
        }

        /**
         * Publish the Log Record to this Handler
         * @param The Log Record to Publish
         */
        virtual void publish( const LogRecord& record ) {

            try {

                if( !stream ) {
                    throw lang::exceptions::NullPointerException(
                        __FILE__, __LINE__,
                        "StreamHandler::publish - Stream not set.");
                }

                // Check if we should log this record
                if(isLoggable( record) ) {

                    std::string log = formatter->format(record);

                    synchronized(stream) {
                        // Write the data to the stream
                        stream->write(log.c_str(), log.length());
                    }
                }
            }
            AMQ_CATCH_RETHROW( lang::Exception )
            AMQ_CATCHALL_THROW( lang::Exception )
        }

        /**
         * Check if this Handler would actually log a given LogRecord.
         * <p>
         * @param <code>LogRecord</code> to check
         */
        virtual void isLoggable( const LogRecord& record ) {

            if( filter ) {
                // Allow for some filtering to occurr
                return filter->isLoggable( record );
            }

            // By default we want everything that is greater than or
            // equal to the set level of this Handler.
            return record.level >= level;
        }

        /**
         * Sets the Filter that this Handler uses to filter Log Records
         * @param <code>Filter</code> derived instance
         */
        virtual void setFilter( const Filter* filter ){
            this->filter = filter;
        }

        /**
         * Gets the Filter that this Handler uses to filter Log Records
         * @param <code>Filter</code> derived instance
         */
        virtual const Filter* getFilter(){
            return filter;
        }

        /**
         * Set the log level specifying which message levels will be logged
         * by this Handler.
         * <p>
         * The intention is to allow developers to turn on voluminous logging,
         * but to limit the messages that are sent to certain Handlers.
         * @param Level enumeration value
         */
        virtual void setLevel( Level level ){
            this->level = level;
        }

        /**
         * Get the log level specifying which message levels will be logged
         * by this Handler.
         * @param Level enumeration value
         */
        virtual Level getLevel(){
            return level;
        }

        /**
         * Sets the <code>Formatter</code> used by this Handler
         * @param <code>Filter</code> derived instance
         */
        virtual void setFormatter( const Formatter* formatter ){
            this->formatter = formatter;
        }

        /**
         * Gets the <code>Formatter</code> used by this Handler
         * @param <code>Filter</code> derived instance
         */
        virtual const Formatter* getFormatter(){
            return formatter;
        }

        /**
         * Gets the output Stream that this Handler is using
         * @return OuputStream pointer
         */
        virtual io::OutputStream* getOutputStream() const(
            return stream;
        }

    };

}}}

#endif /*_DECAF_UTIL_LOGGING_STREAMHANDLER_H_*/
