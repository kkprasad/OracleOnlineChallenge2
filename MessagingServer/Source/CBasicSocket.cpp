#include "CBasicSocket.h"

CBasicSocket::CBasicSocket()
{
	this->m_isocketFD = -1;
        this->m_bconnectionStatus = false;
        this->m_bsocketCreatedStatus = false;
}
CBasicSocket::~CBasicSocket()
{
	if(this->m_bconnectionStatus)
	{
		close(this->m_isocketFD);
	        this->Shutdown();
	}
	this->m_bconnectionStatus = false;
	this->m_bsocketCreatedStatus = false;
	
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
	close(this->m_isocketFD);
	Shutdown();
	this->m_bconnectionStatus = false;
	this->m_bsocketCreatedStatus = false;
}
bool CBasicSocket::Shutdown()
{
	shutdown(this->m_isocketFD,SHUT_RDWR);
}
bool CBasicSocket::Listen( int a_iqueueLength)
{
	int l_ilistenStatus = listen(this->m_isocketFD, a_iqueueLength);
	
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
	this->m_isocketFD = a_isocketFD;
}
int CBasicSocket::GetSocketFD()
{
	return this->m_isocketFD;
}
bool CBasicSocket::GetConnectionStatus()
{
	return this->m_bconnectionStatus;
}
bool CBasicSocket::GetSocketCreatedStatus()
{
	return this->m_bsocketCreatedStatus;
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
	this->m_bconnectionStatus = a_bconnectionStatus;
}
void CBasicSocket::SetSocketCreatedStatus(bool a_bsocketCreatedStatus)
{
	this->m_bsocketCreatedStatus = a_bsocketCreatedStatus;
}

