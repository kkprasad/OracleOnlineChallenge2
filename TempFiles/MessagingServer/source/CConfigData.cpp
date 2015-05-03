#include "CConfigData.h"
/*******************************************************************************
File Name               : CConfigData.cpp
Class Name              : CConfigData
Description             : This file contains the methods to load mad manage
			  the configuration file parameters
*******************************************************************************/
CConfigData::CConfigData()
{
/*******************************************************************************
Function Name           : CConfigData
Arguments Description   :
Return Value            : 
Function Description    : This method is the constructor
*******************************************************************************/
	this->m_sConfigurationFileName = "";
	this->m_uProtocol = InvalidSocketProtocol;
	this->m_uSocketType = InvalidSocketType;
	this->m_iServerPortNumber = 0;
	this->m_sServerSourceIP = "";
	this->m_iMaxMessageHandlerThreads = 0;
	this->m_iMaxMessageSenderThreads = 0;
}
CConfigData::~CConfigData()
{
}

std::string CConfigData::GetConfigurationFileName()
{
/*******************************************************************************
Function Name           : GetConfigurationFileName
Arguments Description   :
Return Value            : Name of the configuration file
Function Description    : This method is to get the name of the configuration
			  file
*******************************************************************************/
	return this->m_sConfigurationFileName;
}

void CConfigData::SetConfigurationFileName(std::string a_sConfigurationFileName)
{
/*******************************************************************************
Function Name           : GetInstance
Arguments Description   : name of the configuration file
Return Value            : 
Function Description    : This method is to set name of the configuration file
*******************************************************************************/
	this->m_sConfigurationFileName = a_sConfigurationFileName;
}

int CConfigData::GetMaxMessageHandlerThreads()
{
/*******************************************************************************
Function Name           : GetMaxMessageHandlerThreads
Arguments Description   : 
Return Value            : Number of message handling threads
Function Description    : This method is to get the number of message handling 
			  threads
*******************************************************************************/
	return this->m_iMaxMessageHandlerThreads;
}
void CConfigData::SetMaxMessageHandlerThreads(int a_iMaxMessageHandlerThreads)
{
/*******************************************************************************
Function Name           : SetMaxMessageHandlerThreads
Arguments Description   : Number of message handling threads
Return Value            : 
Function Description    : This method is to set the number of message handling
			  threads
*******************************************************************************/
	this->m_iMaxMessageHandlerThreads = a_iMaxMessageHandlerThreads;
}
int CConfigData::GetMaxMessageSenderThreads()
{
/*******************************************************************************
Function Name           : GetMaxMessageSenderThreads
Arguments Description   : 
Return Value            : Number of message sending threads
Function Description    : This method is to get the number of message sending 
			  threads
*******************************************************************************/
	return this->m_iMaxMessageSenderThreads;
}
void CConfigData::SetMaxMessageSenderThreads(int a_iMaxMessageSenderThreads)
{
/*******************************************************************************
Function Name           : SetMaxMessageSenderThreads
Arguments Description   : Number of message sending threads 
Return Value            : 
Function Description    : This method is to set the number of message sending 
			  threads
*******************************************************************************/
	this->m_iMaxMessageSenderThreads = a_iMaxMessageSenderThreads;
}

std::string CConfigData::GetParam( std::string a_sParameter)
{
/*******************************************************************************
Function Name           : GetParam
Arguments Description   : Parameter name of which the value need to be find out
			  from the configuration file
Return Value            : 
Function Description    : This method read the configuration file and get the 
			  value corresponding to the parameter given as 
			  argument
*******************************************************************************/
	std::string l_sParamDefaultVal = "";
	std::string l_sSection = "MessagingServer";
	std::string l_sKey = a_sParameter;
	char l_cpParamVal[256];
	try
	{
	if ( CFileHandler::GetInstance().GetPrivateProfileString( l_sSection.c_str(), l_sKey.c_str(), l_sParamDefaultVal.c_str(), l_cpParamVal, this->m_sConfigurationFileName.c_str() ) == -1 ) {
		return "";
	}

	l_sParamDefaultVal = "";

	//l_sParamDefaultVal.append( l_cpParamVal);
	l_sParamDefaultVal = l_cpParamVal;

	return l_sParamDefaultVal;
	}
	catch (CException a_uException)
	{
		CFileHandler::GetInstance().WriteToSysLog("Exception Raised in CConfigData::GetParam:%s",a_uException.what());
	}
	catch (...)
	{
		CFileHandler::GetInstance().WriteToSysLog("Unknown Exception Raised in CConfigData::GetParam:Error Number:%d:Error String:%s",errno,strerror(errno));
	}
	return "";
}

bool CConfigData::LoadConfigurationData()
{
/*******************************************************************************
Function Name           : LoadConfigurationData
Arguments Description   : name of the configuration file
Return Value            : 
Function Description    : This method loads the required parameters by calling 
			  GetParam method of reading configuraiton file
*******************************************************************************/
	int l_iTemp = 0;
	std::string l_sTemp;
	std::string l_sParameterName;
	try
	{

		//m_uProtocol
		l_sTemp = "";	
		l_sParameterName = "Protocol";
		l_sTemp = this->GetParam(l_sParameterName);
		this->SetProtocol(l_sTemp);
		//m_uSocketType
		l_sTemp = "";	
		l_sParameterName = "SocketType";
		l_sTemp = this->GetParam(l_sParameterName);
		this->SetSocketType(l_sTemp);
		//m_iServerPortNumber

		l_sTemp = "";	
		l_sParameterName = "PortNumber";
		l_sTemp = this->GetParam(l_sParameterName);
		l_iTemp = atoi(l_sTemp.c_str());
		this->SetServerPortNumber(l_iTemp);

		l_sTemp = "";	
		l_sParameterName = "LogDirectory";
		l_sTemp = this->GetParam(l_sParameterName);
		this->SetLogFileDirectory(l_sTemp);

		//m_sServerSourceIP
		l_sTemp = "";	
		l_sParameterName = "SourceServerIP";
		l_sTemp = this->GetParam(l_sParameterName);
		this->SetServerSourceIP(l_sTemp);

		l_sTemp = "";	
		l_sParameterName = "MaxMessageSendThreads";
		l_sTemp = this->GetParam(l_sParameterName);
		l_iTemp = atoi(l_sTemp.c_str());
		this->SetMaxMessageSenderThreads(l_iTemp);

		l_sTemp = "";	
		l_sParameterName = "MaxMessageHandlerThreads";
		l_sTemp = this->GetParam(l_sParameterName);
		l_iTemp = atoi(l_sTemp.c_str());
		this->SetMaxMessageHandlerThreads(l_iTemp);
	}	
	catch (CException a_uException)
	{
		CFileHandler::GetInstance().WriteToSysLog("Exception Raised in CConfigData::LoadConfigurationData:%s",a_uException.what());
		return false;
	}
	catch (...)
	{
		CFileHandler::GetInstance().WriteToSysLog("Unknown Exception Raised in CConfigData::LoadConfigurationData:Error Number:%d:Error String:%s",errno,strerror(errno));
		return false;
	}
	return true;
}

SocketProtocol CConfigData::GetProtocolType()
{
/*******************************************************************************
Function Name           : GetProtocolType
Arguments Description   : 
Return Value            : return the protocol type
Function Description    : This method is to get the protocol type for the socket
*******************************************************************************/
	return this->m_uProtocol;
}

SocketType CConfigData::GetSocketType()
{
/*******************************************************************************
Function Name           : GetSocketType
Arguments Description   : 
Return Value            : returns the socket type
Function Description    : This method is to get the type of the socket
*******************************************************************************/
	return this->m_uSocketType;
}

int CConfigData::GetServerPortNumber()
{
/*******************************************************************************
Function Name           : GetServerPortNumber
Arguments Description   : 
Return Value            : Returns the port number 
Function Description    : This method is to get the port number of the server 
			  to be listened
*******************************************************************************/
	return this->m_iServerPortNumber;
}

std::string CConfigData::GetServerSourceIP()
{
/*******************************************************************************
Function Name           : GetServerSourceIP
Arguments Description   : 
Return Value            : returns the source IP
Function Description    : This method is to get the source IP of the server
*******************************************************************************/
	return this->m_sServerSourceIP;
}

void CConfigData::SetServerSourceIP(std::string a_sServerSourceIP)
{
/*******************************************************************************
Function Name           : SetServerSourceIP
Arguments Description   : The source IP
Return Value            : 
Function Description    : This method is to set the source IP
*******************************************************************************/
	this->m_sServerSourceIP = a_sServerSourceIP;
}

void CConfigData::SetServerPortNumber(int a_iServerPortNumber)
{
/*******************************************************************************
Function Name           : SetServerPortNumber
Arguments Description   : Server port number to be listened
Return Value            : 
Function Description    : This method is to set the port number to be listened
*******************************************************************************/
	this->m_iServerPortNumber = a_iServerPortNumber;
}
std::string CConfigData::GetLogFileDirectory()
{
	return this->m_sLogFileDirectory;
}
void CConfigData::SetLogFileDirectory(std::string a_sLogFileDirectory)
{
	this->m_sLogFileDirectory = a_sLogFileDirectory;
}

void CConfigData::SetProtocol(std::string a_sProtocol)
{
/*******************************************************************************
Function Name           : SetProtocol
Arguments Description   : Protocol to be used
Return Value            : 
Function Description    : This method is to set the protocol to be used
*******************************************************************************/
	try
	{
		if(a_sProtocol == "TCPSocket")
		{
			this->m_uProtocol = TCPSocket;
		}
		else if(a_sProtocol == "UDPSocket")
		{
			this->m_uProtocol = SCTPSocket;
		}
		else if(a_sProtocol == "SCTPSocket")
		{
			this->m_uProtocol = SCTPSocket;
		}
		else
		{
			this->m_uProtocol = InvalidSocketProtocol;
		}
	}
	catch (...)
	{
		CFileHandler::GetInstance().WriteToSysLog("Unknown Exception Raised in CConfigData::SetProtocol:Error Number:%d,:Error String:%s",errno,strerror(errno));
	}
}

void CConfigData::SetSocketType(std::string a_sSocketType)
{
/*******************************************************************************
Function Name           : SetSocketType
Arguments Description   : Socket type to be used to create socket
Return Value            : 
Function Description    : This method is to set the socket type to be used
*******************************************************************************/
	try
	{
		if(a_sSocketType == "SynchronousSocket")
		{
			this->m_uSocketType = SynchronousSocket;
		}
		else if(a_sSocketType == "AsynchronousSocket")
		{
			this->m_uSocketType = AsynchronousSocket;
		}
		else
		{
			this->m_uSocketType = InvalidSocketType;
		}
	}
	catch (...)
	{
		CFileHandler::GetInstance().WriteToSysLog("Unknown Exception Raised in CConfigData::SetSocketType:Error Number:%d,:Error String:%s",errno,strerror(errno));
	}
}


