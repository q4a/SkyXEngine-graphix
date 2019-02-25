#ifndef _IDSRvertexBuffer_H_
#define _IDSRvertexBuffer_H_

#include "GXContext.h"

class CGXVertexBuffer: public IGXVertexBuffer
{
	friend class CGXContext;

	CGXVertexBuffer(CGXContext * pRender):m_pRender(pRender)
	{
	};
	~CGXVertexBuffer();

	CGXContext * m_pRender;
	ID3D11Buffer *m_pBuffer;
	//D3DFORMAT m_format;
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
