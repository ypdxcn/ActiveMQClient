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

#include <decaf/lang/Thread.h>
#include <decaf/lang/Runnable.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <decaf/lang/Integer.h>
#include <decaf/util/Date.h>
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
#include <iostream>
#include <memory>


#include <sstream>
#include <iomanip>

#include "document.h"
#include "prettywriter.h"
#include "stringbuffer.h"
#include "filereadstream.h"

#include "XQueue.h"
#include "MemShareAlive.h"
#include "samplerpacket.h"
#include "strutils.h"
#include "logger.h"
#include "ConfigImpl.h"

#include "XQueueIO.h"

using namespace activemq::core;
using namespace decaf::util::concurrent;
using namespace decaf::util;
using namespace decaf::lang;
using namespace cms;
using namespace std;

//custom
using namespace rapidjson;


//设置颜色：
/*
对应的颜色码表：
0 = 黑色       8 = 灰色
1 = 蓝色       9 = 淡蓝色
2 = 绿色       10 = 淡绿色
3 = 浅绿色     11 = 淡浅绿色
4 = 红色       12 = 淡红色
5 = 紫色       13 = 淡紫色
6 = 黄色       14 = 淡黄色
7 = 白色       15 = 亮白色 
*/
void setColor(unsigned short ForeColor=7,unsigned short BackGroundColor=0)
{
    HANDLE handle=GetStdHandle(STD_OUTPUT_HANDLE);//获取当前窗口句柄
    SetConsoleTextAttribute(handle,ForeColor+BackGroundColor*0x10);//设置颜色
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

	//WatchDog
	CMemShareAlive			m_oMemShareAlive;
	CConfigImpl*		    m_pConfig;
public:

    HelloWorldProducer( const std::string& brokerURI,
                        int numMessages,
                        bool useTopic = false,
                        bool sessionTransacted = false ){
        this->connection = NULL;
        this->session = NULL;
        this->destination = NULL;
        this->producer = NULL;
        this->numMessages = numMessages;
        this->useTopic = useTopic;
        this->sessionTransacted = sessionTransacted;
        this->brokerURI = brokerURI;
    }

    virtual ~HelloWorldProducer(){
        cleanup();
    }

    virtual void run() {
		//默认参数
		brokerURI = "tcp://10.1.6.2:61616";
		string sXQueName = "QUEUE_WJF_QUOTATION";
		string mq_user = "jlt_mq";
		string mq_pwd = "jlt_mq123";
		string mq_topic = "QUOTATION.WJF";
		unsigned int uiNodeID = 6601;
		double dlForexInstMulti = 10000.0, dlOtherInstMulti = 100.0;
		bool blTsCheck = false;

		//获取路径
		char szFileName[_MAX_PATH], szFilePath[_MAX_PATH];
		char * pcName;
		::GetModuleFileName(0, szFileName, _MAX_PATH);
		::GetFullPathName(szFileName, _MAX_PATH, szFilePath, &pcName);
		char szBuf[_MAX_PATH];
		strcpy(szBuf, pcName);
		*pcName = '\0';
		SetCurrentDirectory(szFilePath);

		//组配置文件名
		std::string sCfgFilename;
		sCfgFilename = szFilePath;
		sCfgFilename+= DEFUALT_CONF_PATH PATH_SLASH;
		sCfgFilename = sCfgFilename + "activemq";//m_sProcName;
		sCfgFilename = sCfgFilename + ".cfg";

		m_pConfig = new CConfigImpl();
		//加载配置文件
		if (m_pConfig->Load(sCfgFilename) != 0)
		{
			cout << "加载配置文件[" << sCfgFilename << "]失败!" << endl;
			msleep(3);
			return ;
		}

		//初始化日志
		cout << "初始化日志..." << endl;
		// 初始化日志
		if (CLogger::Instance()->Initial(m_pConfig->GetProperties("logger")) != 0)
		{
			cout << "Init Log [" << "activemq.cfg" << "] failure!" << endl;
			msleep(3);
			return ;
		}

		cout << "启动日志..." << endl;

		// 启动日志
		if (CLogger::Instance()->Start() != 0)
		{
			cout << "Log start failure!" << endl;
			msleep(3);
			return;
		}

		string sTmp = "";
	    if (0 == m_pConfig->GetProperty("mem_que_name",sTmp))
		 sXQueName = sTmp;

		  if (0 == m_pConfig->GetProperty("mq_url",sTmp))
		 brokerURI = sTmp;

		 if (0 == m_pConfig->GetProperty("mq_user",sTmp))
		 mq_user = sTmp;

		if (0 == m_pConfig->GetProperty("mq_pwd",sTmp))
		 mq_pwd = sTmp;

		if (0 == m_pConfig->GetProperty("mq_topic",sTmp))
		 mq_topic = sTmp;

		if (0 == m_pConfig->GetProperty("node_id",sTmp))
		 uiNodeID = FromString<unsigned int>(sTmp);
			
		if (0 == m_pConfig->GetProperty("forex_inst_multi",sTmp))
		 dlForexInstMulti = FromString<double>(sTmp);

				
		if (0 == m_pConfig->GetProperty("other_inst_multi",sTmp))
		 dlOtherInstMulti = FromString<double>(sTmp);

		if (0 == m_pConfig->GetProperty("tm_check",sTmp))
		 blTsCheck = FromString<bool>(sTmp);

		
		//
		char szProcessName[_MAX_PATH];
		::GetModuleFileName(0, szProcessName, _MAX_PATH);
		//string sTmp = szProcessName;
		sTmp = szProcessName;
		sTmp = LeftSubRight(sTmp, '.');

        //sTmp = LeftSubRight(szFilePath, '.');
		m_oMemShareAlive.Bind(E_PROCESS_APP);
		if (FALSE == m_oMemShareAlive.Create(sTmp.c_str()))
		{
			setColor(12,0);
			sTmp = "m_oMemShareAlive.Create fail";
			cout << sTmp.c_str() << endl;
			CRLog(E_ERROR,"%s", sTmp.c_str());
		}
		else
		{
			cout << sTmp.c_str() << endl;
			unsigned int uiProcessID = ::GetCurrentProcessId();
			m_oMemShareAlive.IamAlive(uiProcessID);
			//m_oMemShareAlive.SetNodeID(uiNodeID);
		}


		CXQueue xQueue;
		do
		{
			setColor(12,0);
			int nRet = xQueue.Open(sXQueName.c_str());
			if (!xQueue.IsOpen())
			{
				cout << "打开共享内存信息队列[" << sXQueName << "]失败!10秒后再试!" << endl;
				CRLog(E_ERROR,"%s", sXQueName.c_str());
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




        //custom
		do
		{



			try {
				// Create a ConnectionFactory
				auto_ptr<ConnectionFactory> connectionFactory(
					ConnectionFactory::createCMSConnectionFactory( brokerURI ) );

				// Create a Connection
				connection = connectionFactory->createConnection(mq_user, mq_pwd);
				connection->start();

				// Create a Session
				if( this->sessionTransacted ) {
					session = connection->createSession( Session::SESSION_TRANSACTED );
				} else {
					session = connection->createSession( Session::AUTO_ACKNOWLEDGE );
				}

				// Create the destination (Topic or Queue)
				if( useTopic ) {
					destination = session->createTopic( "mq_topic" );
				} else {
					destination = session->createQueue( "mq_topic" );
				}

				// Create a MessageProducer from the Session to the Topic or Queue
				producer = session->createProducer( destination );
				producer->setDeliveryMode( DeliveryMode::NON_PERSISTENT );

				//// Create the Thread Id String
				//string threadIdStr = Integer::toString( Thread::getId() );

				//// Create a messages
				//string text = (string)"Hello world! from thread " + threadIdStr;

				//for( int ix=0; ix<numMessages; ++ix ){
				//	TextMessage* message = session->createTextMessage( text );

				//	message->setIntProperty( "Integer", ix );

				//	// Tell the producer to send the message
				//	printf( "Sent message #%d from thread %s\n", ix+1, threadIdStr.c_str() );
				//	producer->send( message );

				//	delete message;
				//}

				size_t nMessageId = 0;
				unsigned int uiTimeout = 2000;
				do
				{
					m_oMemShareAlive.IamAlive();
					//cout << "iamalive" << endl;
					long lRet = xQueue.Read((LPBYTE)pBuf, &uiBufSize, &nMessageId, NULL, uiTimeout);
					if (XQUEUE_ERROR_SUCCESS != lRet)
					{
						setColor(12,0);
						string strTmp ="There is no data in the  XQueue!" ;
						cout << strTmp.c_str() << endl;
						CRLog(E_ERROR,"%s", strTmp.c_str());
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

					setColor(11,0);

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
					CRLog(E_APPINFO,"%s", sMqTopic.c_str());
					producer->send(message.get());
				} while (true);


				delete[]pBuf;


			}catch ( CMSException& e ) {
				//e.printStackTrace();
				setColor(12,0);
				cout << "mq execption " << e.getMessage() + " 10秒后再试！" << endl;
				CRLog(E_ERROR,"%s", e.getMessage().c_str());
				//e.printStackTrace();
				Sleep(3000);
			}
		}while (1);
    }

private:

    void cleanup(){

        // Destroy resources.
        try{
            if( destination != NULL ) delete destination;
        }catch ( CMSException& e ) { e.printStackTrace(); }
        destination = NULL;

        try{
            if( producer != NULL ) delete producer;
        }catch ( CMSException& e ) { e.printStackTrace(); }
        producer = NULL;

        // Close open resources.
        try{
            if( session != NULL ) session->close();
            if( connection != NULL ) connection->close();
        }catch ( CMSException& e ) { e.printStackTrace(); }

        try{
            if( session != NULL ) delete session;
        }catch ( CMSException& e ) { e.printStackTrace(); }
        session = NULL;

        try{
            if( connection != NULL ) delete connection;
        }catch ( CMSException& e ) { e.printStackTrace(); }
        connection = NULL;
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

	CMemShareAlive			m_oMemShareAlive;
	CConfigImpl*		    m_pConfig;
	vector< CXQueueIo<QUOTATION>* >			m_vecQueueIo;

public:

    HelloWorldConsumer( const std::string& brokerURI,
                        long numMessages,
                        bool useTopic = false,
                        bool sessionTransacted = false,
                        long waitMillis = 30000 )
                         : latch(1), doneLatch(numMessages){
        this->connection = NULL;
        this->session = NULL;
        this->destination = NULL;
        this->consumer = NULL;
        this->waitMillis = waitMillis;
        this->useTopic = useTopic;
        this->sessionTransacted = sessionTransacted;
        this->brokerURI = brokerURI;
    }
    virtual ~HelloWorldConsumer(){
        cleanup();
    }

    void waitUntilReady() {
        latch.await();
    }

	//MQServer订阅  共享队列写
    virtual void run() {

        try {

			//string mq_user  = "jlt_mq";
			//string mq_pwd   = "jlt_mq123";
			//brokerURI = "tcp://10.0.2.107:61616";
			//string destURI = "jlt.srv.quote.engine.pubQuote.topic.queueName";

			//            // Create a ConnectionFactory
   //         ActiveMQConnectionFactory* connectionFactory =
   //             new ActiveMQConnectionFactory( brokerURI );

   //         // Create a Connection
   //         connection = connectionFactory->createConnection(mq_user, mq_pwd);
   //         delete connectionFactory;
   //         connection->start();

   //         connection->setExceptionListener(this);

			//bool clientAck = false;
   //         // Create a Session
   //         if( clientAck ) {
   //             session = connection->createSession( Session::CLIENT_ACKNOWLEDGE );
   //         } else {
   //             session = connection->createSession( Session::AUTO_ACKNOWLEDGE );
   //         }

   //         // Create the destination (Topic or Queue)
   //         if( useTopic ) {
   //             destination = session->createTopic( destURI );
   //         } else {
   //             destination = session->createQueue( destURI );
   //         }

   //         // Create a MessageConsumer from the Session to the Topic or Queue
   //         consumer = session->createConsumer( destination );
   //         consumer->setMessageListener( this );





			brokerURI = "tcp://10.0.2.107:61616";
			string sXQueName = "QUEUE_WJF_MQ_CLIENT_WRITE";
			string mq_user  = "jlt_mq";
			string mq_pwd   = "jlt_mq123";
			string mq_topic = "jlt.srv.quote.engine.pubQuote.topic.queueName";
			unsigned int uiNodeID = 6602;
            double dlForexInstMulti = 10000.0, dlOtherInstMulti = 100.0;
			bool  blTsCheck=false;
			//读取配置

			//初始化日志

			//写行情到共享队列

			//获取路径
			char szFileName[_MAX_PATH], szFilePath[_MAX_PATH];
			char * pcName;
			::GetModuleFileName(0, szFileName, _MAX_PATH);
			::GetFullPathName(szFileName, _MAX_PATH, szFilePath, &pcName);
			char szBuf[_MAX_PATH];
			strcpy(szBuf, pcName);
			*pcName = '\0';
			SetCurrentDirectory(szFilePath);

			//组配置文件名
			std::string sCfgFilename;
			sCfgFilename = szFilePath;
			sCfgFilename+= DEFUALT_CONF_PATH PATH_SLASH;
			sCfgFilename = sCfgFilename + "activemq";//m_sProcName;
			sCfgFilename = sCfgFilename + ".cfg";

			m_pConfig = new CConfigImpl();
			//加载配置文件
			if (m_pConfig->Load(sCfgFilename) != 0)
			{
				cout << "加载配置文件[" << sCfgFilename << "]失败!" << endl;
				msleep(3);
				return ;
			}

			//初始化日志
			cout << "初始化日志..." << endl;
			// 初始化日志
			if (CLogger::Instance()->Initial(m_pConfig->GetProperties("logger")) != 0)
			{
				cout << "Init Log [" << "activemq.cfg" << "] failure!" << endl;
				msleep(3);
				return ;
			}

			cout << "启动日志..." << endl;

			// 启动日志
			if (CLogger::Instance()->Start() != 0)
			{
				cout << "Log start failure!" << endl;
				msleep(3);
				return;
			}

			string sTmp = "";
			if (0 == m_pConfig->GetProperty("mem_que_name",sTmp))
			 sXQueName = sTmp;

			  if (0 == m_pConfig->GetProperty("mq_url",sTmp))
			 brokerURI = sTmp;

			 if (0 == m_pConfig->GetProperty("mq_user",sTmp))
			 mq_user = sTmp;

			if (0 == m_pConfig->GetProperty("mq_pwd",sTmp))
			 mq_pwd = sTmp;

			if (0 == m_pConfig->GetProperty("mq_topic",sTmp))
			 mq_topic = sTmp;

			if (0 == m_pConfig->GetProperty("node_id",sTmp))
			 uiNodeID = FromString<unsigned int>(sTmp);
				
			if (0 == m_pConfig->GetProperty("forex_inst_multi",sTmp))
			 dlForexInstMulti = FromString<double>(sTmp);

					
			if (0 == m_pConfig->GetProperty("other_inst_multi",sTmp))
			 dlOtherInstMulti = FromString<double>(sTmp);

			if (0 == m_pConfig->GetProperty("tm_check",sTmp))
			 blTsCheck = FromString<bool>(sTmp);


	        
			//
			char szProcessName[_MAX_PATH];
			::GetModuleFileName(0, szProcessName, _MAX_PATH);
			//string sTmp = szProcessName;
			sTmp = szProcessName;
			sTmp = LeftSubRight(sTmp, '.');
            sTmp += "Writer";

			//sTmp = LeftSubRight(szFilePath, '.');
			m_oMemShareAlive.Bind(E_PROCESS_APP);
			if (FALSE == m_oMemShareAlive.Create(sTmp.c_str()))
			{
				setColor(12,0);
				cout << "m_oMemShareAlive.Create fail" << endl;
				CRLog(E_ERROR,"%s", sXQueName.c_str());
			}
			else
			{
				cout << sTmp << endl;
				unsigned int uiProcessID = ::GetCurrentProcessId();
				m_oMemShareAlive.IamAlive(uiProcessID);
				//m_oMemShareAlive.SetNodeID(uiNodeID);
			}


			unsigned int uiXQueNum = 2;
			if (0 == m_pConfig->GetProperty("XQUE_NUM", sTmp))
			{
				uiXQueNum = strutils::FromString<unsigned int>(sTmp);
				if (uiXQueNum > 10)
					uiXQueNum = 2;
			}

			for (unsigned int uiIndex = 1; uiIndex <= uiXQueNum; uiIndex++)
			{
				string sCfgName = "XQUE" + strutils::ToString<unsigned int>(uiIndex);

				CConfig *pCfgWriter;
				pCfgWriter = m_pConfig->GetProperties(sCfgName);
				if (0 != pCfgWriter && !pCfgWriter->IsEmpty())
				{
				}
				else
				{
					pCfgWriter = m_pConfig;
				}
				CRLog(E_APPINFO,"初始化[%s]发布点", sCfgName.c_str());
				CXQueueIo<QUOTATION>* pWriter = new CXQueueIo<QUOTATION>();
				pWriter->Init(pCfgWriter);
				m_vecQueueIo.push_back(pWriter);

			}

            //启动
			int nCount = 0;
			for (vector< CXQueueIo<QUOTATION>*  >::iterator it = m_vecQueueIo.begin(); it != m_vecQueueIo.end(); ++it)
			{
				nCount++;
				if (0 != *it)
				{
					CRLog(E_APPINFO,"启动[XQUE%d]", nCount);
					(*it)->Start();
				}
			}



			//do{

				// Create a ConnectionFactory
				auto_ptr<ConnectionFactory> connectionFactory(
					ConnectionFactory::createCMSConnectionFactory( brokerURI ) );

				// Create a Connection
				connection = connectionFactory->createConnection(mq_user,mq_pwd);
				connection->start();
				connection->setExceptionListener(this);

				// Create a Session
				if( this->sessionTransacted == true ) {
					session = connection->createSession( Session::SESSION_TRANSACTED );
				} else {
					session = connection->createSession( Session::AUTO_ACKNOWLEDGE );
				}

				// Create the destination (Topic or Queue)
				if( useTopic ) {
					destination = session->createTopic( mq_topic);
				} else {
					destination = session->createQueue( mq_topic );
				}

				// Create a MessageConsumer from the Session to the Topic or Queue
				consumer = session->createConsumer( destination );

				consumer->setMessageListener( this );

				std::cout.flush();
				std::cerr.flush();

				// Indicate we are ready for messages.
				latch.countDown();

				// Wait while asynchronous messages come in.
				doneLatch.await( waitMillis +100000);
			//}while(1);

			
        } catch( CMSException& e ) {

            // Indicate we are ready for messages.
            latch.countDown();

            e.printStackTrace();
        }
    }

    // Called from the consumer since this class is a registered MessageListener.
    virtual void onMessage( const Message* message ){

        static int count = 0;

        try
        {
            count++;
            const TextMessage* textMessage =  dynamic_cast< const TextMessage* >( message );
            string text = "";

            if( textMessage != NULL ) {
                text = textMessage->getText();
            } else {
                text = "NOT A TEXTMESSAGE!";
            }

            //printf( "Message #%d Received: %s\n", count, text.c_str() );
            cout<< text << endl;

			unsigned int uiBlockSize = 1024;
		    unsigned int uiXQueLen = 1024;
			unsigned int uiBufSize = uiBlockSize*uiXQueLen;
			char* pBuf = new char[uiBufSize];
		    memset(pBuf, 0x00, uiBufSize);
			memcpy(pBuf,text.c_str(),text.length());

			QUOTATION quo = { 0 };
			quo.Decode(pBuf, uiBufSize);


			CRLog(E_APPINFO,"%s", text.c_str());

			//分发给相应队列处理
			int nCount = 0;
			for (vector< CXQueueIo<QUOTATION>*  >::iterator it = m_vecQueueIo.begin(); it != m_vecQueueIo.end(); ++it)
			{
				nCount++;
				if (0 != *it)
				{
					(*it)->Enque(quo);
				}
			}



        } catch (CMSException& e) {
            e.printStackTrace();
        }

        // Commit all messages.
        if( this->sessionTransacted ) {
            session->commit();
        }

        // No matter what, tag the count down latch until done.
        doneLatch.countDown();
    }

    // If something bad happens you see it here as this class is also been
    // registered as an ExceptionListener with the connection.
    virtual void onException( const CMSException& ex AMQCPP_UNUSED) {
        printf("CMS Exception occurred.  Shutting down client.\n");
        ex.printStackTrace();
        exit(1);
    }

private:

    void cleanup(){

        //*************************************************
        // Always close destination, consumers and producers before
        // you destroy their sessions and connection.
        //*************************************************

        // Destroy resources.
        try{
            if( destination != NULL ) delete destination;
        }catch (CMSException& e) { e.printStackTrace(); }
        destination = NULL;

        try{
            if( consumer != NULL ) delete consumer;
        }catch (CMSException& e) { e.printStackTrace(); }
        consumer = NULL;

        // Close open resources.
        try{
            if( session != NULL ) session->close();
            if( connection != NULL ) connection->close();
        }catch (CMSException& e) { e.printStackTrace(); }

        // Now Destroy them
        try{
            if( session != NULL ) delete session;
        }catch (CMSException& e) { e.printStackTrace(); }
        session = NULL;

        try{
            if( connection != NULL ) delete connection;
        }catch (CMSException& e) { e.printStackTrace(); }
        connection = NULL;
    }
};

//argv[1] 为传进的参数:
//0 ：Product
//1 ：Consumer
int main(int argc AMQCPP_UNUSED, char* argv[] AMQCPP_UNUSED) {

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
    // =====================
    // Use either stomp or openwire, the default ports are different for each
    //
    // Examples:
    //    tcp://127.0.0.1:61616                      default to openwire
    //    tcp://127.0.0.1:61616?wireFormat=openwire  same as above
    //    tcp://127.0.0.1:61613?wireFormat=stomp     use stomp instead
    //
    std::string brokerURI =
        "tcp://10.0.2.107:61616";
//        "?wireFormat=openwire"
//        "&connection.alwaysSyncSend=true"
//        "&connection.useAsyncSend=true"
//        "&transport.commandTracingEnabled=true"
//        "&transport.tcpTracingEnabled=true"
//        "&wireFormat.tightEncodingEnabled=true"
        ;

    //============================================================
    // set to true to use topics instead of queues
    // Note in the code above that this causes createTopic or
    // createQueue to be used in both consumer an producer.
    //============================================================
    bool useTopics = true;
    bool sessionTransacted = false;
    int numMessages = 2000;

    long long startTime = Date::getCurrentTimeMilliseconds();

	//string sTmp = argv[1];
	//unsigned int param = FromString<unsigned int>(sTmp);
	unsigned int param = 1;
	if(/*argc >=2 &&  */param == 1)
	{
		HelloWorldConsumer consumer( brokerURI, numMessages, useTopics, sessionTransacted );
		// Start the consumer thread.
	    Thread consumerThread( &consumer );
		consumerThread.start();
		// Wait for the consumer to indicate that its ready to go.
	    consumer.waitUntilReady();
	   // Wait for the threads to complete.
		consumerThread.join();
	}
	else
	{
		HelloWorldProducer producer( brokerURI, numMessages, useTopics );
		// Start the producer thread.
		Thread producerThread( &producer );
		producerThread.start();
		// Wait for the threads to complete.
		producerThread.join();

	}
    long long endTime = Date::getCurrentTimeMilliseconds();
    double totalTime = (endTime - startTime) / 1000.0;

    std::cout << "Time to completion = " << totalTime << " seconds." << std::endl;
    std::cout << "-----------------------------------------------------\n";
    std::cout << "Finished with the example." << std::endl;
    std::cout << "=====================================================\n";
}

// END SNIPPET: demo
