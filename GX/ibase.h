
/*****************************************************
Copyright © DogmaNet Team, 2019
Site: dogmanet.ru
See the license in LICENSE
*****************************************************/

#ifndef __GXIBASE_H
#define __GXIBASE_H

#include <common/types.h>
#include <atomic>

//! базовый интерфейс
class IGXBaseInterface
{
protected:
	IGXBaseInterface()
	{
		m_uRefCount.store(1);
	}
	virtual ~IGXBaseInterface()
	{
	}
	std::atomic_uint m_uRefCount;
public:

	//! инкрементирует счетчик ссылок на объект
	void AddRef()
	{
		++m_uRefCount;

		//assert(m_uRefCount < 256);
	}

	//! декрементирует счетчик ссылок, если количество ссылок == 0 тогда освобождает/удаляет данные
	virtual void Release()
	{
		if(!--m_uRefCount)
		{
			delete this;
		}
	}
};

#endif
