#ifndef __CFILEREADER_H__
#define __CFILEREADER_H__
#include <iostream>
#include <errno.h>
#include <stdlib.h>

class CFileReader
{
protected:
public:
	 static long GetPrivateProfileString(const char *,const char *,const char * ,char *,const char *);
	 static long GetPrivateProfileSection(const char *,char *,const char *);
};

#endif

