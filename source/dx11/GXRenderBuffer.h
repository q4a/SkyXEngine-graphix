#ifndef _IDSGrenderBufferOgl_H_
#define _IDSGrenderBufferOgl_H_

#include "GXContext.h"

class CGXRenderBuffer: public IGXRenderBuffer
{
	friend class CGXContext;
	friend class CGXDevice;

	CGXRenderBuffer(CGXDevice *pRender, UINT uCountStreams, IGXVertexBuffer **ppVertexBuffers, IGXVertexDeclaration *pVertexDeclaration);
	~CGXRenderBuffer();

	CGXDevice *m_pRender;

	IGXVertexBuffer *m_ppVertexBuffers[GX_MAX_VSTREAM];
	IGXVertexDeclaration *m_pVertexDeclaration;

	UINT m_uStreamCount;
};

#endif
