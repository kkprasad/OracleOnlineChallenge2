#ifndef __CMESSAGINGSERVER_H__
#define __CMESSAGINGSERVER_H__
#include "CServerSocket.h"
#include <queue>
#include <map>
#include <string>
#include <pthread.h>

struct MessageData
{
	int m_isocketFD;
	std::string m_sdata;
};
enum MessageProcessorAction
{
	SendResponse = 1,
	CloseConnection = 2
};
class CMessagingServer:public CServerSocket
{
private:
	int					m_imaxMessageHandlerThreads;
	bool					m_bmessagingServerActiveStatus;
	static std::queue<MessageData*>		m_qreceivedMessageQueue;
	static std::queue<MessageData*>		m_qsendMessageQueue;
	static std::queue<int>			m_qobseleteMessageHandlerQueue;
	static std::map<int,CMessagingServer*> 	m_mactiveMessagingServerList;
	static pthread_t			m_umessageHandlerthreadId;
	static pthread_t			m_umessageSenderthreadId;
	static pthread_t			m_umessageReceiverThreadId;

public:
	CMessagingServer();
	~CMessagingServer();
	void SetMaxMessageHandlerThreads(int);
	int GetMaxMessageHandlerThreads();
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
}; 
#endif
