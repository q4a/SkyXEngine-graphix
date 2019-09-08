#ifndef _IDSGvertexDeclarationOgl_H_
#define _IDSGvertexDeclarationOgl_H_

#include "GXDevice.h"

class CGXVertexDeclaration: public IGXVertexDeclaration
{
	friend class CGXContext;
	friend class CGXDevice;

	CGXVertexDeclaration(ID3D11Device *pDevice, CGXDevice *pRender, const GXVertexElement *pDecl);
	~CGXVertexDeclaration();
	CGXDevice *m_pRender;
		
	
	ID3D11InputLayout *m_pDeclaration = NULL;

	byte m_u8StreamCount;
	byte m_u8StreamStride[GX_MAX_VSTREAM];
	GXDECLSPEC m_u8SpecSpec[GX_MAX_VSTREAM];
};

#endif
