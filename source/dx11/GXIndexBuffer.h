#ifndef _IDSRindexBuffer_H_
#define _IDSRindexBuffer_H_

#include "GXContext.h"

class CGXIndexBuffer: public IGXIndexBuffer
{
	friend class CGXContext;

	CGXIndexBuffer(CGXContext * pRender):m_pRender(pRender)
	{
	};
	~CGXIndexBuffer();

	CGXContext * m_pRender;
	ID3D11Buffer *m_pBuffer;
	DXGI_FORMAT m_format;
	bool m_wasReset = true;
	UINT m_uSize;

	bool m_isLockable = true;

public:
	void Release();

	bool lock(void **ppData, GXBUFFERLOCK mode);
	void unlock();

	bool wasReset();
};

#endif
