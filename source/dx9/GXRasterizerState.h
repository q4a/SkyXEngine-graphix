#ifndef _CGXRasterizerState_H_
#define _CGXRasterizerState_H_

#include "GXContext.h"

class CGXRasterizerState: public IGXRasterizerState
{
	friend class CGXContext;

	CGXRasterizerState(CGXContext * pRender):m_pRender(pRender)
	{
	};
	~CGXRasterizerState();

	CGXContext * m_pRender;
	IDirect3DStateBlock9 *m_pStateBlock = NULL;
	BOOL m_isScissorsEnabled = 0;
	GXRASTERIZER_DESC m_desc;

	void onDevLost();
	void onDevRst();

public:
	void Release();
};

#endif
