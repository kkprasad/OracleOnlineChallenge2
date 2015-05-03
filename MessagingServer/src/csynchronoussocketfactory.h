#ifndef __CSYNCHRONOUSSOCKETFACTORY_H__
#define __CSYNCHRONOUSSOCKETFACTORY_H__
#include <csocketfactory.h>
#include <ctcpsocket.h>

class CSynchronousSocketFactory:public CSocketFactory
{
public:
	CSynchronousSocketFactory();
	~CSynchronousSocketFactory();
	CBasicSocket* MakeSocketObject(int);
};
#endif
