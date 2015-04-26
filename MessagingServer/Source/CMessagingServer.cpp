#include "CMessagingServer.h"
#include <algorithm>

std::queue<int> CMessagingServer::m_qobseleteMessageHandlerQueue;
std::queue<MessageData*> CMessagingServer::m_qreceivedMessageQueue;
std::queue<MessageData*> CMessagingServer::m_qsendMessageQueue;
std::map<int,CMessagingServer*> CMessagingServer::m_mactiveMessagingServerList;
pthread_t  CMessagingServer::m_umessageHandlerthreadId;
pthread_t CMessagingServer::m_umessageSenderthreadId;
pthread_t CMessagingServer::m_umessageReceiverThreadId;
pthread_mutex_t CMessagingServer::m_umessageHandlerMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t CMessagingServer::m_usendMessageMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t CMessagingServer::m_uactiveServerListMutex = PTHREAD_MUTEX_INITIALIZER;
int CMessagingServer::m_imaxMessageSenderThreads = 0;
int CMessagingServer::m_imaxMessageHandlerThreads = 0;


#define MAX_RANDOM_STRING_LENGTH 50

CMessagingServer::CMessagingServer()
{
}
CMessagingServer::~CMessagingServer()
{
}

void CMessagingServer::SetMaxMessageSenderThreads(int a_imaxMessageSendThreads)
{
	CMessagingServer::m_imaxMessageSenderThreads = a_imaxMessageSendThreads;
}
int CMessagingServer::GetMaxMessageSenderThreads()
{
	return CMessagingServer::m_imaxMessageSenderThreads;
}

void CMessagingServer::SetMaxMessageHandlerThreads(int a_imaxMessageHandlerThreads)
{
	CMessagingServer::m_imaxMessageHandlerThreads = a_imaxMessageHandlerThreads;
}
int CMessagingServer::GetMaxMessageHandlerThreads()
{
	return CMessagingServer::m_imaxMessageHandlerThreads;
}
bool CMessagingServer::GetMessagingServerActiveStatus()
{
	return this->m_bmessagingServerActiveStatus;
}

void CMessagingServer::SetMessagingServerActiveStatus(bool a_bmessagingServerActiveStatus)
{
	this->m_bmessagingServerActiveStatus = a_bmessagingServerActiveStatus;
}
void CMessagingServer::OnMessageReceive(std::string a_sreceivedMessage,int a_ireceivedMessageLength)
{
	std::cout<<"Received Message:"<<a_sreceivedMessage<<std::endl;
	if(a_ireceivedMessageLength > 0)
	{
		MessageData *l_upmessageData = NULL;
		l_upmessageData = new MessageData;
		l_upmessageData->m_isocketFD = this->GetSocketInstance()->GetSocketFD();
		l_upmessageData->m_sdata = a_sreceivedMessage;
		pthread_mutex_lock(&CMessagingServer::m_umessageHandlerMutex);
		CMessagingServer::m_qreceivedMessageQueue.push(l_upmessageData);
		pthread_mutex_unlock(&CMessagingServer::m_umessageHandlerMutex);
	}
}
void CMessagingServer::OnAccept(int a_isocketFD)
{
	std::cout<<"Received New Connection.."<<std::endl;
	CMessagingServer* l_upmessagingServer = NULL;
	try
	{
		l_upmessagingServer = new CMessagingServer;
		if(l_upmessagingServer == NULL)
			throw;
		if ( !l_upmessagingServer->MakeServerSocketInstance(false,a_isocketFD) )
		{
			throw;
		}

	}
	catch(...)
	{
		if(l_upmessagingServer != NULL)
		{
			delete l_upmessagingServer;
		}
		this->ForceDisconnect(a_isocketFD);
		std::cout<<"Error in creating new instance to handle the new socket connection.."<<std::endl;
		return;	
	}
	l_upmessagingServer->SetMessagingServerActiveStatus(true);
	l_upmessagingServer->GetSocketInstance()->SetConnectionStatus(true);
	pthread_mutex_lock(&CMessagingServer::m_uactiveServerListMutex);
	CMessagingServer::m_mactiveMessagingServerList[a_isocketFD] = l_upmessagingServer;
	pthread_mutex_unlock(&CMessagingServer::m_uactiveServerListMutex);

	int l_ithreadAttrInitStatus;
	int l_isetDetachStateStatus;
	int l_ithreadCreationStatus;
	pthread_attr_t l_uthreadAttribute;
	try
	{
		l_ithreadAttrInitStatus = pthread_attr_init(&l_uthreadAttribute);
                if(l_ithreadAttrInitStatus != 0 )
                {
                        std::cout<<"Error in Initializing Thread Attributes in OnAccept"<<std::endl;
                        throw;
                }
                l_isetDetachStateStatus = pthread_attr_setdetachstate(&l_uthreadAttribute, PTHREAD_CREATE_DETACHED);
                if(l_isetDetachStateStatus != 0 )
                {
                        std::cout<<"Error in Setting Detach state in OnAccept"<<std::endl;
                        throw;
                }
                l_ithreadCreationStatus = pthread_create(&l_upmessagingServer->m_umessageReceiverThreadId, &l_uthreadAttribute,CMessagingServer::ThreadMessageReceiver , (void*)l_upmessagingServer);
                if(l_ithreadCreationStatus != 0 )
                {
                        std::cout<<"Error in creating thread ThreadMessageReceiver"<<std::endl;
                        throw;
                }
                pthread_attr_destroy(&l_uthreadAttribute);
	}
        catch(...)
        {
		pthread_mutex_lock(&CMessagingServer::m_uactiveServerListMutex);
		CMessagingServer::m_mactiveMessagingServerList.erase(a_isocketFD);
		pthread_mutex_unlock(&CMessagingServer::m_uactiveServerListMutex);
		if(l_upmessagingServer !=NULL)
		{
			delete l_upmessagingServer;
		}
		this->ForceDisconnect(a_isocketFD);
		if(l_ithreadAttrInitStatus == 0)
		{
			pthread_attr_destroy(&l_uthreadAttribute);
		}
	
                std::cout<<"Error in creating ThreadMessageReceiver"<<std::endl;
        }
	
}
void CMessagingServer::OnDisconnect(int a_isocketFD)
{
	//appy mutex
//	this->GetSocketInstance()->SetConnectionStatus(false);
	CMessagingServer::m_qobseleteMessageHandlerQueue.push(a_isocketFD);
}

void* CMessagingServer::ThreadMessageHandler(void* a_uparguments)
{
	CMessagingServer *l_upinvokedObject = (CMessagingServer*)a_uparguments;
	
	while(CMessagingServer::GetServerRunningStatus())
	{
		MessageData	*l_upmessageData = NULL;
		MessageData	*l_upmessageDataToSend = NULL;
		pthread_mutex_lock(&CMessagingServer::m_umessageHandlerMutex);
		if(CMessagingServer::m_qreceivedMessageQueue.size()>0)
		{
			l_upmessageData = CMessagingServer::m_qreceivedMessageQueue.front();
			CMessagingServer::m_qreceivedMessageQueue.pop();
			pthread_mutex_unlock(&CMessagingServer::m_umessageHandlerMutex);

			if(l_upmessageData != NULL)
			{
				MessageProcessorAction l_umessageProcessorAction;
				std::string l_sresponseMessage;
				l_umessageProcessorAction = CMessagingServer::ProcessMessage(l_upmessageData->m_sdata, l_sresponseMessage);				
				switch(l_umessageProcessorAction)
				{
					case SendResponse:
						l_upmessageDataToSend = new MessageData;
						l_upmessageDataToSend->m_isocketFD = l_upmessageData->m_isocketFD;
						l_upmessageDataToSend->m_sdata = l_sresponseMessage; 
						pthread_mutex_lock(&CMessagingServer::m_usendMessageMutex);
						CMessagingServer::m_qsendMessageQueue.push(l_upmessageDataToSend);
						pthread_mutex_unlock(&CMessagingServer::m_usendMessageMutex);
						break;
					case CloseConnection:
						/*
						if(CMessagingServer::m_mactiveMessagingServerList.find(l_upmessageData->m_isocketFD) != CMessagingServer::m_mactiveMessagingServerList.end())
						{
							CMessagingServer *l_upcurrentObject = NULL;
							l_upcurrentObject = CMessagingServer::m_mactiveMessagingServerList[l_upmessageData->m_isocketFD];
							if(l_upcurrentObject != NULL )
							{
								l_upcurrentObject->GetSocketInstance()->SetConnectionStatus(false);
								l_upcurrentObject->DisconnectClient();	
								sleep(1);
								delete l_upcurrentObject;		
							}
							CMessagingServer::m_mactiveMessagingServerList.erase(l_upmessageData->m_isocketFD);
						}
						*/
						CMessagingServer::m_qobseleteMessageHandlerQueue.push(l_upmessageData->m_isocketFD);
						
						break;
					default:
						break;
				};
				delete l_upmessageData;
			}
		}
		else
		{
			pthread_mutex_unlock(&CMessagingServer::m_umessageHandlerMutex);
		}
		usleep(1000);
	}
}
void* CMessagingServer::ThreadMessageSender(void* a_uparguments)
{
	CMessagingServer *l_upinvokedObject = (CMessagingServer*)a_uparguments;
	CMessagingServer *l_upcurrentObject = NULL;
	while(CMessagingServer::GetServerRunningStatus())
	{
		MessageData	*l_upmessageData = NULL;
		pthread_mutex_lock(&CMessagingServer::m_usendMessageMutex);
		if(CMessagingServer::m_qsendMessageQueue.size()>0)
		{
			l_upmessageData = CMessagingServer::m_qsendMessageQueue.front();
			CMessagingServer::m_qsendMessageQueue.pop();
			pthread_mutex_unlock(&CMessagingServer::m_usendMessageMutex);
			if(l_upmessageData != NULL)
			{
				l_upcurrentObject = NULL;
		
				pthread_mutex_lock(&CMessagingServer::m_uactiveServerListMutex);
				if(CMessagingServer::m_mactiveMessagingServerList.find(l_upmessageData->m_isocketFD) != CMessagingServer::m_mactiveMessagingServerList.end())
				{
					l_upcurrentObject = CMessagingServer::m_mactiveMessagingServerList[l_upmessageData->m_isocketFD];
					pthread_mutex_unlock(&CMessagingServer::m_uactiveServerListMutex);
					if(l_upcurrentObject != NULL )
					{
						if( l_upcurrentObject->GetMessagingServerActiveStatus() )
						{
							int l_isentMessageLength = 0;
							if(!l_upcurrentObject->SendMessage(l_upmessageData->m_sdata, l_isentMessageLength))
							{
								std::cout<<"Error in sending message to socketFD:"<<l_upcurrentObject->GetSocketInstance()->GetSocketFD()<<std::endl;
							}
							std::cout<<"Sent Response Message to Socket FD:"<<l_upcurrentObject->GetSocketInstance()->GetSocketFD()<<std::endl;
						}
					}
				}
				else
				{
					pthread_mutex_unlock(&CMessagingServer::m_uactiveServerListMutex);
				}
				delete l_upmessageData;
			}
		}
		else
		{
			pthread_mutex_unlock(&CMessagingServer::m_usendMessageMutex);
		}
		usleep(1000);
	}
	std::cout<<"Exiting from ThreadMessageSender"<<std::endl;
}
void* CMessagingServer::ThreadDeleteObseleteMessageHandlers(void* a_uparguments)
{
	CMessagingServer *l_upcurrentObject = (CMessagingServer*)a_uparguments;
	
	while(CMessagingServer::GetServerRunningStatus())
	{
		CMessagingServer* l_upobseleteObject = NULL;
		int l_isocketFD = 0;
		if(CMessagingServer::m_qobseleteMessageHandlerQueue.size()>0)
		{
			l_isocketFD = CMessagingServer::m_qobseleteMessageHandlerQueue.front();
			CMessagingServer::m_qobseleteMessageHandlerQueue.pop();
			
			pthread_mutex_lock(&CMessagingServer::m_uactiveServerListMutex);
			if(CMessagingServer::m_mactiveMessagingServerList.find(l_isocketFD) != CMessagingServer::m_mactiveMessagingServerList.end())
			{
				l_upobseleteObject = CMessagingServer::m_mactiveMessagingServerList[l_isocketFD];
				CMessagingServer::m_mactiveMessagingServerList.erase(l_isocketFD);
				pthread_mutex_unlock(&CMessagingServer::m_uactiveServerListMutex);
				std::cout<<"Removed object from m_mactiveMessagingServerList"<<std::endl;
				if(l_upobseleteObject != NULL)
				{
					l_upobseleteObject->GetSocketInstance()->SetConnectionStatus(false);
					l_upobseleteObject->SetMessagingServerActiveStatus(false);
					sleep(1);	
					l_upobseleteObject->DisconnectClient();
					delete l_upobseleteObject;
				}
			}
			else
			{
				pthread_mutex_unlock(&CMessagingServer::m_uactiveServerListMutex);
			}
		}
		usleep(1000);
	}
}

bool CMessagingServer::StartThreads()
{
	int l_ithreadAttrInitStatus;
	int l_isetDetachStateStatus;
	int l_ithreadCreationStatus;
	pthread_attr_t l_uthreadAttribute;
	pthread_t l_uthreadId;
	try
	{
		l_ithreadAttrInitStatus = pthread_attr_init(&l_uthreadAttribute);
                if(l_ithreadAttrInitStatus != 0 )
                {
                        std::cout<<"Error in Initializing Thread Attributes in OnAccept"<<std::endl;
                        throw;
                }
                l_isetDetachStateStatus = pthread_attr_setdetachstate(&l_uthreadAttribute, PTHREAD_CREATE_DETACHED);
                if(l_isetDetachStateStatus != 0 )
                {
                        std::cout<<"Error in Setting Detach state in OnAccept"<<std::endl;
                        throw;
                }
		for(int l_iindex = 0;l_iindex<CMessagingServer::m_imaxMessageSenderThreads;l_iindex++)
		{
			std::cout<<"Starting ThreadMessageSender Number:"<<l_iindex+1<<std::endl;
			//l_ithreadCreationStatus = pthread_create(&CMessagingServer::m_umessageSenderthreadId, &l_uthreadAttribute,ThreadMessageSender , (void*)this);
			l_ithreadCreationStatus = pthread_create(&l_uthreadId, &l_uthreadAttribute,ThreadMessageSender , (void*)this);
			if(l_ithreadCreationStatus != 0 )
			{
				std::cout<<"Error in creating thread ThreadMessageSender"<<std::endl;
				throw;
			}
		}
		for(int l_iindex = 0;l_iindex<CMessagingServer::m_imaxMessageHandlerThreads;l_iindex++)
		{
			std::cout<<"Starting ThreadMessageHandler Number:"<<l_iindex+1<<std::endl;
			//l_ithreadCreationStatus = pthread_create(&CMessagingServer::m_umessageHandlerthreadId, &l_uthreadAttribute,ThreadMessageHandler , (void*)this);
			l_ithreadCreationStatus = pthread_create(&l_uthreadId, &l_uthreadAttribute,ThreadMessageHandler , (void*)this);
			if(l_ithreadCreationStatus != 0 )
			{
				std::cout<<"Error in creating thread ThreadMessageSender"<<std::endl;
				throw;
			}
		}
		l_ithreadCreationStatus = pthread_create(&CMessagingServer::m_umessageHandlerthreadId, &l_uthreadAttribute,ThreadDeleteObseleteMessageHandlers , (void*)this);
                if(l_ithreadCreationStatus != 0 )
                {
                        std::cout<<"Error in creating thread ThreadDeleteObseleteMessageHandlers"<<std::endl;
                        throw;
                }
                pthread_attr_destroy(&l_uthreadAttribute);
		return true;
	}
        catch(...)
        {
		if(l_ithreadAttrInitStatus == 0)
		{
			pthread_attr_destroy(&l_uthreadAttribute);
		}
	
                std::cout<<"Error in StartThreads"<<std::endl;
		return false;
        }
}
MessageProcessorAction CMessagingServer::ProcessMessage(std::string a_sreceivedData, std::string& a_responseMessage)
{
	
	if(a_sreceivedData.length() > 0)
	{
		std::string l_sreceivedData;
		l_sreceivedData = a_sreceivedData;
		std::transform(l_sreceivedData.begin(), l_sreceivedData.end(), l_sreceivedData.begin(), std::ptr_fun<int, int>(std::toupper));
		if(l_sreceivedData == "GETMESSAGE\n")
		{
			a_responseMessage = "The message is ";
			a_responseMessage += GetRandomResponseString(MAX_RANDOM_STRING_LENGTH);
			return SendResponse;
		}
		else if(l_sreceivedData == "BYE\n")
		{
			return CloseConnection;
		}
		else
		{
			a_responseMessage = "Bad Request" ;
			a_responseMessage += "\n";
			return SendResponse;
		}
	}
	else
	{
		a_responseMessage = "Bad Request";
		a_responseMessage += "\n";
		return SendResponse;
	}
}
std::string CMessagingServer::GetRandomResponseString(int a_irandomStringLength)
{

	srand(time(0));
	std::string l_scharacterSet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ012345abcdefghijklmnopqrstuvwxyz67890";
	int pos;
	while(l_scharacterSet.size() != a_irandomStringLength) {
		pos = ((rand() % (l_scharacterSet.size() - 1)));
		l_scharacterSet.erase (pos, 1);
	}
	std::cout<<std::endl<<l_scharacterSet<<std::endl;
	return l_scharacterSet;
}

