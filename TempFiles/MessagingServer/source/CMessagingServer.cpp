#include "CMessagingServer.h"
#include <algorithm>

std::queue<int> CMessagingServer::m_qObseleteMessageHandlerQueue;
std::queue<MessageData*> CMessagingServer::m_qReceivedMessageQueue;
std::queue<MessageData*> CMessagingServer::m_qSendMessageQueue;
std::map<int,CMessagingServer*> CMessagingServer::m_mActiveMessagingServerList;
pthread_t  *CMessagingServer::m_upMessageHandlerthreadId = NULL;
pthread_t *CMessagingServer::m_upMessageSenderthreadId = NULL;
pthread_t CMessagingServer::m_uMessageReceiverThreadId;
pthread_t CMessagingServer::m_uObseleteMessageHandlerthreadId;
pthread_mutex_t CMessagingServer::m_uMessageHandlerMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t CMessagingServer::m_uSendMessageMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t CMessagingServer::m_uActiveServerListMutex = PTHREAD_MUTEX_INITIALIZER;
int CMessagingServer::m_iMaxMessageSenderThreads = 0;
int CMessagingServer::m_iMaxMessageHandlerThreads = 0;


#define MAX_RANDOM_STRING_LENGTH 50

CMessagingServer::CMessagingServer()
{
}
CMessagingServer::~CMessagingServer()
{
}

void CMessagingServer::SetMaxMessageSenderThreads(int a_iMaxMessageSendThreads)
{
	CMessagingServer::m_iMaxMessageSenderThreads = a_iMaxMessageSendThreads;
}
int CMessagingServer::GetMaxMessageSenderThreads()
{
	return CMessagingServer::m_iMaxMessageSenderThreads;
}

void CMessagingServer::SetMaxMessageHandlerThreads(int a_iMaxMessageHandlerThreads)
{
	CMessagingServer::m_iMaxMessageHandlerThreads = a_iMaxMessageHandlerThreads;
}
int CMessagingServer::GetMaxMessageHandlerThreads()
{
	return CMessagingServer::m_iMaxMessageHandlerThreads;
}
bool CMessagingServer::GetMessagingServerActiveStatus()
{
	return this->m_bMessagingServerActiveStatus;
}

void CMessagingServer::SetMessagingServerActiveStatus(bool a_bMessagingServerActiveStatus)
{
	this->m_bMessagingServerActiveStatus = a_bMessagingServerActiveStatus;
}
void CMessagingServer::OnMessageReceive(std::string a_sReceivedMessage,int a_iReceivedMessageLength)
{
	bool l_bMessageHandlerMutexStatus = false;
	try
	{
		CFileHandler::GetInstance().WriteToFile(900,"REQUEST:SockeFD:%d:%s", this->GetSocketInstance()->GetSocketFD(),a_sReceivedMessage.c_str());	
		if(a_iReceivedMessageLength > 0)
		{
			MessageData *l_upMessageData = NULL;
			l_upMessageData = new MessageData;
			if(l_upMessageData == NULL)
			{
				CException l_uException("Allocating memory for new MessageData failed");
				throw l_uException;
			}
			l_upMessageData->m_iSocketFD = this->GetSocketInstance()->GetSocketFD();
			l_upMessageData->m_sData = a_sReceivedMessage;
			pthread_mutex_lock(&CMessagingServer::m_uMessageHandlerMutex);
			l_bMessageHandlerMutexStatus = true;
			CMessagingServer::m_qReceivedMessageQueue.push(l_upMessageData);
			pthread_mutex_unlock(&CMessagingServer::m_uMessageHandlerMutex);
			l_bMessageHandlerMutexStatus = false;
		}
	}
	catch(CException a_uException)
	{
		if(l_bMessageHandlerMutexStatus)
		{
			pthread_mutex_unlock(&CMessagingServer::m_uMessageHandlerMutex);
		}
		CFileHandler::GetInstance().WriteToFile(500,"Exception Raised in CMessagingServer::OnMessageReceive:%s",a_uException.what());	
	}
	catch(...)
	{
		if(l_bMessageHandlerMutexStatus)
		{
			pthread_mutex_unlock(&CMessagingServer::m_uMessageHandlerMutex);
		}
		CFileHandler::GetInstance().WriteToFile(500,"Unknown Exception raised in CMessagingServer::OnMessageReceive:Error in handling new messages :ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));	
	}
}
void CMessagingServer::OnAccept(int a_iSocketFD)
{
	CFileHandler::GetInstance().WriteToFile(400,"Received New Connection Request on Socket FD:%d", a_iSocketFD);	
	CMessagingServer* l_upMessagingServer = NULL;
	try
	{
		l_upMessagingServer = new CMessagingServer;
		if(l_upMessagingServer == NULL)
		{
			CException l_uException("Memory Allocation for new Messaging handling  Server Failed");
			throw l_uException;
		}
		if ( !l_upMessagingServer->MakeServerSocketInstance(false,a_iSocketFD) )
		{
			char l_cpDescription[2048] = {0x00};
			sprintf(l_cpDescription,"MakeServerSocketInstance returned FALSE for SocketFD:%d",a_iSocketFD);
			
			CException l_uException(l_cpDescription);
			throw l_uException;
		}

	}
	catch(CException a_uException)
	{
		if(l_upMessagingServer != NULL)
		{
			delete l_upMessagingServer;
		}
		this->ForceDisconnect(a_iSocketFD);
		CFileHandler::GetInstance().WriteToFile(500,"Exception Raised in CMessagingServer::OnAccept:%s",a_uException.what());	
	}
	catch(...)
	{
		if(l_upMessagingServer != NULL)
		{
			delete l_upMessagingServer;
		}
		this->ForceDisconnect(a_iSocketFD);
		CFileHandler::GetInstance().WriteToFile(500,"Unknown Exception raised in CMessagingServer::OnAccept while creating new instance for new socket :ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));	
		return;	
	}
	l_upMessagingServer->SetMessagingServerActiveStatus(true);
	l_upMessagingServer->GetSocketInstance()->SetConnectionStatus(true);
	pthread_mutex_lock(&CMessagingServer::m_uActiveServerListMutex);
	CMessagingServer::m_mActiveMessagingServerList[a_iSocketFD] = l_upMessagingServer;
	pthread_mutex_unlock(&CMessagingServer::m_uActiveServerListMutex);

	int l_iThreadAttrInitStatus;
	int l_iSetDetachStateStatus;
	int l_iThreadCreationStatus;
	pthread_attr_t l_uThreadAttribute;
	try
	{
		l_iThreadAttrInitStatus = pthread_attr_init(&l_uThreadAttribute);
                if(l_iThreadAttrInitStatus != 0 )
                {
			CException l_uException("Error in Initializing Thread Attributes in OnAccept");
                        throw l_uException;
                }
                l_iSetDetachStateStatus = pthread_attr_setdetachstate(&l_uThreadAttribute, PTHREAD_CREATE_DETACHED);
                if(l_iSetDetachStateStatus != 0 )
                {
			CException l_uException("Error in Setting Detach state in OnAccept");
                        throw l_uException;
                }
                l_iThreadCreationStatus = pthread_create(&l_upMessagingServer->m_uMessageReceiverThreadId, &l_uThreadAttribute,CMessagingServer::ThreadMessageReceiver , (void*)l_upMessagingServer);
                if(l_iThreadCreationStatus != 0 )
                {
			CException l_uException("Error in creating thread ThreadMessageReceiver");
                        throw l_uException;
                }
                pthread_attr_destroy(&l_uThreadAttribute);
	}
	catch(CException a_uException)
	{
		pthread_mutex_lock(&CMessagingServer::m_uActiveServerListMutex);
		CMessagingServer::m_mActiveMessagingServerList.erase(a_iSocketFD);
		pthread_mutex_unlock(&CMessagingServer::m_uActiveServerListMutex);
		if(l_upMessagingServer !=NULL)
		{
			delete l_upMessagingServer;
		}
		this->ForceDisconnect(a_iSocketFD);
		if(l_iThreadAttrInitStatus == 0)
		{
			pthread_attr_destroy(&l_uThreadAttribute);
		}
		CFileHandler::GetInstance().WriteToFile(500,"Exception Raised in CMessagingServer::OnAccept:%s",a_uException.what());	
	}
        catch(...)
        {
		pthread_mutex_lock(&CMessagingServer::m_uActiveServerListMutex);
		CMessagingServer::m_mActiveMessagingServerList.erase(a_iSocketFD);
		pthread_mutex_unlock(&CMessagingServer::m_uActiveServerListMutex);
		if(l_upMessagingServer !=NULL)
		{
			delete l_upMessagingServer;
		}
		this->ForceDisconnect(a_iSocketFD);
		if(l_iThreadAttrInitStatus == 0)
		{
			pthread_attr_destroy(&l_uThreadAttribute);
		}
		CFileHandler::GetInstance().WriteToFile(500,"Unknown Exception raised in CMessagingServer::OnAccept: Error in creating ThreadMessageReceiver :ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));	
        }
	
}
void CMessagingServer::OnDisconnect(int a_iSocketFD)
{
	CMessagingServer::m_qObseleteMessageHandlerQueue.push(a_iSocketFD);
}

void* CMessagingServer::ThreadMessageHandler(void* a_upArguments)
{
	CMessagingServer *l_upInvokedObject = (CMessagingServer*)a_upArguments;
	
	while(CMessagingServer::GetServerRunningStatus())
	{
		MessageData	*l_upMessageData = NULL;
		MessageData	*l_upMessageDataToSend = NULL;
		bool		l_bMessageHandlerMutexStatus = false;
		bool		l_bSendMessageMutexStatus = false;
		try
		{
			pthread_mutex_lock(&CMessagingServer::m_uMessageHandlerMutex);
			l_bMessageHandlerMutexStatus = true;
			if(CMessagingServer::m_qReceivedMessageQueue.size()>0)
			{
				l_upMessageData = CMessagingServer::m_qReceivedMessageQueue.front();
				CMessagingServer::m_qReceivedMessageQueue.pop();
				pthread_mutex_unlock(&CMessagingServer::m_uMessageHandlerMutex);
				l_bMessageHandlerMutexStatus = false;
				if(l_upMessageData != NULL)
				{
					MessageProcessorAction l_uMessageProcessorAction;
					std::string l_sResponseMessage;
					l_uMessageProcessorAction = CMessagingServer::ProcessMessage(l_upMessageData->m_sData, l_sResponseMessage);				
					switch(l_uMessageProcessorAction)
					{
						case SendResponse:
							l_upMessageDataToSend = new MessageData;
							l_upMessageDataToSend->m_iSocketFD = l_upMessageData->m_iSocketFD;
							l_upMessageDataToSend->m_sData = l_sResponseMessage; 
							pthread_mutex_lock(&CMessagingServer::m_uSendMessageMutex);
							l_bSendMessageMutexStatus = true;
							CMessagingServer::m_qSendMessageQueue.push(l_upMessageDataToSend);
							pthread_mutex_unlock(&CMessagingServer::m_uSendMessageMutex);
							l_bSendMessageMutexStatus = false;
							break;
						case CloseConnection:
							CMessagingServer::m_qObseleteMessageHandlerQueue.push(l_upMessageData->m_iSocketFD);

							break;
						default:
							break;
					};
					delete l_upMessageData;
				}
			}
			else
			{
				pthread_mutex_unlock(&CMessagingServer::m_uMessageHandlerMutex);
				l_bMessageHandlerMutexStatus = false;
			}
		}
		catch(CException a_uException)
		{
			CFileHandler::GetInstance().WriteToFile(500,"Exception Raised in CMessagingServer::ThreadMessageHandler:%s",a_uException.what());	
			if(l_bMessageHandlerMutexStatus)
			{
				pthread_mutex_unlock(&CMessagingServer::m_uMessageHandlerMutex);
			}
			if(l_bSendMessageMutexStatus)
			{
				pthread_mutex_unlock(&CMessagingServer::m_uSendMessageMutex);
			}
			if(l_upMessageData != NULL)
			{
				delete l_upMessageData;
			}
		}
		catch(...)
		{
			CFileHandler::GetInstance().WriteToFile(500,"Unknown Exception Raised in CMessagingServer::ThreadMessageHandler:ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));	
			if(l_bMessageHandlerMutexStatus)
			{
				pthread_mutex_unlock(&CMessagingServer::m_uMessageHandlerMutex);
			}
			if(l_bSendMessageMutexStatus)
			{
				pthread_mutex_unlock(&CMessagingServer::m_uSendMessageMutex);
			}
			if(l_upMessageData != NULL)
			{
				delete l_upMessageData;
			}
		}

		usleep(1000);
	}
	CFileHandler::GetInstance().WriteToFile(400,"\nExiting from ThreadMessageHandler");	
	//pthread_exit(NULL);
}
void* CMessagingServer::ThreadMessageSender(void* a_upArguments)
{
	CMessagingServer *l_upInvokedObject = (CMessagingServer*)a_upArguments;
	CMessagingServer *l_upCurrentObject = NULL;
	while(CMessagingServer::GetServerRunningStatus())
	{
		MessageData	*l_upMessageData = NULL;
		bool l_bSendMutexStatus = false;
		bool l_bActiveServerListMutexStatus = false;
		try
		{
		pthread_mutex_lock(&CMessagingServer::m_uSendMessageMutex);
		l_bSendMutexStatus = true;
		if(CMessagingServer::m_qSendMessageQueue.size()>0)
		{
			l_upMessageData = CMessagingServer::m_qSendMessageQueue.front();
			CMessagingServer::m_qSendMessageQueue.pop();
			pthread_mutex_unlock(&CMessagingServer::m_uSendMessageMutex);
			l_bSendMutexStatus = false;
			if(l_upMessageData != NULL)
			{
				l_upCurrentObject = NULL;
		
				pthread_mutex_lock(&CMessagingServer::m_uActiveServerListMutex);
				l_bActiveServerListMutexStatus = true;
				if(CMessagingServer::m_mActiveMessagingServerList.find(l_upMessageData->m_iSocketFD) != CMessagingServer::m_mActiveMessagingServerList.end())
				{
					l_upCurrentObject = CMessagingServer::m_mActiveMessagingServerList[l_upMessageData->m_iSocketFD];
					if(l_upCurrentObject == NULL )
                                        {	
						CMessagingServer::m_mActiveMessagingServerList.erase(l_upMessageData->m_iSocketFD);
						CFileHandler::GetInstance().WriteToFile(400,"Invalid pointer found in m_mActiveMessagingServerList for SocketFD=%d",l_upMessageData->m_iSocketFD);	
					}
					pthread_mutex_unlock(&CMessagingServer::m_uActiveServerListMutex);
					l_bActiveServerListMutexStatus = false;
					if(l_upCurrentObject != NULL )
					{
						if( l_upCurrentObject->GetMessagingServerActiveStatus() )
						{
							int l_iSentMessageLength = 0;
							if(!l_upCurrentObject->SendMessage(l_upMessageData->m_sData, l_iSentMessageLength))
							{
								CFileHandler::GetInstance().WriteToFile(400,"Error in sending message to Socket FD=%d",l_upMessageData->m_iSocketFD);	
							}
							CFileHandler::GetInstance().WriteToFile(900,"RESPONSE:SocketFD=%d:%s",l_upMessageData->m_iSocketFD,l_upMessageData->m_sData.c_str());	
						}
					}
				}
				else
				{
					pthread_mutex_unlock(&CMessagingServer::m_uActiveServerListMutex);
					l_bActiveServerListMutexStatus = false;
				}
				delete l_upMessageData;
			}
		}
		else
		{
			pthread_mutex_unlock(&CMessagingServer::m_uSendMessageMutex);
			l_bSendMutexStatus = false;
		}
		}
		catch(CException a_uException)
		{
			CFileHandler::GetInstance().WriteToFile(500,"Exception Raised in CMessagingServer::ThreadMessageSender:%s",a_uException.what());	
			if(l_bSendMutexStatus)
			{
				pthread_mutex_unlock(&CMessagingServer::m_uSendMessageMutex);
			}
			if(l_bActiveServerListMutexStatus)
			{
				pthread_mutex_unlock(&CMessagingServer::m_uActiveServerListMutex);
			}
			if(l_upMessageData != NULL)
			{
				delete l_upMessageData;
			}
		}
		catch(...)
		{
			CFileHandler::GetInstance().WriteToFile(500,"Unknown Exception Raised in CMessagingServer::ThreadMessageSender:ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));	
			if(l_bSendMutexStatus)
			{
				pthread_mutex_unlock(&CMessagingServer::m_uSendMessageMutex);
			}
			if(l_bActiveServerListMutexStatus)
			{
				pthread_mutex_unlock(&CMessagingServer::m_uActiveServerListMutex);
			}
			if(l_upMessageData != NULL)
			{
				delete l_upMessageData;
			}

		}

		usleep(1000);
	}
	CFileHandler::GetInstance().WriteToFile(400,"\nExiting from ThreadMessageSender");	
	//pthread_exit(NULL);
}
void* CMessagingServer::ThreadDeleteObseleteMessageHandlers(void* a_upArguments)
{
	CMessagingServer *l_upCurrentObject = (CMessagingServer*)a_upArguments;
	
	while(CMessagingServer::GetServerRunningStatus())
	{
		CMessagingServer* l_upObseleteObject = NULL;
		int l_iSocketFD = 0;
		bool l_bMutexStatus = false;
		try
		{	
		if(CMessagingServer::m_qObseleteMessageHandlerQueue.size()>0)
		{
			l_iSocketFD = CMessagingServer::m_qObseleteMessageHandlerQueue.front();
			CMessagingServer::m_qObseleteMessageHandlerQueue.pop();
			
			pthread_mutex_lock(&CMessagingServer::m_uActiveServerListMutex);
			l_bMutexStatus = true;
			if(CMessagingServer::m_mActiveMessagingServerList.find(l_iSocketFD) != CMessagingServer::m_mActiveMessagingServerList.end())
			{
				l_upObseleteObject = CMessagingServer::m_mActiveMessagingServerList[l_iSocketFD];
				CMessagingServer::m_mActiveMessagingServerList.erase(l_iSocketFD);
				pthread_mutex_unlock(&CMessagingServer::m_uActiveServerListMutex);
				l_bMutexStatus = false;
				CFileHandler::GetInstance().WriteToFile(400,"Removed object from m_mActiveMessagingServerList:SocketFD=%d",l_iSocketFD);	
				if(l_upObseleteObject != NULL)
				{
					l_upObseleteObject->GetSocketInstance()->SetConnectionStatus(false);
					l_upObseleteObject->SetMessagingServerActiveStatus(false);
					l_upObseleteObject->DisconnectClient();
					sleep(1);
					l_upObseleteObject->UnSetSocketInstance();
					delete l_upObseleteObject;
				}
			}
			else
			{
				pthread_mutex_unlock(&CMessagingServer::m_uActiveServerListMutex);
				l_bMutexStatus = false;
			}
		}
		}
		catch (CException a_uException)
		{
			if(l_bMutexStatus)
			{
				pthread_mutex_lock(&CMessagingServer::m_uActiveServerListMutex);
			}
			CFileHandler::GetInstance().WriteToFile(500,"Exception Raised in CMessagingServer::ThreadDeleteObseleteMessageHandlers:%s",a_uException.what());	
		}
		catch (...)
		{
			if(l_bMutexStatus)
			{
				pthread_mutex_lock(&CMessagingServer::m_uActiveServerListMutex);
			}
			CFileHandler::GetInstance().WriteToFile(500,"Unknown Exception Raised in CMessagingServer::ThreadDeleteObseleteMessageHandlers:ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));	
		}
		usleep(1000);
	}
	CFileHandler::GetInstance().WriteToFile(400,"\nExiting from ThreadDeleteObseleteMessageHandlers");	
	//pthread_exit(NULL);
}

bool CMessagingServer::StartThreads()
{
	int l_iThreadAttrInitStatus;
	int l_iSetDetachStateStatus;
	int l_iThreadCreationStatus;
	pthread_attr_t l_uThreadAttribute;
	pthread_t l_uThreadId;
	try
	{
		l_iThreadAttrInitStatus = pthread_attr_init(&l_uThreadAttribute);
                if(l_iThreadAttrInitStatus != 0 )
                {
			CException l_uException("Error in Initializing Thread Attributes in OnAccept");
                        throw l_uException;
                }
                l_iSetDetachStateStatus = pthread_attr_setdetachstate(&l_uThreadAttribute, PTHREAD_CREATE_DETACHED);
                if(l_iSetDetachStateStatus != 0 )
                {
			CException l_uException("Error in Setting Detach state in OnAccept");
                        throw l_uException;
                }
		CMessagingServer::m_upMessageSenderthreadId = new pthread_t[CMessagingServer::m_iMaxMessageSenderThreads];
		if(CMessagingServer::m_upMessageSenderthreadId == NULL )
		{
			CException l_uException("Error in allocating memory for CMessagingServer::m_uMessageSenderthreadId");
			throw l_uException;
		}
	
		for(int l_iIndex = 0;l_iIndex<CMessagingServer::m_iMaxMessageSenderThreads;l_iIndex++)
		{
			CFileHandler::GetInstance().WriteToFile(400,"Starting ThreadMessageSender Number:%d",l_iIndex+1);	
			//l_iThreadCreationStatus = pthread_create(&l_uThreadId, &l_uThreadAttribute,ThreadMessageSender , (void*)this);
			l_iThreadCreationStatus = pthread_create(&CMessagingServer::m_upMessageSenderthreadId[l_iIndex], &l_uThreadAttribute,ThreadMessageSender , (void*)this);
			if(l_iThreadCreationStatus != 0 )
			{
				CException l_uException("Error in creating thread ThreadMessageSender");
				throw l_uException;
			}
		}
		CMessagingServer::m_upMessageHandlerthreadId = new pthread_t[CMessagingServer::m_iMaxMessageHandlerThreads];
		if(CMessagingServer::m_upMessageHandlerthreadId == NULL )
		{
			CException l_uException("Error in allocating memory for CMessagingServer::m_upMessageHandlerthreadId");
			throw l_uException;
		}
		CFileHandler::GetInstance().WriteToSysLog("Started ThreadMessageSender");	
		for(int l_iIndex = 0;l_iIndex<CMessagingServer::m_iMaxMessageHandlerThreads;l_iIndex++)
		{
			CFileHandler::GetInstance().WriteToFile(400,"Starting ThreadMessageHandler Number:%d",l_iIndex+1);	
			//l_iThreadCreationStatus = pthread_create(&l_uThreadId, &l_uThreadAttribute,ThreadMessageHandler , (void*)this);
			l_iThreadCreationStatus = pthread_create(&CMessagingServer::m_upMessageHandlerthreadId[l_iIndex], &l_uThreadAttribute,ThreadMessageHandler , (void*)this);
			if(l_iThreadCreationStatus != 0 )
			{
				CException l_uException("Error in creating thread ThreadMessageSender");
				throw l_uException;
			}
		}
		CFileHandler::GetInstance().WriteToSysLog("Started ThreadMessageHandler");	
		CFileHandler::GetInstance().WriteToFile(400,"Starting ThreadDeleteObseleteMessageHandlers");	
		l_iThreadCreationStatus = pthread_create(&CMessagingServer::m_uObseleteMessageHandlerthreadId, &l_uThreadAttribute,ThreadDeleteObseleteMessageHandlers , (void*)this);
                if(l_iThreadCreationStatus != 0 )
                {
			CException l_uException("Error in creating thread ThreadDeleteObseleteMessageHandlers");
			throw l_uException;
                }
		CFileHandler::GetInstance().WriteToSysLog("Started ThreadDeleteObseleteMessageHandlers");	
                pthread_attr_destroy(&l_uThreadAttribute);
		return true;
	}
	catch(CException a_uException)
	{
		if(l_iThreadAttrInitStatus == 0)
		{
			pthread_attr_destroy(&l_uThreadAttribute);
		}
		
		CFileHandler::GetInstance().WriteToFile(400,"Exception Raised in CMessagingServer::StartThreads:%s",a_uException.what());	
		return false;	
	}
        catch(...)
        {
		if(l_iThreadAttrInitStatus == 0)
		{
			pthread_attr_destroy(&l_uThreadAttribute);
		}
	
		CFileHandler::GetInstance().WriteToFile(400,"Unknown Exception Raised in CMessagingServer::StartThreads:ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));	
		return false;
        }
}
bool CMessagingServer::StopThreads()
{
	try
	{
		this->SetServerRunningStatus(false);
		sleep(1);
		if(CMessagingServer::m_upMessageSenderthreadId != NULL )
		{
			delete []CMessagingServer::m_upMessageSenderthreadId;
			CMessagingServer::m_upMessageSenderthreadId = NULL;
		}
		if(CMessagingServer::m_upMessageHandlerthreadId != NULL )
		{
			delete []CMessagingServer::m_upMessageHandlerthreadId;
			CMessagingServer::m_upMessageHandlerthreadId = NULL;
		}
		while(!CMessagingServer::m_qObseleteMessageHandlerQueue.empty()) 
		{
			CMessagingServer::m_qObseleteMessageHandlerQueue.pop();
		}
		while(!CMessagingServer::m_qSendMessageQueue.empty()) 
		{
			CMessagingServer::m_qSendMessageQueue.pop();
		}
		while(!CMessagingServer::m_qReceivedMessageQueue.empty()) 
		{
			CMessagingServer::m_qReceivedMessageQueue.pop();
		}
		
		CFileHandler::GetInstance().WriteToSysLog("Stoped CMessagingServer Threads");	
		std::map<int,CMessagingServer*>::iterator l_uActiveServerListIterator;
		for(l_uActiveServerListIterator = CMessagingServer::m_mActiveMessagingServerList.begin();l_uActiveServerListIterator!=CMessagingServer::m_mActiveMessagingServerList.end();l_uActiveServerListIterator++)
		{
			if(l_uActiveServerListIterator->second != NULL)
			{
				l_uActiveServerListIterator->second->GetSocketInstance()->SetConnectionStatus(false);
				sleep(1);
				delete l_uActiveServerListIterator->second;
			}
		}
		CMessagingServer::m_mActiveMessagingServerList.clear();

	}
	catch(...)
	{
		CFileHandler::GetInstance().WriteToFile(400,"Unknown Exception Raised in CMessagingServer::StopThreads:ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));	
		CFileHandler::GetInstance().WriteToSysLog("Unknown Exception Raised in CMessagingServer::StopThreads:ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));	

	}
}
MessageProcessorAction CMessagingServer::ProcessMessage(std::string a_sReceivedData, std::string& a_sResponseMessage)
{
	try
	{	
		if(a_sReceivedData.length() > 0)
		{
			std::string l_sReceivedData;
			l_sReceivedData = a_sReceivedData;
			std::transform(l_sReceivedData.begin(), l_sReceivedData.end(), l_sReceivedData.begin(), std::ptr_fun<int, int>(std::toupper));
			if(l_sReceivedData == "GETMESSAGE\n")
			{
				a_sResponseMessage = "The message is ";
				a_sResponseMessage += GetRandomResponseString(MAX_RANDOM_STRING_LENGTH);
				return SendResponse;
			}
			else if(l_sReceivedData == "BYE\n")
			{
				return CloseConnection;
			}
			else
			{
				a_sResponseMessage = "Bad Request" ;
				a_sResponseMessage += "\n";
				return SendResponse;
			}
		}
		else
		{
			a_sResponseMessage = "Bad Request";
			a_sResponseMessage += "\n";
			return SendResponse;
		}
	}
	catch(...)
	{
		CFileHandler::GetInstance().WriteToFile(500,"Unknown Exception Raised in CMessagingServer::ProcessMessage:ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));	
		std::string l_sDescription;
		char l_cpDescription[2048]={0x00};
		sprintf(l_cpDescription,"Exception Raised in CMessagingServer::ProcessMessage:ErrorNumber:%d:ErrorString:%s",errno,strerror(errno));
		CException l_uException(l_cpDescription);
		throw l_uException;
	}

}
std::string CMessagingServer::GetRandomResponseString(int a_iRandomStringLength)
{
	std::string     l_sCharacterSet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ012345abcdefghijklmnopqrstuvwxyz67890";
	std::string 	l_sRandomString;
	time_t          l_uTime;
	int             l_iRandomStringLength = 0;
	int             l_iTemp = 0;
	int             l_iPosition = 0;
	int             l_iIndex = 0;
	char            l_cpTime[15] = {0x00};
	static long int l_lUniqueVal = 0;

	try
	{
		time(&l_uTime);
		sprintf(l_cpTime,"%ld",l_uTime);

		if((a_iRandomStringLength - strlen(l_cpTime))%2 == 1)
		{
			l_iRandomStringLength = (a_iRandomStringLength - strlen(l_cpTime))/2 + 1;
		}
		else
		{
			l_iRandomStringLength = (a_iRandomStringLength - strlen(l_cpTime))/2;
		}

		++l_lUniqueVal;
		if(l_lUniqueVal >2000000)
		{	
			l_lUniqueVal = 0;
		}
		srand(l_lUniqueVal);
		while(l_iIndex < l_iRandomStringLength) {
			l_iPosition = ((rand() % (l_sCharacterSet.size() - 1)));
			l_sRandomString += l_sCharacterSet[l_iPosition];
			++l_iIndex;
		}
		l_sRandomString += l_cpTime;
		l_iIndex = 0;
		l_iTemp = l_sRandomString.length();
		while(l_iIndex < a_iRandomStringLength - l_iTemp) {
			l_iPosition = ((rand() % (l_sCharacterSet.size() - 1)));
			l_sRandomString += l_sCharacterSet[l_iPosition];
			++l_iIndex;
		}
	}
	catch(...)
	{
		CFileHandler::GetInstance().WriteToFile(500,"Unknown Exception Raised in CMessagingServer::GetRandomResponseString:ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));	
		l_sRandomString = "";
	}
	return l_sRandomString;

}

