#ifndef __CSOCKETFACTORY_H__
#define __CSOCKETFACTORY_H__
#include "CBasicSocket.h"

class CSocketFactory
{
private:
public:
	virtual CBasicSocket* MakeSocketObject(int ) = 0;
};
#endif
