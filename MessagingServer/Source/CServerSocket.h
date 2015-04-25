#ifndef __CSERVERSOCKET_H__
#define __CSERVERSOCKET_H__
#include "CBasicSocket.h"
#include "CSocketFactory.h"
#include "CSynchronousSocketFactory.h"
#include <pthread.h>

class CServerSocket
{
private:
	static int		m_isourcePortNumber;
	static int		m_iserverConnectionQueueSize;
	static std::string	m_ssourceIP;
	static SocketProtocol	m_uprotocol;
	static SocketType	m_usocketType;
	static pthread_t	m_ulistenThreadId;
	static bool		m_bserverRunningStatus;
	CBasicSocket*	m_upserverSocketObject;		
public:
	CServerSocket();
	~CServerSocket();
	void SetPortNumber(int);
	int GetPortNumber();
	void SetSourceIP(std::string);
	std::string GetSourceIP();
	void SetServerRunningStatus(bool);
	static bool GetServerRunningStatus();
	void SetProtocol(SocketProtocol);
	SocketProtocol GetProtocol();
	void SetServerConnectionQueueSize(int);
	int GetServerConnectionQueueSize();
	SocketType GetSocketType();
	static void SetSocketType(SocketType a_usocketType);
	static void ForceDisconnect(int);

	//Server related methods
	int Accept();
	bool Listen();
	bool Bind();
	
	bool StartServer();
	bool MakeServerSocketInstance(bool ,int);
	bool MakeServerSocketInstance(bool );
	CBasicSocket* GetSocketInstance();
	void SetSocketInstance(CBasicSocket* a_upsocketObject);
	bool SendMessage(std::string, int&);
	bool ReceiveMessage(std::string&, int&);
	bool DisconnectClient();

	virtual void OnAccept(int);
	virtual void OnMessageReceive(std::string,int);
	virtual void OnDisconnect(int);

	static void* ThreadListenConnections(void*);
	static void* ThreadMessageReceiver(void*);
}; 
#endif
