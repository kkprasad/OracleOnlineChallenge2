/*******************************************************************************
File Name               : cbasicsocket.cpp
Class Name              : CBasicSocket
Description             : This file contains the methods which are common to
                          all types of sockets. CTCPSocket class is made by 
			  inheriting from this class
*******************************************************************************/

#include <cbasicsocket.h>


CBasicSocket::CBasicSocket()
{
	this->m_iSocketFD = -1;
        this->m_bConnectionStatus = false;
        this->m_bSocketCreatedStatus = false;
}
CBasicSocket::~CBasicSocket()
{
	if(this->m_bConnectionStatus)
	{
		close(this->m_iSocketFD);
	        this->Shutdown();
	}
	this->m_bConnectionStatus = false;
	this->m_bSocketCreatedStatus = false;
	
}

bool CBasicSocket::Create()
{

}
void  CBasicSocket::ForceClose(int a_iSocketFD)
{
	try
	{
		close(a_iSocketFD);
	}
	catch(...)
	{
		CFileHandler::GetInstance().WriteToFile(700,"Unknown Exception Raised in CBasicSocket::ForceClose:ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));
		CException l_uException("Exception raised in CBasicSocket::ForceClose");
		throw l_uException;
	}
}
void CBasicSocket::ForceShutdown(int a_iSocketFD)
{
	shutdown(a_iSocketFD,SHUT_RDWR);
}
bool CBasicSocket::Close()
{
	try
	{
		close(this->m_iSocketFD);
		Shutdown();
		this->m_bConnectionStatus = false;
		this->m_bSocketCreatedStatus = false;
	}
	catch(...)
	{
		CFileHandler::GetInstance().WriteToFile(700,"Unknown Exception Raised in CBasicSocket::Close:ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));
		CException l_uException("Exception raised in CBasicSocket::Close");
		throw l_uException;
	}
}
bool CBasicSocket::Shutdown()
{
	try
	{
	shutdown(this->m_iSocketFD,SHUT_RDWR);
	}
	catch(...)
	{
		CFileHandler::GetInstance().WriteToFile(700,"Unknown Exception Raised in CBasicSocket::Shutdown:ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));
		CException l_uException("Exception raised in CBasicSocket::Shutdown");
		throw l_uException;
	}
}
bool CBasicSocket::Listen( int a_iQueueLength)
{
	try
	{
		int l_iListenStatus = listen(this->m_iSocketFD, a_iQueueLength);

		if(l_iListenStatus != 0) 
			return false;
		else
			return true;
	}
	catch(...)
	{
		CFileHandler::GetInstance().WriteToFile(700,"Unknown Exception Raised in CBasicSocket::Listen:ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));
		CException l_uException("Exception raised in CBasicSocket::Listen");
		throw l_uException;
	}
}
int CBasicSocket::Accept()
{
	try
	{
		int l_iNewConnection = accept(this->m_iSocketFD, (struct sockaddr*)NULL, NULL);
		return l_iNewConnection;
	}
	catch(...)
	{
		CFileHandler::GetInstance().WriteToFile(700,"Unknown Exception Raised in CBasicSocket::Accept:ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));
		CException l_uException("Exception raised in CBasicSocket::Accept");
		throw l_uException;
	}
}
void CBasicSocket::SetSocketFD(int a_iSocketFD)
{
	this->m_iSocketFD = a_iSocketFD;
}
int CBasicSocket::GetSocketFD()
{
	return this->m_iSocketFD;
}
bool CBasicSocket::GetConnectionStatus()
{
	return this->m_bConnectionStatus;
}
bool CBasicSocket::GetSocketCreatedStatus()
{
	return this->m_bSocketCreatedStatus;
}

bool CBasicSocket::Bind(std::string a_sSourceIP, int a_iPortNumber)
{

}
bool CBasicSocket::Send(std::string a_sData, int& a_iMessageLength)
{

}
bool CBasicSocket::Receive(std::string &a_sData, int& a_iMessageLength)
{

}

bool CBasicSocket::Connect(std::string a_sDestinationIP, int a_iDestinationPortNumber)
{

}
void CBasicSocket::SetConnectionStatus(bool a_bConnectionStatus)
{
	this->m_bConnectionStatus = a_bConnectionStatus;
}
void CBasicSocket::SetSocketCreatedStatus(bool a_bSocketCreatedStatus)
{
	this->m_bSocketCreatedStatus = a_bSocketCreatedStatus;
}

