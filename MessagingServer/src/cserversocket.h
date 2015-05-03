#ifndef __CSERVERSOCKET_H__
#define __CSERVERSOCKET_H__
#include <cbasicsocket.h>
#include <csocketfactory.h>
#include <csynchronoussocketfactory.h>
#include <pthread.h>

class CServerSocket
{
private:
	static int		m_iSourcePortNumber;
	static int		m_iServerConnectionQueueSize;
	static std::string	m_sSourceIP;
	static SocketProtocol	m_uProtocol;
	static SocketType	m_uSocketType;
	static pthread_t	m_uListenThreadId;
	static bool		m_bServerRunningStatus;
	CBasicSocket*	m_upServerSocketObject;		
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
	bool StopServer();
	bool MakeServerSocketInstance(bool ,int);
	bool MakeServerSocketInstance(bool );
	CBasicSocket* GetSocketInstance();
	void SetSocketInstance(CBasicSocket* a_upsocketObject);
	void UnSetSocketInstance();
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
