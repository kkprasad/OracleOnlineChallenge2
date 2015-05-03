#include "CTCPSocket.h"

/*******************************************************************************
File Name               : CTCPSocket.cpp
Class Name              : CTCPSocket
Description             : This file contains the methods for TCP socket 
			  operations
*******************************************************************************/

bool CTCPSocket::Create()
{
/*******************************************************************************
Function Name           : Create()
Arguments Description   :
Return Value            : Status of the socket creation
Function Description    : This method is to create the socket
*******************************************************************************/
	try
	{
	int l_iSocketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(l_iSocketFD==-1) 
	{
		CException l_uException("Error in Socket Creation");	
		throw l_uException;
	}
	this->SetSocketFD( l_iSocketFD );
	this->SetSocketCreatedStatus(true);
	}
	catch (CException a_uException)
        {
                CFileHandler::GetInstance().WriteToFile(300,"Exception Raised in CTCPSocket::Create:%s",a_uException.what());
		throw a_uException;
        }
        catch (...)
        {
                CFileHandler::GetInstance().WriteToFile(300,"Unknown Exception Raised in CTCPSocket::Create:ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));
		char l_cpDescription[1024] = {0x00};
		sprintf(l_cpDescription,"Unknown Exception Raised in CTCPSocket::Create:ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));
		CException l_uException(l_cpDescription);	
		throw l_uException;
        }

	return true;
}

bool CTCPSocket::Bind(std::string a_sSourceIP,int a_iSourcePortNumber)
{
/*******************************************************************************
Function Name           : Bind
Arguments Description   : Source IP and Port number to be listen
Return Value            : Status of bind operation
Function Description    : This method is to bind to the source IP and port 
			  number. Do retry 10 times if the bind operation is 
			  failed
*******************************************************************************/
	int l_iSocketOption = 1; 
	try
	{
	if (setsockopt(this->GetSocketFD(),SOL_SOCKET,SO_REUSEADDR,(char *)&l_iSocketOption,sizeof(l_iSocketOption)) == -1)
	{
		perror("setsockopt");
		return false;
	}
	struct sockaddr_in l_uSockAddr;

	l_uSockAddr.sin_port=htons(a_iSourcePortNumber);
	l_uSockAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	l_uSockAddr.sin_family = AF_INET;
	//l_uSockAddr.sin_addr.s_addr=inet_addr(a_sSourceIP.c_str());

	int l_iBindRetryCount=0;
	while(bind(this->GetSocketFD(),(sockaddr *)&(l_uSockAddr),sizeof(l_uSockAddr)) == -1)
	{
		if(++l_iBindRetryCount >10)
		{
			CFileHandler::GetInstance().WriteToFile(400,"Error in Binding after 10 retries");
			return false;
		}
		std::cout<<" Bind Fail Retrying ..."<<l_iBindRetryCount<<std::endl;
		perror("");
		sleep(1);
	}
	
	CFileHandler::GetInstance().WriteToFile(400,"Bind Success ... 0n m_iSocketFD = %d\n",this->GetSocketFD());
	}
	catch (CException a_uException)
        {
                CFileHandler::GetInstance().WriteToFile(300,"Exception Raised in CTCPSocket::Bind:%s",a_uException.what());
		throw a_uException;
        }
        catch (...)
        {
                CFileHandler::GetInstance().WriteToFile(300,"Unknown Exception Raised in CTCPSocket::Bind:ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));
		char l_cpDescription[1024]= {0x00};
		sprintf(l_cpDescription,"Unknown Exception Raised in CTCPSocket::Bind:ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));
		CException l_uException(l_cpDescription);
		throw l_uException;
        }
	return true;
}
bool CTCPSocket::Send(std::string a_sData , int& a_iMessageLength)
{
/*******************************************************************************
Function Name           : Send
Arguments Description   : Data to be send and reference to the length of the
			  data that has sent
Return Value            : Status of send operation
Function Description    : This method is to send packet to client
*******************************************************************************/
	try
	{
		a_iMessageLength = send(this->GetSocketFD(), a_sData.c_str(), a_sData.length(), MSG_NOSIGNAL);
        }
	catch (...)
        {
                CFileHandler::GetInstance().WriteToFile(300,"Unknown Exception Raised in CTCPSocket::Send:ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));
		char l_cpDescription[1024]= {0x00};
		sprintf(l_cpDescription,"Unknown Exception Raised in CTCPSocket::Send:ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));
		CException l_uException(l_cpDescription);
		throw l_uException;
        }
	return true;	
}

bool CTCPSocket::Receive(std::string& a_sReceivedData, int& a_iReceivedMessageLength)
{
/*******************************************************************************
Function Name           : Receive
Arguments Description   : Reference to Data received and reference to the length
			 of the data that has received
Return Value            : Status of receive operation
Function Description    : This method is to receive packet from client
*******************************************************************************/
	char l_cpBuffer[MAX_BUFFER_SIZE]={0x00};
	bool l_bReceiveStatus = false;
	try
	{
		a_iReceivedMessageLength = recv(this->GetSocketFD(), l_cpBuffer, MAX_BUFFER_SIZE - 1,MSG_NOSIGNAL);
		if(a_iReceivedMessageLength == 0)
		{
			l_bReceiveStatus = false;
		}
		else if(a_iReceivedMessageLength == -1)
		{
			a_sReceivedData = "";
			l_bReceiveStatus = false;
		}
		else if(a_iReceivedMessageLength >0)
		{	
			a_sReceivedData = l_cpBuffer;
			l_bReceiveStatus = true;
		}
	}
	catch (...)
	{
		CFileHandler::GetInstance().WriteToFile(300,"Unknown Exception Raised in CTCPSocket::Receive:ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));
		char l_cpDescription[1024]= {0x00};
		sprintf(l_cpDescription,"Unknown Exception Raised in CTCPSocket::Receive:ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));
		CException l_uException(l_cpDescription);
		throw l_uException;
        }
	return l_bReceiveStatus;
}
bool CTCPSocket::Connect(std::string a_sDestinationIP, int a_iDestinationPortNumber)
{
/*******************************************************************************
Function Name           : Connect
Arguments Description   : Destination IP and port Number to be connected
Return Value            : Status of connect operation
Function Description    : This method is for the client activities.  It is 
			  used to connect to the server socket
*******************************************************************************/
	struct sockaddr_in l_uSockAddr;
	try
	{
		l_uSockAddr.sin_port=htons(a_iDestinationPortNumber);
		l_uSockAddr.sin_addr.s_addr=inet_addr(a_sDestinationIP.c_str());
		int l_iConnectStatus = connect(this->GetSocketFD(), (struct sockaddr*)&l_uSockAddr, sizeof(l_uSockAddr));
		if(l_iConnectStatus == -1) 
		{
			this->SetConnectionStatus(false);
			return false;
		}
		else
		{
			this->SetConnectionStatus(true);
			return true;
		}
	}
	catch (...)
	{
		CFileHandler::GetInstance().WriteToFile(300,"Unknown Exception Raised in CTCPSocket::Connect:ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));
		char l_cpDescription[1024]= {0x00};
		sprintf(l_cpDescription,"Unknown Exception Raised in CTCPSocket::Connect:ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));
		CException l_uException(l_cpDescription);
		throw l_uException;
	}
}
CTCPSocket::CTCPSocket()
{
}
CTCPSocket::~CTCPSocket()
{
}



