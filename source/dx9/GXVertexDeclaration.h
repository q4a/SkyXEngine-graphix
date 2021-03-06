﻿#ifndef _IDSGvertexDeclarationOgl_H_
#define _IDSGvertexDeclarationOgl_H_

#include "GXContext.h"

class CGXVertexDeclaration: public IGXVertexDeclaration
{
	friend class CGXContext;
	
	CGXVertexDeclaration(IDirect3DDevice9 *pDevice, CGXContext * pRender, const GXVertexElement * pDecl);
	~CGXVertexDeclaration();
	CGXContext * m_pRender;
		
	IDirect3DVertexDeclaration9 *m_pDeclaration = NULL;

	byte m_u8StreamCount;
	byte m_u8StreamStride[GX_MAX_VSTREAM];
	GXDECLSPEC m_u8SpecSpec[GX_MAX_VSTREAM];

};

#endif
