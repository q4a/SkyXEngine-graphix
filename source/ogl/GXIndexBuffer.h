#ifndef _IDSRindexBufferOgl_H_
#define _IDSRindexBufferOgl_H_

#include "GXContext.h"


class CGXIndexBuffer: public IGXIndexBuffer
{
	friend class CGXContext;

	CGXIndexBuffer(CGXContext * pRender):m_pRender(pRender)
	{
	};
	CGXContext * m_pRender;
	GLuint m_pBuffer;
	bool m_wasReset = true;
	UINT m_uIndexSize;
public:
	void Release();

	bool lock(void** ppData, GXBUFFERLOCK mode) override;
	void unlock() override;

	bool wasReset() override;
};

#endif
