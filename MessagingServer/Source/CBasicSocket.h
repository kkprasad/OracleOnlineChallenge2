#ifndef __CBASICSOCKET_H__
#define __CBASICSOCKET_H__
#include<iostream>
#include<sys/socket.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include "string.h"
#include "errno.h"
#include "Definitions.h"

#define MAX_BUFFER_SIZE 1025

class CBasicSocket
{
private:
	int 		m_isocketFD;
	bool 		m_bconnectionStatus;
	bool		m_bsocketCreatedStatus;
public:

	CBasicSocket();
	~CBasicSocket();
	virtual bool Create();
	virtual bool Bind(std::string,int);
	virtual bool Send(std::string , int&);
	virtual bool Receive(std::string &, int &);
	virtual bool Connect();

	bool Listen(int);
	bool Close();
	bool Shutdown();
	int Accept();

	static void ForceClose(int a_isocketFD);
	static void ForceShutdown(int a_isocketFD);

	void SetSocketFD(int);
	int GetSocketFD();
	void SetConnectionStatus(bool);
	bool GetConnectionStatus();
	void SetSocketCreatedStatus(bool);
	bool GetSocketCreatedStatus();
};

#endif
