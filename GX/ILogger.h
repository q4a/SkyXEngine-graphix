
/*****************************************************
Copyright © DogmaNet Team, 2019
Site: dogmanet.ru
See the license in LICENSE
*****************************************************/

#ifndef __GXILOGGER_H
#define __GXILOGGER_H

#include <common/types.h>

//! базовый интерфейс
class IGXLogger
{
public:
	virtual void logInfo(const char *szString) = 0;
	virtual void logWarning(const char *szString) = 0;
	virtual void logError(const char *szString) = 0;
};

#endif