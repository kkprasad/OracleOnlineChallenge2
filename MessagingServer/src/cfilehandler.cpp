/*******************************************************************************
File Name               : cfilehandler.cpp
Class Name              : CFileHandler
Description             : This file contains the methods to do various file 
			  operations
*******************************************************************************/

#include <cfilehandler.h>
#include<sys/time.h>
#include <time.h>

std::map<int,FILE*> CFileHandler::m_mFilePointerMap;
std::string CFileHandler::m_sLogDirectory;
CFileHandler CFileHandler::m_uFileHandlerInstance;

CFileHandler CFileHandler::GetInstance()
{
/*******************************************************************************
Function Name           : GetInstance
Arguments Description   : 
Return Value            : Instance of CFileHandler
Function Description    : This method is to get the instance of the 
			  Class CFileHandler
*******************************************************************************/
	return CFileHandler::m_uFileHandlerInstance;
}

void CFileHandler::SetLogFileDirectory(std::string a_sLogFileDirectory)
{
	CFileHandler::m_sLogDirectory = a_sLogFileDirectory;
}
std::string CFileHandler::GetLogFileDirectory()
{
	return CFileHandler::m_sLogDirectory;
}


std::string CFileHandler::GetTime()
{
/*********************************************************************
Function Name           : GetTime
Arguments Description   : 
Return Value            : Current date and time
Function Description    : This method is to get the current date 
			  and time 
*********************************************************************/
	static char 	l_cpReturnString[1024]={0};
	char 		l_cpMonths[][4]= {"Jan","Feb","Mar","Apr","May", "Jun","Jul","Aug","Sep","Oct","Nov","Dec"}; 
	std::string 	l_sReturnString;
	timeval 	l_tTime;
        tm 		*l_tmLocalTime;

	try
	{
		gettimeofday(&l_tTime,NULL);
		l_tmLocalTime = new tm;
		localtime_r(&l_tTime.tv_sec, l_tmLocalTime);
		if(l_tmLocalTime == NULL)
		{
			CException l_uException("localtime returns NULL");
			throw l_uException;
		}
		sprintf(l_cpReturnString, "%02d-%s-%04d %02d:%02d:%02d",	
				l_tmLocalTime->tm_mday,	
				l_cpMonths[l_tmLocalTime->tm_mon],	
				1900+l_tmLocalTime->tm_year,
				l_tmLocalTime->tm_hour,
				l_tmLocalTime->tm_min,
				l_tmLocalTime->tm_sec);
		l_sReturnString = l_cpReturnString;
		if(l_tmLocalTime != NULL)
		{
			delete l_tmLocalTime;
		}
	} 
	catch(CException a_uException)
	{
		if(l_tmLocalTime != NULL)
		{
			delete l_tmLocalTime;
		}
		std::cout<<"Exception Raised in GetTime:"<<a_uException.what()<<std::endl;
		l_sReturnString = "";
	}
	catch (...)
	{
		std::cout<<"Exception Raised in GetTime:Error Number:"<<errno<<",Error String:"<<strerror(errno)<<std::endl;
		if(l_tmLocalTime != NULL)
		{
			delete l_tmLocalTime;
		}
	}
	return l_sReturnString;
}

bool CFileHandler::WriteToSysLog(const char* a_cpFormat,...)
{
	char            l_cpBuffer[1024]={0};
	va_list         l_uArgList;
	try
	{
		va_start(l_uArgList, a_cpFormat);
		vsprintf(l_cpBuffer, a_cpFormat ,l_uArgList);
		openlog("msgserverd", LOG_PID|LOG_CONS, LOG_USER);
		//syslog(LOG_NOTICE, l_cpBuffer);
		syslog(LOG_INFO, l_cpBuffer);
		closelog();
		return true;
	}
	catch(...)
	{
		std::cout<<"Error in writing to system log. Message is..\n"<<l_cpBuffer<<std::endl;
		return false;
	}        
}

bool CFileHandler::WriteToFile(const int a_iFileNumber,const char* a_cpFormat,...)
{
	/*******************************************************************************
	  Function Name           : WriteToFile
	  Arguments Description   : File Number, Format of the string, data to be written
	  Return Value            : Status of the file writing
	  Function Description    : This method is to write data to log file 
	  The opened file pointer will be stored in map so that
	  it will not be opened again and again
	 *******************************************************************************/
	char 		l_cpBuffer[1024]={0};
	va_list 	l_uArgList;
	char 		l_cpFileName[128]={0};
	FILE 		*l_FFilePointer = NULL;
	try
	{
		va_start(l_uArgList, a_cpFormat);
		if(CFileHandler::m_mFilePointerMap.find(a_iFileNumber) != CFileHandler::m_mFilePointerMap.end())
		{
			l_FFilePointer = CFileHandler::m_mFilePointerMap[a_iFileNumber];
			if(l_FFilePointer == NULL)
			{
				sprintf(l_cpFileName, "%s/log%03u.log",CFileHandler::m_sLogDirectory.c_str(),a_iFileNumber);
//				sprintf(l_cpFileName, "log%03u.log",a_iFileNumber);
				l_FFilePointer = fopen(l_cpFileName,"a+");
				if(l_FFilePointer==NULL)
				{
					std::string l_sDescription;
					l_sDescription = "Unable to Open File ";
					l_sDescription += l_cpFileName;
					CException l_uException(l_sDescription);
					throw l_uException;
				}
				else
				{
					CFileHandler::m_mFilePointerMap[a_iFileNumber] = l_FFilePointer;
				}
			}
		}
		else
		{
			sprintf(l_cpFileName, "%s/log%03u.txt",CFileHandler::m_sLogDirectory.c_str(),a_iFileNumber);
			//sprintf(l_cpFileName, "log%03u.txt",a_iFileNumber);
			l_FFilePointer = fopen(l_cpFileName,"a+");
			if(l_FFilePointer==NULL) 
			{
				std::string l_sDescription;
				l_sDescription = "Unable to Open File ";
				l_sDescription += l_cpFileName;
				CException l_uException(l_sDescription);
				throw l_uException;	
			}
			else
			{
				CFileHandler::m_mFilePointerMap[a_iFileNumber] = l_FFilePointer;
			}
		}
	} catch(CException a_uException)
	{
		std::cout<<"Exception Raised in WriteToFile:Block for Getting File Pointer:"<<a_uException.what()<<std::endl;
		return false;
	}
	catch (...)
	{
		std::cout<<"Exception Raised in WriteToFile:Error Number:"<<errno<<":Error String:"<<strerror(errno)<<std::endl;
	}
	try
	{
		vsprintf(l_cpBuffer, a_cpFormat ,l_uArgList);

		fprintf(l_FFilePointer, "\n%s#%s", GetTime().c_str(), l_cpBuffer);
		fflush(l_FFilePointer);
		va_end(l_uArgList);
		std::cout<<l_cpBuffer<<std::endl;
	} catch(CException a_uException)
	{
		std::cout<<"Exception Raised in WriteToFile:"<<a_uException.what()<<std::endl;
		if(l_FFilePointer != NULL)
		{
			fclose(l_FFilePointer);
			CFileHandler::m_mFilePointerMap.erase(a_iFileNumber);
		}
		return false;
	}
	catch (...)
	{
		std::cout<<"Exception Raised in WriteToFile:Error Number:"<<errno<<":Error String:"<<strerror(errno)<<std::endl;
	}
	return true;
}

long CFileHandler::GetPrivateProfileString(const char *a_cpSection,const char *a_cpKey,const char *a_cpDefaultString, char *a_cpData,const char *a_cpFileName)
{
/*******************************************************************************
Function Name           : GetPrivateProfileString
Arguments Description   : Section Name, Key Tag, Default Value, Pointer to 
			  store the read data, File name to be read
Return Value            : Length of the value that is stored in a_cpData
Function Description    : This method is to Read value from configuration file
			  corresponding to the Key Tag in a particular section
			  If the value corresponding to the key tag is not found,
			  the default string value will be copied
*******************************************************************************/

	FILE 	*l_FFilePointer = NULL;
	char 	l_cpSection[100];
	try
	{
		if((a_cpSection == NULL) || (a_cpKey == NULL) || (a_cpDefaultString == NULL) || (a_cpFileName ==NULL) || \
				(a_cpData == NULL))
			return -1;
		if(strlen(a_cpSection) >96)
			return -1;
		l_FFilePointer=fopen(a_cpFileName,"r");

		if(l_FFilePointer == NULL)
		{
			CFileHandler::GetInstance().WriteToFile(100,"Exception Raised CFileHandler::GetPrivateProfileString:Error in opening configuraiton file. File pointer is NULL\n");	
			CFileHandler::GetInstance().WriteToSysLog("Exception Raised CFileHandler::GetPrivateProfileString:Error in opening configuraiton file. File pointer is NULL\n");	
//			CException l_uException("Error in opening configuraiton file. File pointer is NULL");
//			throw l_uException;
			return -1;
		}
	}
	catch(CException a_uException)
	{
		CFileHandler::GetInstance().WriteToFile(100,"Exception Raised CFileHandler::GetPrivateProfileString:%s\n",a_uException.what());	
		return -1;
	}
	catch(...)
	{
		CFileHandler::GetInstance().WriteToFile(100,"Error %d : %s \n",errno,strerror(errno));	
		return -1;
	}


	char 		l_cpDataBuffer[100] = {0x00};
	char 		l_cpTempDataBuffer[100] = {0x00};
	char 		*l_cpTagName = NULL;
	char 		*l_cpTagValue = NULL;	
	int 		l_ulSectionLength = 0;
	unsigned long 	l_ulLength= 0;

	try
	{

		l_cpSection[0]='[';
		l_cpSection[1]='\0';
		strcat(l_cpSection,a_cpSection);
		strcat(l_cpSection,"]");

		l_ulLength = strlen(a_cpDefaultString);
		strcpy(a_cpData,a_cpDefaultString);

		fgets(l_cpDataBuffer,100,l_FFilePointer);
		while(!feof(l_FFilePointer))
		{
			if(l_cpDataBuffer[0]=='[')
			{
				l_ulSectionLength = strlen(l_cpDataBuffer);

				if(l_cpDataBuffer[l_ulSectionLength - 1] == '\n')
					l_ulSectionLength--;

				if(l_cpDataBuffer[l_ulSectionLength - 1] == '\r')
					l_ulSectionLength--;

				l_cpDataBuffer[l_ulSectionLength] = '\0';

				if(strcmp(l_cpDataBuffer,l_cpSection)==0)
				{
					fgets(l_cpDataBuffer,100,l_FFilePointer);

					while(l_cpDataBuffer[0] != '[')
					{
						strcpy(l_cpTempDataBuffer,l_cpDataBuffer);
						l_cpTagName = strtok(l_cpTempDataBuffer,"=");
						l_cpTagValue = strtok(NULL,"=");

						if(l_cpTagName && l_cpTagValue)
						{

							if(strcmp(l_cpTagName,a_cpKey) == 0)
							{
								l_ulLength = strlen(l_cpTagValue);
								if(l_cpTagValue[l_ulLength - 1] == '\n')
									l_ulLength--;

								if(l_cpTagValue[l_ulLength - 1] == '\r')
									l_ulLength--;

								if(l_ulLength != 0)
									strncpy(a_cpData,l_cpTagValue,l_ulLength);
								a_cpData[l_ulLength] = '\0';
								fclose(l_FFilePointer);
								return (l_ulLength);
							}
						}
						fgets(l_cpDataBuffer,100,l_FFilePointer);
						if(feof(l_FFilePointer))
							break;
					}
				}
			}
			fgets(l_cpDataBuffer,100,l_FFilePointer);
		}
		fclose(l_FFilePointer);
	} catch(...)
	{
		CFileHandler::GetInstance().WriteToFile(100,"Error %d : %s \n",errno,strerror(errno));
		if (l_FFilePointer != NULL)
		{
			fclose(l_FFilePointer);
		}
		return -1;
	}
	return (l_ulLength);
}
CFileHandler::CFileHandler()
{
}
CFileHandler::~CFileHandler()
{
}
void CFileHandler::ClearFilePointerMap()
{
	try
	{
		std::map<int,FILE*>::iterator l_uMapIterator;
		for(l_uMapIterator = CFileHandler::m_mFilePointerMap.begin();l_uMapIterator != CFileHandler::m_mFilePointerMap.end();l_uMapIterator++)
		{
			if(l_uMapIterator->second != NULL)
			{
				fclose(l_uMapIterator->second);
			}
		}
		CFileHandler::m_mFilePointerMap.clear();
	}
	catch(...)
	{
		CFileHandler::GetInstance().WriteToFile(100,"Error in ClearFilePointerMap: Error:%d : %s \n",errno,strerror(errno));
	}
}
