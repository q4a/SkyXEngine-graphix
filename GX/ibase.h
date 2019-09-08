
/*****************************************************
Copyright © DogmaNet Team, 2019
Site: dogmanet.ru
See the license in LICENSE
*****************************************************/

#ifndef __GXIBASE_H
#define __GXIBASE_H

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

	//! инкрементирует счетчик ссылок на объект
	void AddRef()
	{
		++m_uRefCount;
	}

	//! декрементирует счетчик ссылок, если количество ссылок == 0 тогда освобождает/удаляет данные
	virtual void Release()
	{
		--m_uRefCount;
		if(!m_uRefCount)
		{
			delete this;
		}
	}
};

#endif
