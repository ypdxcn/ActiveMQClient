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

#include <decaf/io/EOFException.h>

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


	
CMemShareAlive			m_oMemShareAlive;
CConfigImpl*		    m_pConfig = NULL;

string sXQueName = "QUEUE_WJF_MQ_CLIENT_WRITE";
string XQueName1;
string XQueName2;
string mq_user  = "jlt_mq";
string mq_pwd   = "jlt_mq123";
string mq_topic = "jlt.srv.quote.engine.pubQuote.topic.queueName";
double m_dlForexInstMulti = 10000.0, m_dlOtherInstMulti = 100.0;
bool   m_blTsCheck = false;
unsigned int uiBlockSize = 1024;
unsigned int uiXQueLen = 1024;
unsigned int uiBufSize = uiBlockSize*uiXQueLen;
string  uiLastSeqNo = "";
char* pBuf= NULL;
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

const int color_black		 = 0;
const int color_blue	     = 1;
const int color_green		 = 2;
const int color_light_green  = 3;
const int color_reed		 = 4;
const int color_purple		= 5;
const int color_yellow		= 6;
const int color_white		= 7;
const int color_gray		= 8;
//const int color_light_blue = 9;
//const int color_light_green = 10;
const int color_light_blue = 11;
const int color_light_red = 12;
const int color_light_purple = 13;
const int color_light_yellow = 14;
const int color_wight_white = 15;


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
	//CMemShareAlive			m_oMemShareAlive;
	//CConfigImpl*		    m_pConfig;
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

		CXQueue xQueue;
		do
		{
			setColor(12,0);
			int nRet = xQueue.Open(sXQueName.c_str());
			if (!xQueue.IsOpen())
			{
				string strTmp ="打开共享内存信息队列[";
                strTmp       += sXQueName;
				strTmp       += "]失败!10秒后再试!";
				//cout << "打开共享内存信息队列[" << sXQueName << "]失败!10秒后再试!" << endl;
				CRLog(E_ERROR,"%s", strTmp.c_str());
				m_oMemShareAlive.IamAlive();

				Sleep(10000);
			}
			else
			{
				break;
			}
		} while (1);


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
					destination = session->createTopic( mq_topic );
				} else {
					destination = session->createQueue( mq_topic );
				}

				// Create a MessageProducer from the Session to the Topic or Queue
				producer = session->createProducer( destination );
				producer->setDeliveryMode( DeliveryMode::NON_PERSISTENT );

			

				size_t nMessageId = 0;
				unsigned int uiTimeout = 2000;
				do
				{
					setColor(8);
					m_oMemShareAlive.IamAlive();
					//cout << "iamalive" << endl;
					long lRet = xQueue.Read((LPBYTE)pBuf, &uiBufSize, &nMessageId, NULL, uiTimeout);
					if (XQUEUE_ERROR_SUCCESS != lRet)
					{
						setColor(12);

						string strTmp ="共享队列 [";
						strTmp       += sXQueName;
						strTmp       += "]没有数据!";
						CRLog(E_ERROR,"%s",strTmp.c_str());
						continue;
					}

					setColor(11);
					CRLog(E_ERROR,"共享队列[XQueue]读完成");
                    setColor(8);

					QUOTATION quo = { 0 };
					quo.Decode(pBuf, uiBufSize);

					string instCode = quo.m_CodeInfo.m_acCode;
					double dlMulti = 100.00;
					unsigned short usPrecision = 2;
					if (instCode == "AUTD" || instCode == "AU9999")
					{
						dlMulti = 100;
						usPrecision = 2;
					}
					else if (instCode == "5120")
					{
						dlMulti = m_dlOtherInstMulti;
						usPrecision = 2;
					}
					else if (instCode == "USDCNH")
					{
						dlMulti = m_dlForexInstMulti;
						usPrecision = 4;
					}
					else
					{
						continue;
					}	

					if (m_blTsCheck)
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
					
					m_oMemShareAlive.IamAlive();

					producer->send(message.get());

					setColor(8);
                    CRLog(E_APPINFO,"发送到MQServer[%s]", sMqTopic.c_str());

				} while (true);


				//delete[]pBuf;


			}catch ( CMSException& e ) {
				//e.printStackTrace();
				setColor(12,0);
				//cout << "mq execption " << e.getMessage() + " 10秒后再试！" << endl;
				CRLog(E_ERROR,"Catch Exception:%s", e.getMessage().c_str());
				m_oMemShareAlive.IamAlive();
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

tryTest:
		setColor(color_gray);
        try {


            string  sTmp;
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

				m_oMemShareAlive.IamAlive();
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


			int nCount2=0;

			do{

				try
				{
					    m_oMemShareAlive.IamAlive();
					    nCount2++;


						CRLog(E_APPINFO,"创建 connectionFactory");

						// Create a ConnectionFactory
						auto_ptr<ConnectionFactory> connectionFactory(	ConnectionFactory::createCMSConnectionFactory( brokerURI ) );

						// Create a Connection
						connection = connectionFactory->createConnection(mq_user,mq_pwd);
						connection->start();
						connection->setExceptionListener(this);

						// Create a Session
						if( this->sessionTransacted == true ) 
						{
							session = connection->createSession( Session::SESSION_TRANSACTED );
						} else 
						{
							session = connection->createSession( Session::AUTO_ACKNOWLEDGE );
						}


						CRLog(E_APPINFO,"创建 createSession");



						// Create the destination (Topic or Queue)
						if( useTopic ) 
						{
							destination = session->createTopic( mq_topic);

							
							CRLog(E_APPINFO,"创建 createTopic");
						


						} else {
							destination = session->createQueue( mq_topic );

								
							CRLog(E_APPINFO,"创建 createQueue");
						
						}

						



						// Create a MessageConsumer from the Session to the Topic or Queue
						consumer = session->createConsumer( destination );

						consumer->setMessageListener( this );

						std::cout.flush();
						std::cerr.flush();

						// Indicate we are ready for messages.
						//屏蔽掉，可以持续运行收数据
						//latch.countDown();

						setColor(color_light_blue);
						CRLog(E_APPINFO,"已经订阅[%s]，等待接收数据，等待时间[%ld],循环计数[%u]", mq_topic.c_str(),waitMillis,nCount2);
						setColor(color_gray);

						// Wait while asynchronous messages come in.
						doneLatch.await( waitMillis);

						CRLog(E_APPINFO,"等待超时！", mq_topic.c_str(),waitMillis,nCount);
				}
				catch(decaf::io::EOFException)
				{
					goto tryTest;
				}
				catch(decaf::io::IOException)
				{
					goto tryTest;
				}
				catch(...)
				{
					setColor(12);
					CRLog(E_APPINFO,"异常退出！");
                    m_oMemShareAlive.IamAlive();
					setColor(8);
				}

				
			}while(true);

			
        } 
		catch( CMSException& e ) 
        {

            // Indicate we are ready for messages.
            latch.countDown();

            e.printStackTrace();
			setColor(color_light_red);
			CRLog(E_APPINFO,"异常退出[%s]", e.getMessage().c_str());
        }
		catch( ... ) 
		{
			setColor(color_light_red);
			CRLog(E_APPINFO,"异常退出");
        }

		goto tryTest;
    }

    // Called from the consumer since this class is a registered MessageListener.
    virtual void onMessage( const Message* message ){

        static int count = 0;
        setColor(8);
        try
        {
			m_oMemShareAlive.IamAlive();
            count++;
            const TextMessage* textMessage =  dynamic_cast< const TextMessage* >(message);
            string text = "";

            if( textMessage != NULL ) 
			{
                text = textMessage->getText();
            } 
			else 
			{
                text = "NOT A TEXTMESSAGE!";

				setColor(12);
				CRLog(E_APPINFO,"接收数据[%s]", text.c_str());
				return;
            }

		    memset(pBuf, 0x00, uiBufSize);	

			QUOTATION quo = { 0 };
			quo.Decode(pBuf, uiBufSize);

			CRLog(E_APPINFO,"接收原始数据[%s]", text.c_str());


			if(count > 1000000)
			{
					std::cout.flush();
					std::cerr.flush();
					count = 0;
			}

			//构造标准jason数组（加头尾）
			string  tmp = "{\"docs_list\":";
			tmp += text;
			tmp += "}";

			//text
			using rapidjson::Document;
			Document doc;
			doc.Parse<0>(tmp.c_str());

			//doc.ParseArray<0>(text.c_str());
			if (doc.HasParseError()) 
			{
				//rapidjson::ParseErrorCode code = doc.GetParseError();
				setColor(12);
				CRLog(E_APPINFO,"解析jason出错[%s]");
				return;
			}

            //解析数组
			Value& docs_list = doc["docs_list"];
			assert(docs_list.IsArray());

			//setColor(13);
			//CRLog(E_APPINFO,"暂时不下发，传输jason数据完善，再完善向下转发的代码");
			//return;
			for (int i=0; i<docs_list.Size(); i++)
			{
				//something here
				 Value & doc = docs_list[i];  
                 assert(doc.IsObject());
				 //解析对象
          
				    string seqNo     =  doc["sequenceNo"].GetString();

					if(uiLastSeqNo == seqNo)
					{
						setColor(color_light_blue);
						CRLog(E_APPINFO,"过滤掉重复序列号[%d]",quo.m_uiSeqNo);
						setColor(color_gray);
						continue;
					}
					uiLastSeqNo = seqNo;

					string maketCode = doc["market_code"].GetString();
					if( maketCode == "M004")
					 quo.m_CodeInfo.m_usMarketType = 0x6400;//HJ_MARKET + HJ_OTHER;//0x6500;//

					strcpy(quo.m_CodeInfo.m_acCode , doc["prod_code"].GetString());
					quo.m_uiDate           = FromString<unsigned int>(doc["quoteDate"].GetString());
					quo.m_uiTime           = FromString<unsigned int>(doc["quoteTime"].GetString());
					//quo.m_uiTime           *= 1000;

					string instCode = quo.m_CodeInfo.m_acCode;
					double dlMulti = 100.00;
					unsigned short usPrecision = 2;
					//if (instCode == "AUTD" || instCode == "AU9999")
					//{
					//	dlMulti = m_dlOtherInstMulti;
					//	usPrecision = 2;
					//}
					//else if (instCode == "5120")
					//{
					//	dlMulti = m_dlOtherInstMulti;
					//	usPrecision = 2;
					//}
					//else if (instCode == "USDCNH")
					//{
					//	dlMulti = m_dlForexInstMulti;
					//	usPrecision = 4;
					//}



					quo.m_Bid[0].m_uiPrice =  doc["buyPrice"].GetDouble()*dlMulti;
					quo.m_Ask[0].m_uiPrice =  doc["sellPrice"].GetDouble()*dlMulti;
					quo.m_uiLast           =  doc["lastPrice"].GetDouble()*dlMulti;
					quo.m_uiSettle         =  doc["settlePrice"].GetDouble()*dlMulti;
					quo.m_uiAverage        =  doc["avgPrice"].GetDouble()*dlMulti;
					quo.m_uiClose          =  doc["closePrice"].GetDouble()*dlMulti;
					quo.m_uilastClose      =  doc["lastClosePrice"].GetDouble()*dlMulti;
					quo.m_uiLastSettle     =  doc["lastSettlePrice"].GetDouble()*dlMulti;
					
					setColor(color_light_blue);
					CRLog(E_APPINFO,"向下分发数据为{\"market_code\":%d,\"prod_code\":\"%s\",\"quoteDate\":%d,\"quoteTime\":%d,\"sequenceNo\":%s,\"buyPrice\":\"%d\",\"sellPrice\":\"%d\",\"lastPrice\":\"%d\",\"settlePrice\":\"%d\",\"avgPrice\":\"%d\",\"closePrice\":\"%d\",\"lastClosePrice\":\"%d\",\"lastSettlePrice\":\"%d\"}" , 
												quo.m_CodeInfo.m_usMarketType,
												quo.m_CodeInfo.m_acCode,
												quo.m_uiDate,quo.m_uiTime,
												uiLastSeqNo.c_str(),
												quo.m_Bid[0].m_uiPrice,
												quo.m_Ask[0].m_uiPrice,
												quo.m_uiLast,
												quo.m_uiSettle,
												quo.m_uiAverage,
												quo.m_uiClose,
												quo.m_uilastClose,
												quo.m_uiLastSettle);


					//分发给相应队列处理
					int uiCount = 0;
					string  sXQueName,sTmp;
					for (vector< CXQueueIo<QUOTATION>*  >::iterator it = m_vecQueueIo.begin(); it != m_vecQueueIo.end(); ++it)
					{
						uiCount++;
						if (0 != *it)
						{
							(*it)->Enque(quo);

							sXQueName = "XQUE" + strutils::ToString<unsigned int>(uiCount);
							sXQueName +=".XQUE_NAME";


							if (0 == m_pConfig->GetProperty(sXQueName,sTmp))
							   sXQueName = sTmp;

							setColor(color_light_purple);
							CRLog(E_APPINFO,"共享队列XQueue[%s]写完成",sXQueName.c_str());
							setColor(color_gray);
						}
						
					}//end for



			}//end for

			 
			m_oMemShareAlive.IamAlive();
		

        } catch (CMSException& e) {
            e.printStackTrace();
        }

        // Commit all messages.
        if( this->sessionTransacted ) {
            session->commit();
        }

        // No matter what, tag the count down latch until done.
        //doneLatch.countDown();
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

	//读取配置
	unsigned int uiNodeID = 6602;
    double dlForexInstMulti = 10000.0, dlOtherInstMulti = 100.0;
	bool  blTsCheck=false;
	unsigned int param = 1;

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
	sCfgFilename = sCfgFilename + "activemq";
	sCfgFilename = sCfgFilename + ".cfg";

	m_pConfig = new CConfigImpl();
	//加载配置文件
	if (m_pConfig->Load(sCfgFilename) != 0)
	{
		cout << "加载配置文件[" << sCfgFilename << "]失败!" << endl;
		msleep(3);
		return 0;
	}

	//初始化日志
	cout << "初始化日志..." << endl;
	// 初始化日志
	if (CLogger::Instance()->Initial(m_pConfig->GetProperties("logger")) != 0)
	{
		cout << "Init Log [" << "activemq.cfg" << "] failure!" << endl;
		msleep(3);
		return 0;
	}

	cout << "启动日志..." << endl;

	// 启动日志
	if (CLogger::Instance()->Start() != 0)
	{
		cout << "Log start failure!" << endl;
		msleep(3);
		return 0;
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

	if (0 == m_pConfig->GetProperty("Consumer",sTmp))
	 param = FromString<unsigned int>(sTmp);

    //全局赋值
	m_dlForexInstMulti = dlForexInstMulti;
	m_dlOtherInstMulti = dlOtherInstMulti;
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
		cout << "MemShareAlive.Create fail" << endl;
		CRLog(E_ERROR,"%s", sXQueName.c_str());
	}
	else
	{
		cout << "MemShareAlive.Create Successfull." << endl;
		unsigned int uiProcessID = ::GetCurrentProcessId();
		m_oMemShareAlive.IamAlive(uiProcessID);
		//m_oMemShareAlive.SetNodeID(uiNodeID);
	}


	pBuf = new char[uiBufSize];

	if(param == 1)
	{
		cout << "Consumer Mode Start..." << endl;
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
		cout << "Producer Mode Start..." << endl;

		HelloWorldProducer producer( brokerURI, numMessages, useTopics );
		// Start the producer thread.
		Thread producerThread( &producer );
		producerThread.start();
		// Wait for the threads to complete.
		producerThread.join();

	}
    long long endTime = Date::getCurrentTimeMilliseconds();
    double totalTime = (endTime - startTime) / 1000.0;

	delete []pBuf;

    std::cout << "Time to completion = " << totalTime << " seconds." << std::endl;
    std::cout << "-----------------------------------------------------\n";
    std::cout << "Finished with the example." << std::endl;
    std::cout << "=====================================================\n";
}

// END SNIPPET: demo
