#ifndef __CCONFIGDATA_H__
#define __CCONFIGDATA_H__
#include<iostream>
#include <definitions.h>
#include <string.h>
#include <cfilehandler.h>

class CConfigData
{
private:
	std::string	m_sConfigurationFileName;
	std::string	m_sServerSourceIP;
	int		m_iServerPortNumber;
	int		m_iMaxMessageHandlerThreads;
	int		m_iMaxMessageSenderThreads;
	SocketProtocol	m_uProtocol;
	SocketType	m_uSocketType;
	std::string	m_sLogFileDirectory;
public:
	CConfigData();
	~CConfigData();
	bool LoadConfigurationData();
	std::string GetConfigurationFileName();
	SocketProtocol GetProtocolType();
	SocketType GetSocketType();
	int GetServerPortNumber();
	std::string GetServerSourceIP();

	void SetConfigurationFileName(std::string);
	void SetServerSourceIP(std::string);
	void SetServerPortNumber(int);
	void SetProtocol(std::string);
	void SetSocketType(std::string);	
	int GetMaxMessageHandlerThreads();
	void SetMaxMessageHandlerThreads(int);	
	int GetMaxMessageSenderThreads();
	void SetMaxMessageSenderThreads(int);
	std::string GetLogFileDirectory();
	void SetLogFileDirectory(std::string);	
	std::string  GetParam( std::string );
};
#endif

