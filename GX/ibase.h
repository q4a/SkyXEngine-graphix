
/*****************************************************
Copyright © DogmaNet Team, 2019
Site: dogmanet.ru
See the license in LICENSE
*****************************************************/

#ifndef __IBASE_H
#define __IBASE_H

#include <common/types.h>

//! базовый интерфейс
class IGXBaseInterface
{
protected:
	virtual ~IGXBaseInterface()
	{
	}
	UINT m_uRefCount = 1;
public:
	void AddRef()
	{
		++m_uRefCount;
	}
	virtual void Release() = 0;
};

#endif