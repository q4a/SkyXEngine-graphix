
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

	//! инкрементирует счетчик ссылок на объект
	void AddRef()
	{
		++m_uRefCount;
	}

	//! декрементирует счетчик ссылок, если количество ссылок == 0 тогда освобождает/удаляет данные
	virtual void Release() = 0;
};

#endif