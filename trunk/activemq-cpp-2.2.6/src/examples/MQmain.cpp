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

// START SNIPPET: demo

#include <activemq/library/ActiveMQCPP.h>
#include <decaf/lang/Thread.h>
#include <decaf/lang/Runnable.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <decaf/lang/Integer.h>
#include <decaf/lang/Long.h>
#include <decaf/lang/System.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/util/Config.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>
#include <cms/BytesMessage.h>
#include <cms/MapMessage.h>
#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <memory>

#include <sstream>
#include <iomanip>
#include "XQueue.h"
#include "document.h"
#include "prettywriter.h"
#include "stringbuffer.h"
#include "filereadstream.h"

#include "MemShareAlive.h"

using namespace rapidjson;
using namespace activemq::core;
using namespace decaf::util::concurrent;
using namespace decaf::util;
using namespace decaf::lang;
using namespace cms;
using namespace std;

#define CODE_LEN  16
#define NAME_LEN  32
typedef struct tagCodeInfo
{
	unsigned int m_usMarketType;			//�г�����
	unsigned int m_uiUnit;					//ÿ�ֵ�λ
	char           m_acCode[CODE_LEN];		//��Լ����
	char           m_acName[NAME_LEN];		//��Լ����
}CODE_INFO, *PCODE_INFO;

typedef struct	tagBidAskUnit
{
	tagBidAskUnit()
	{
		m_uiVol = 0;
		m_uiPrice = 0;
	}
	unsigned int m_uiVol;		//ί��
	unsigned int m_uiPrice;	    //ί��
} BID_ASK_UNIT, *PBID_ASK_UNIT;

typedef struct tagQuotation
{
	CODE_INFO m_CodeInfo;		//��Լ��Ϣ[8]

	unsigned int m_uiSeqNo;     //����
	unsigned int m_uiDate;
	unsigned int m_uiTime;
	unsigned int m_uilastClose; //����
	unsigned int m_uiLastSettle;//���
	unsigned int m_uiSettle;    //����
	unsigned int m_uiOpenPrice; //����
	unsigned int m_uiHigh;      //��߼�
	unsigned int m_uiLow;       //��ͼ�
	unsigned int m_uiClose;     //���̼�
	unsigned int m_uiHighLimit;
	unsigned int m_uiLowLimit;
	unsigned int m_uiLast;      //���¼�
	unsigned int m_uiAverage;   //����
	unsigned int m_uiVolume;    //�ɽ���
	unsigned int m_uiWeight;    //�ɽ�����
	unsigned int m_uiTurnOver;  //�ɽ����
	unsigned int m_uiChiCangLiang;//�ֲ���
	unsigned int m_uiLastChiCangLiang;//���ճֲ���
	BID_ASK_UNIT     m_Bid[5];
	BID_ASK_UNIT     m_Ask[5];

	string Key() const
	{
		string sKey;
		sKey.assign((const char*)(&m_CodeInfo.m_usMarketType), sizeof(m_CodeInfo.m_usMarketType));
		sKey.append(m_CodeInfo.m_acCode, sizeof(m_CodeInfo.m_acCode));
		return sKey;
	}
	void Decode(const char* pBuf, unsigned int uiSize)
	{
		*this = *(QUOTATION*)(pBuf);
	}
	const char* Encode(unsigned int & uiLen)
	{
		uiLen = sizeof(QUOTATION);
		return (const char*)(this);
	}
} QUOTATION, *PQUOTATION;

template <typename T>
std::string ToString(const T tFrom, unsigned short usPrecision = 2, unsigned short usWidth = 0xFFFF, char cPad = ' ', char cAlign = 'L')
{
	std::stringstream ss;
	ss << fixed << setprecision(usPrecision);

	switch (cAlign)
	{
	case 'L':
		ss << left;
		break;
	case 'l':
		ss << left;
		break;
	case 'R':
		ss << right;
		break;
	case 'r':
		ss << right;
		break;
	default:
		break;
	}

	if (usWidth != 0xFFFF)
	{
		ss << setw(usWidth) << setfill(cPad);
	}

	ss << tFrom;
	return ss.str();
}


//���Ҳ���nCount��c, ȡ���Ӵ�
string LeftSubRight(const string& sSrc, char c, size_t nCount = 1)
{
	if (0 == nCount)
		return sSrc;

	string::size_type iPos = string::npos;
	string::size_type iPosLast = string::npos;
	do
	{
		iPos = sSrc.rfind(c, iPosLast);
		if (iPos == string::npos)
		{
			if (iPosLast == string::npos)
			{
				return sSrc;
			}
			else
			{
				return sSrc.substr(0, iPosLast);
			}
		}
		else
		{
			nCount--;
			if (nCount == 0)
			{
				if (iPos != string::npos)
				{
					return sSrc.substr(0, iPos);
				}
				else
				{
					return sSrc.substr(0, iPosLast);
				}
			}
			iPosLast = iPos - 1;
		}
	} while (1);
}

class HelloWorldProducer : public Runnable {
private:

    Connection* connection;
    Session* session;
    Destination* destination;
    MessageProducer* producer;
    int numMessages;
    bool useTopic;
    bool sessionTransacted;
    std::string brokerURI;
	CMemShareAlive			m_oMemShareAlive;
private:

    HelloWorldProducer(const HelloWorldProducer&);
    HelloWorldProducer& operator=(const HelloWorldProducer&);

public:

    HelloWorldProducer(const std::string& brokerURI, int numMessages, bool useTopic = false, bool sessionTransacted = false) :
        connection(NULL),
        session(NULL),
        destination(NULL),
        producer(NULL),
        numMessages(numMessages),
        useTopic(useTopic),
        sessionTransacted(sessionTransacted),
        brokerURI(brokerURI) {
    }

    virtual ~HelloWorldProducer(){
        cleanup();
    }

    void close() {
        this->cleanup();
    }

    virtual void run() {

		brokerURI = "tcp://10.0.2.107:61616";
		string sXQueName = "QUEUE_WJF_QUOTATION";
		string mq_user = "jlt_mq";
		string mq_pwd = "jlt_mq123";
		string mq_topic = "QUOTATION.WJF";
		unsigned int uiNodeID = 6601;
		
		char szFileName[_MAX_PATH], szFilePath[_MAX_PATH];
		char * pcName;
		::GetModuleFileName(0, szFileName, _MAX_PATH);
		::GetFullPathName(szFileName, _MAX_PATH, szFilePath, &pcName);
		char szBuf[_MAX_PATH];
		strcpy(szBuf, pcName);
		*pcName = '\0';
		SetCurrentDirectory(szFilePath);

		bool blTsCheck = false;
		double dlForexInstMulti = 10000.0, dlOtherInstMulti = 100.0;
		String filename = ".\\mq.json";
		FILE * pFile = fopen(filename.c_str(), "r");
		if (pFile) {
			char buff[1024] = { 0 };
			rapidjson::Document jsonCfg;
			rapidjson::FileReadStream inputStream(pFile, buff, sizeof(buff));
			jsonCfg.ParseStream<0>(inputStream);
			fclose(pFile);

			if (jsonCfg.HasMember("mem_que_name")) {
				sXQueName = jsonCfg["mem_que_name"].GetString();
			}

			if (jsonCfg.HasMember("mq_url")) {
				brokerURI = jsonCfg["mq_url"].GetString();
			}

			if (jsonCfg.HasMember("mq_user")) {
				mq_user = jsonCfg["mq_user"].GetString();
			}

			if (jsonCfg.HasMember("mq_pwd")) {
				mq_pwd = jsonCfg["mq_pwd"].GetString();
			}

			if (jsonCfg.HasMember("mq_topic")) {
				mq_topic = jsonCfg["mq_topic"].GetString();
			}

			if (jsonCfg.HasMember("node_id")) {
				uiNodeID = jsonCfg["node_id"].GetInt();
			}

			if (jsonCfg.HasMember("forex_inst_multi")) {
				dlForexInstMulti = (double)(jsonCfg["forex_inst_multi"].GetInt());
			}

			if (jsonCfg.HasMember("other_inst_multi")) {
				dlOtherInstMulti = (double)(jsonCfg["other_inst_multi"].GetInt());
			}

			if (jsonCfg.HasMember("tm_check")) {
				if (1 == jsonCfg["tm_check"].GetInt())
				{
					blTsCheck = true;
				}
			}
		}

		//
		char szProcessName[_MAX_PATH];
		::GetModuleFileName(0, szProcessName, _MAX_PATH);
		string sTmp = szProcessName;
		sTmp = LeftSubRight(sTmp, '.');
		m_oMemShareAlive.Bind(E_PROCESS_APP);
		if (FALSE == m_oMemShareAlive.Create(sTmp.c_str()))
		{
			cout << "m_oMemShareAlive.Create fail" << endl;
		}
		else
		{
			cout << sTmp << endl;
			unsigned int uiProcessID = ::GetCurrentProcessId();
			m_oMemShareAlive.IamAlive(uiProcessID);
			//m_oMemShareAlive.SetNodeID(uiNodeID);
		}


		CXQueue xQueue;
		do
		{
			int nRet = xQueue.Open(sXQueName.c_str());
			if (!xQueue.IsOpen())
			{
				cout << "�򿪹����ڴ���Ϣ����[" << sXQueName << "]ʧ��!10�������!" << endl;
				m_oMemShareAlive.IamAlive();
				//cout << "iamalive" << endl;
				Sleep(10000);
			}
			else
			{
				break;
			}
		} while (1);

		unsigned int uiBlockSize = 1024;
		unsigned int uiXQueLen = 1024;
		unsigned int uiBufSize = uiBlockSize*uiXQueLen;
		char* pBuf = new char[uiBufSize];
		memset(pBuf, 0x00, uiBufSize);

		do
		{
			try {
				m_oMemShareAlive.IamAlive();
				//cout << "iamalive" << endl;

				// Create a ConnectionFactory
				auto_ptr<ConnectionFactory> connectionFactory(
					ConnectionFactory::createCMSConnectionFactory(brokerURI));

				// Create a Connection
				connection = connectionFactory->createConnection(mq_user, mq_pwd);
				connection->start();

				// Create a Session
				if (this->sessionTransacted) {
					session = connection->createSession(Session::SESSION_TRANSACTED);
				}
				else {
					session = connection->createSession(Session::AUTO_ACKNOWLEDGE);
				}

				// Create the destination (Topic or Queue)
				if (useTopic) {
					destination = session->createTopic(mq_topic);
				}
				else {
					destination = session->createQueue(mq_topic);
				}

				// Create a MessageProducer from the Session to the Topic or Queue
				producer = session->createProducer(destination);
				producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);
				size_t nMessageId = 0;
				unsigned int uiTimeout = 2000;
				do
				{
					m_oMemShareAlive.IamAlive();
					//cout << "iamalive" << endl;
					long lRet = xQueue.Read((LPBYTE)pBuf, &uiBufSize, &nMessageId, NULL, uiTimeout);
					if (XQUEUE_ERROR_SUCCESS != lRet)
					{
						//cout << "time out" << endl;
						continue;
					}

					QUOTATION quo = { 0 };
					quo.Decode(pBuf, uiBufSize);

					string instCode = quo.m_CodeInfo.m_acCode;
					double dlMulti = 100.00;
					unsigned short usPrecision = 2;
					if (instCode == "AUTD" || instCode == "AU9999")
					{
						dlMulti = dlOtherInstMulti;
						usPrecision = 2;
					}
					else if (instCode == "5120")
					{
						dlMulti = dlOtherInstMulti;
						usPrecision = 2;
					}
					else if (instCode == "USDCNH")
					{
						dlMulti = dlForexInstMulti;
						usPrecision = 4;
					}
					else
					{
						continue;
					}

					if (blTsCheck)
					{
						SYSTEMTIME st;
						::GetLocalTime(&st);
						unsigned int uiQuoTime = quo.m_uiTime / 1000;
						unsigned int uiHour = uiQuoTime / 10000;
						unsigned int uiMin = (uiQuoTime % 10000) / 100;
						unsigned int uiSec = uiQuoTime % 100;
						int nDelay = st.wHour * 3600 + st.wMinute * 60 + st.wSecond - (int)uiHour * 3600 - (int)uiMin * 60 - (int)uiSec;
						if (nDelay >= 60 || nDelay <= -60)
						{
							cout << quo.m_CodeInfo.m_acCode << " " << quo.m_uiDate << " " << quo.m_uiTime / 1000 << " " << quo.m_uiLast / dlMulti << " " << quo.m_uilastClose / dlMulti << " " << quo.m_uiLastSettle / dlMulti << " timeouto" << endl;
							//cout << uiQuoTime << " " << uiHour << " " << uiMin << " " << uiSec << " " << st.wHour << " " << st.wMinute << " " << st.wSecond << " " << nDelay << endl;
							continue;
						}
						else if (st.wYear * 10000 + st.wMonth * 100 + st.wDay != quo.m_uiDate
							&& st.wHour * 10000 + st.wMinute * 100 + st.wSecond > 23100
							&& st.wHour * 10000 + st.wMinute * 100 + st.wSecond < 200000)
						{
							cout << quo.m_CodeInfo.m_acCode << " " << quo.m_uiDate << " " << quo.m_uiTime / 1000 << " " << quo.m_uiLast / dlMulti << " " << quo.m_uilastClose / dlMulti << " " << quo.m_uiLastSettle / dlMulti << " timeouti" << endl;
							continue;
						}
					}

					rapidjson::StringBuffer strBuffer;
					rapidjson::Writer<rapidjson::StringBuffer> writer(strBuffer);
					writer.StartObject();
					writer.String("market_code");
					writer.Uint(quo.m_CodeInfo.m_usMarketType);
					writer.String("prod_code");
					writer.String(quo.m_CodeInfo.m_acCode);
					writer.String("quoteDate");
					writer.Uint(quo.m_uiDate);
					writer.String("quoteTime");
					writer.Uint(quo.m_uiTime / 1000);
					writer.String("sequenceNo");
					writer.Uint(quo.m_uiSeqNo);
					writer.String("buyPrice");
					writer.String(ToString<double>((double)quo.m_Bid[0].m_uiPrice / dlMulti, usPrecision).c_str());
					writer.String("sellPrice");
					writer.String(ToString<double>((double)quo.m_Ask[0].m_uiPrice / dlMulti, usPrecision).c_str());
					writer.String("midPrice");
					writer.String(ToString<double>((((double)quo.m_Bid[0].m_uiPrice + (double)quo.m_Ask[0].m_uiPrice) / 2.0) / dlMulti, usPrecision).c_str());
					writer.String("lastPrice");
					writer.String(ToString<double>((double)quo.m_uiLast / dlMulti, usPrecision).c_str());
					writer.String("settlePrice");
					writer.String(ToString<double>((double)quo.m_uiSettle / dlMulti, usPrecision).c_str());
					writer.String("avgPrice");
					writer.String(ToString<double>((double)quo.m_uiAverage / dlMulti, usPrecision).c_str());
					writer.String("closePrice");
					writer.String(ToString<double>((double)quo.m_uiClose / dlMulti, usPrecision).c_str());
					writer.String("lastClosePrice");
					writer.String(ToString<double>((double)quo.m_uilastClose / dlMulti, usPrecision).c_str());
					writer.String("lastSettlePrice");
					writer.String(ToString<double>((double)quo.m_uiLastSettle / dlMulti, usPrecision).c_str());
					writer.EndObject();
					string sMqTopic = strBuffer.GetString();
					std::auto_ptr<TextMessage> message(session->createTextMessage(sMqTopic));
					cout << sMqTopic << endl;
					producer->send(message.get());
				} while (true);
				delete[]pBuf;
			}
			catch (CMSException& e) {
				cout << "mq execption " << e.getMessage() + " 10������ԣ�" << endl;
				//e.printStackTrace();
				Sleep(3000);
			}
		}while (1);
    }

private:

    void cleanup() {

        if (connection != NULL) {
            try {
                connection->close();
            } catch (cms::CMSException& ex) {
                ex.printStackTrace();
            }
        }

        // Destroy resources.
        try {
            delete destination;
            destination = NULL;
            delete producer;
            producer = NULL;
            delete session;
            session = NULL;
            delete connection;
            connection = NULL;
        } catch (CMSException& e) {
            e.printStackTrace();
        }
    }
};

class HelloWorldConsumer : public ExceptionListener,
                           public MessageListener,
                           public Runnable {

private:

    CountDownLatch latch;
    CountDownLatch doneLatch;
    Connection* connection;
    Session* session;
    Destination* destination;
    MessageConsumer* consumer;
    long waitMillis;
    bool useTopic;
    bool sessionTransacted;
    std::string brokerURI;

private:

    HelloWorldConsumer(const HelloWorldConsumer&);
    HelloWorldConsumer& operator=(const HelloWorldConsumer&);

public:

    HelloWorldConsumer(const std::string& brokerURI, int numMessages, bool useTopic = false, bool sessionTransacted = false, int waitMillis = 30000) :
        latch(1),
        doneLatch(numMessages),
        connection(NULL),
        session(NULL),
        destination(NULL),
        consumer(NULL),
        waitMillis(waitMillis),
        useTopic(useTopic),
        sessionTransacted(sessionTransacted),
        brokerURI(brokerURI) {
    }

    virtual ~HelloWorldConsumer() {
        cleanup();
    }

    void close() {
        this->cleanup();
    }

    void waitUntilReady() {
        latch.await();
    }

    virtual void run() {

        try {

            // Create a ConnectionFactory
            auto_ptr<ConnectionFactory> connectionFactory(
                ConnectionFactory::createCMSConnectionFactory(brokerURI));

            // Create a Connection
            connection = connectionFactory->createConnection();
            connection->start();
            connection->setExceptionListener(this);

            // Create a Session
            if (this->sessionTransacted == true) {
                session = connection->createSession(Session::SESSION_TRANSACTED);
            } else {
                session = connection->createSession(Session::AUTO_ACKNOWLEDGE);
            }

            // Create the destination (Topic or Queue)
            if (useTopic) {
                destination = session->createTopic("TEST.FOO");
            } else {
                destination = session->createQueue("TEST.FOO");
            }

            // Create a MessageConsumer from the Session to the Topic or Queue
            consumer = session->createConsumer(destination);

            consumer->setMessageListener(this);

            std::cout.flush();
            std::cerr.flush();

            // Indicate we are ready for messages.
            latch.countDown();

            // Wait while asynchronous messages come in.
            doneLatch.await(waitMillis);

        } catch (CMSException& e) {
            // Indicate we are ready for messages.
            latch.countDown();
            e.printStackTrace();
        }
    }

    // Called from the consumer since this class is a registered MessageListener.
    virtual void onMessage(const Message* message) {

        static int count = 0;

        try {
            count++;
            const TextMessage* textMessage = dynamic_cast<const TextMessage*> (message);
            string text = "";

            if (textMessage != NULL) {
                text = textMessage->getText();
            } else {
                text = "NOT A TEXTMESSAGE!";
            }

            printf("Message #%d Received: %s\n", count, text.c_str());

        } catch (CMSException& e) {
            e.printStackTrace();
        }

        // Commit all messages.
        if (this->sessionTransacted) {
            session->commit();
        }

        // No matter what, tag the count down latch until done.
        doneLatch.countDown();
    }

    // If something bad happens you see it here as this class is also been
    // registered as an ExceptionListener with the connection.
    virtual void onException(const CMSException& ex AMQCPP_UNUSED) {
        printf("CMS Exception occurred.  Shutting down client.\n");
        ex.printStackTrace();
        exit(1);
    }

private:

    void cleanup() {
        if (connection != NULL) {
            try {
                connection->close();
            } catch (cms::CMSException& ex) {
                ex.printStackTrace();
            }
        }

        // Destroy resources.
        try {
            delete destination;
            destination = NULL;
            delete consumer;
            consumer = NULL;
            delete session;
            session = NULL;
            delete connection;
            connection = NULL;
        } catch (CMSException& e) {
            e.printStackTrace();
        }
    }
};

int main(int argc AMQCPP_UNUSED, char* argv[] AMQCPP_UNUSED) {

    activemq::library::ActiveMQCPP::initializeLibrary();
    {
    std::cout << "=====================================================\n";
    std::cout << "Starting the example:" << std::endl;
    std::cout << "-----------------------------------------------------\n";


    // Set the URI to point to the IP Address of your broker.
    // add any optional params to the url to enable things like
    // tightMarshalling or tcp logging etc.  See the CMS web site for
    // a full list of configuration options.
    //
    //  http://activemq.apache.org/cms/
    //
    // Wire Format Options:
    // =========================
    // Use either stomp or openwire, the default ports are different for each
    //
    // Examples:
    //    tcp://127.0.0.1:61616                      default to openwire
    //    tcp://127.0.0.1:61616?wireFormat=openwire  same as above
    //    tcp://127.0.0.1:61613?wireFormat=stomp     use stomp instead
    //
    // SSL:
    // =========================
    // To use SSL you need to specify the location of the trusted Root CA or the
    // certificate for the broker you want to connect to.  Using the Root CA allows
    // you to use failover with multiple servers all using certificates signed by
    // the trusted root.  If using client authentication you also need to specify
    // the location of the client Certificate.
    //
    //     System::setProperty( "decaf.net.ssl.keyStore", "<path>/client.pem" );
    //     System::setProperty( "decaf.net.ssl.keyStorePassword", "password" );
    //     System::setProperty( "decaf.net.ssl.trustStore", "<path>/rootCA.pem" );
    //
    // The you just specify the ssl transport in the URI, for example:
    //
    //     ssl://localhost:61617
    //
	std::string brokerURI = "tcp://10.0.2.107:61616";
//        "?wireFormat=openwire"
//        "&transport.useInactivityMonitor=false"
//        "&connection.alwaysSyncSend=true"
//        "&connection.useAsyncSend=true"
//        "?transport.commandTracingEnabled=true"
//        "&transport.tcpTracingEnabled=true"
//        "&wireFormat.tightEncodingEnabled=true"

    //============================================================
    // set to true to use topics instead of queues
    // Note in the code above that this causes createTopic or
    // createQueue to be used in both consumer an producer.
    //============================================================
    bool useTopics = true;
    bool sessionTransacted = false;
    int numMessages = 2000;

    long long startTime = System::currentTimeMillis();

    HelloWorldProducer producer(brokerURI, numMessages, useTopics);
        //HelloWorldConsumer consumer(brokerURI, numMessages, useTopics, sessionTransacted);

    // Start the consumer thread.
    //Thread consumerThread(&consumer);
    //consumerThread.start();

    // Wait for the consumer to indicate that its ready to go.
    //consumer.waitUntilReady();

    // Start the producer thread.
    Thread producerThread(&producer);
    producerThread.start();

    // Wait for the threads to complete.
    producerThread.join();
    //consumerThread.join();

    long long endTime = System::currentTimeMillis();
    double totalTime = (double)(endTime - startTime) / 1000.0;

    //consumer.close();
    producer.close();

    std::cout << "Time to completion = " << totalTime << " seconds." << std::endl;
    std::cout << "-----------------------------------------------------\n";
    std::cout << "Finished with the example." << std::endl;
    std::cout << "=====================================================\n";

    }
    activemq::library::ActiveMQCPP::shutdownLibrary();
}

// END SNIPPET: demo
