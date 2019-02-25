#ifndef _CGXDepthStencilState_H_
#define _CGXDepthStencilState_H_

#include "GXContext.h"

class CGXDepthStencilState: public IGXDepthStencilState
{
	friend class CGXContext;

	CGXDepthStencilState(CGXContext * pRender):m_pRender(pRender)
	{
	};
	~CGXDepthStencilState();

	CGXContext * m_pRender;
	ID3D11DepthStencilState *m_pStateBlock = NULL;
	GXDEPTH_STENCIL_DESC m_desc;

	void onDevLost();
	void onDevRst();

public:
	void Release();
};

#endif
