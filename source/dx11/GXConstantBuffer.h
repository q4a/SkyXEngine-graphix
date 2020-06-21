#ifndef _GXCONSTANTBUFFER_H_
#define _GXCONSTANTBUFFER_H_

#include "GXDevice.h"

class CGXConstantBuffer: public IGXConstantBuffer
{
	friend class CGXContext;
	friend class CGXDevice;

	CGXConstantBuffer(CGXDevice *pRender, UINT uSize, bool isPartialUpdateAllowed=false);
	~CGXConstantBuffer();

	void update(const void *pData, size_t sizeUpdate = 0) override;
	UINT getSize() override;

	CGXDevice *m_pRender;
	ID3D11Buffer *m_pBuffer;
	UINT m_uSize;
	bool m_isPartialUpdateAllowed;
};

#endif
