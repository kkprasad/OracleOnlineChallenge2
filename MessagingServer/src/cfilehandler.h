#ifndef __CFILEHANDLER_H__
#define __CFILEHANDLER_H__
#include <iostream>
#include <errno.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <map>
#include <cexception.h>
#include <unistd.h>
#include <syslog.h>

class CFileHandler
{
private:
	static std::map<int,FILE*> 	m_mFilePointerMap;
	static std::string		m_sLogDirectory;
	static CFileHandler		m_uFileHandlerInstance;
	
	CFileHandler();
protected:
public:
	static long GetPrivateProfileString(const char *,const char *,const char * ,char *,const char *);
	~CFileHandler();
	std::string GetTime();
	bool WriteToFile(const int ,const char* ,...);
	bool WriteToSysLog(const char* ,...);
	static CFileHandler GetInstance();	
	static void SetLogFileDirectory(std::string);
	std::string GetLogFileDirectory();
	static void ClearFilePointerMap();
};

#endif

