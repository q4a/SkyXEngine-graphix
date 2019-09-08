#ifndef _IDSRvertexBuffer_H_
#define _IDSRvertexBuffer_H_

#include "GXDevice.h"

class CGXVertexBuffer: public IGXVertexBuffer
{
	friend class CGXContext;
	friend class CGXDevice;

	CGXVertexBuffer(CGXDevice *pRender):
		m_pRender(pRender)
	{
	}
	~CGXVertexBuffer();

	CGXDevice *m_pRender;
	ID3D11Buffer *m_pBuffer;
	//D3DFORMAT m_format;
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
