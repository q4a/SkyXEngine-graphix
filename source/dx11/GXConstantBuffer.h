#ifndef _GXCONSTANTBUFFER_H_
#define _GXCONSTANTBUFFER_H_

#include "GXDevice.h"

class CGXConstantBuffer: public IGXConstantBuffer
{
	friend class CGXContext;
	friend class CGXDevice;

	CGXConstantBuffer(CGXDevice *pRender, UINT uSize);
	~CGXConstantBuffer();

	void update(const void *pData, IGXContext *pContext = NULL) override;
	UINT getSize() override;

	CGXDevice *m_pRender;
	ID3D11Buffer *m_pBuffer;
	UINT m_uSize;
};

#endif
