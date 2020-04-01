#ifndef _IDSRvertexBufferOgl_H_
#define _IDSRvertexBufferOgl_H_

#include "GXContext.h"

class CGXVertexBuffer: public IGXVertexBuffer
{
	friend class CGXContext;
	friend class CGXDevice;

	CGXVertexBuffer(CGXContext * pRender):m_pRender(pRender)
	{
	};
	CGXContext * m_pRender;
	GLuint m_pBuffer;
	bool m_wasReset = true;
	UINT m_uSize;
public:
	void Release();

	bool lock(void** ppData, GXBUFFERLOCK mode) override;
	void unlock() override;

	bool wasReset() override;
};

#endif
