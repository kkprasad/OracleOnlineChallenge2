#ifndef __CMESSAGINGSERVER_H__
#define __CMESSAGINGSERVER_H__
#include "CServerSocket.h"
#include <queue>
#include <map>
#include <string>
#include <pthread.h>

struct MessageData
{
	int m_iSocketFD;
	std::string m_sData;
};
enum MessageProcessorAction
{
	SendResponse = 1,
	CloseConnection = 2
};
class CMessagingServer:public CServerSocket
{
private:
	bool					m_bMessagingServerActiveStatus;
	static int				m_iMaxMessageHandlerThreads;
	static int				m_iMaxMessageSenderThreads;
	static std::queue<MessageData*>		m_qReceivedMessageQueue;
	static std::queue<MessageData*>		m_qSendMessageQueue;
	static std::queue<int>			m_qObseleteMessageHandlerQueue;
	static std::map<int,CMessagingServer*> 	m_mActiveMessagingServerList;
	static pthread_t			*m_upMessageHandlerthreadId;
	static pthread_t			*m_upMessageSenderthreadId;
	static pthread_t			m_uMessageReceiverThreadId;
	static pthread_t			m_uObseleteMessageHandlerthreadId;
	static pthread_mutex_t			m_uMessageHandlerMutex;
	static pthread_mutex_t			m_uSendMessageMutex;
	static pthread_mutex_t			m_uActiveServerListMutex;

public:
	CMessagingServer();
	~CMessagingServer();
	void SetMaxMessageHandlerThreads(int);
	int GetMaxMessageHandlerThreads();
	void SetMaxMessageSenderThreads(int);
	int GetMaxMessageSenderThreads();
	bool GetMessagingServerActiveStatus();
	void SetMessagingServerActiveStatus(bool);

	virtual void OnMessageReceive(std::string,int);
	virtual	void OnAccept(int);
	virtual void OnDisconnect(int);

	static void* ThreadMessageHandler(void*);
	static void* ThreadMessageSender(void*);
	static void* ThreadDeleteObseleteMessageHandlers(void*);

	static MessageProcessorAction ProcessMessage(std::string,std::string&);
	static std::string GetRandomResponseString(int);
	bool StartThreads();
	bool StopThreads();
}; 
#endif
