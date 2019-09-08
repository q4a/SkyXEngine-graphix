#ifndef _IDSRindexBuffer_H_
#define _IDSRindexBuffer_H_

#include "GXContext.h"

class CGXIndexBuffer: public IGXIndexBuffer
{
	friend class CGXContext;
	friend class CGXDevice;

	CGXIndexBuffer(CGXDevice *pRender):
		m_pRender(pRender)
	{
	}
	~CGXIndexBuffer();

	CGXDevice *m_pRender;
	ID3D11Buffer *m_pBuffer;
	DXGI_FORMAT m_format;
	bool m_wasReset = true;
	UINT m_uSize;

	bool m_isLockable = true;

public:
	void Release() override;

	bool lock(void **ppData, GXBUFFERLOCK mode) override;
	void unlock() override;

	bool wasReset() override;
};

#endif
