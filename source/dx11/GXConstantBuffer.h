#ifndef _GXCONSTANTBUFFER_H_
#define _GXCONSTANTBUFFER_H_

#include "GXContext.h"

class CGXConstantBuffer: public IGXConstantBuffer
{
	friend class CGXContext;

	CGXConstantBuffer(CGXContext *pRender, UINT uSize);
	~CGXConstantBuffer();

	void update(const void *pData) override;
	UINT getSize() override;

	CGXContext *m_pRender;
	ID3D11Buffer *m_pBuffer;
	UINT m_uSize;

public:
	void Release() override;
};

#endif
