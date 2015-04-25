#include "CBasicSocket.h"

CBasicSocket::CBasicSocket()
{
	m_isocketFD = -1;
        m_bconnectionStatus = false;
        m_bsocketCreatedStatus = false;
}
CBasicSocket::~CBasicSocket()
{
	if(m_bconnectionStatus)
	{
		close(m_isocketFD);
	        Shutdown();
	}
	m_bconnectionStatus = false;
	m_bsocketCreatedStatus = false;
	
}

bool CBasicSocket::Create()
{

}
void  CBasicSocket::ForceClose(int a_isocketFD)
{
	close(a_isocketFD);
}
void CBasicSocket::ForceShutdown(int a_isocketFD)
{
	shutdown(a_isocketFD,SHUT_RDWR);
}
bool CBasicSocket::Close()
{
	close(m_isocketFD);
	Shutdown();
	m_bconnectionStatus = false;
	m_bsocketCreatedStatus = false;
}
bool CBasicSocket::Shutdown()
{
	shutdown(m_isocketFD,SHUT_RDWR);
}
bool CBasicSocket::Listen( int a_iqueueLength)
{
	int l_ilistenStatus = listen(m_isocketFD, a_iqueueLength);
	
	if(l_ilistenStatus != 0) 
		return false;
	else
		return true;
}
int CBasicSocket::Accept()
{
	int l_inewConnection = accept(this->m_isocketFD, (struct sockaddr*)NULL, NULL);
	return l_inewConnection;
}
void CBasicSocket::SetSocketFD(int a_isocketFD)
{
	m_isocketFD = a_isocketFD;
}
int CBasicSocket::GetSocketFD()
{
	return m_isocketFD;
}
bool CBasicSocket::GetConnectionStatus()
{
	return m_bconnectionStatus;
}
bool CBasicSocket::GetSocketCreatedStatus()
{
	return m_bsocketCreatedStatus;
}

bool CBasicSocket::Bind(std::string a_ssourceIP, int a_iportNumber)
{

}
bool CBasicSocket::Send(std::string a_sdata, int& a_imessageLength)
{

}
bool CBasicSocket::Receive(std::string &a_sdata, int& a_imessageLength)
{

}

bool CBasicSocket::Connect()
{

}
void CBasicSocket::SetConnectionStatus(bool a_bconnectionStatus)
{
	m_bconnectionStatus = a_bconnectionStatus;
}
void CBasicSocket::SetSocketCreatedStatus(bool a_bsocketCreatedStatus)
{
	m_bsocketCreatedStatus = a_bsocketCreatedStatus;
}

