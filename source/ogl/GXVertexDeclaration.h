#ifndef _IDSGvertexDeclarationOgl_H_
#define _IDSGvertexDeclarationOgl_H_

#include "GXContext.h"

class CGXVertexDeclaration: public IGXVertexDeclaration
{
	friend class CGXContext;
	friend class CGXDevice;

	struct _declItem
	{
		UINT8 stream;
		GLuint index;
		GLint size;
		GLenum type​;
		GLboolean normalized;
		GLsizei stride​;
		const GLvoid * pointer​;//UINT16
	};

	CGXVertexDeclaration(CGXDevice *pDevice, CGXDevice *pRender, const GXVertexElement *pDecl);
	~CGXVertexDeclaration();
	CGXDevice *m_pRender;

	UINT m_uDeclCount;
	_declItem * m_pDeclItems;

	//GLuint m_pVAO;

public:
	void Release();
};

#endif
