#ifndef _CGXRasterizerState_H_
#define _CGXRasterizerState_H_

#include "GXDevice.h"

class CGXRasterizerState: public IGXRasterizerState
{
	friend class CGXContext;
	friend class CGXDevice;

	CGXRasterizerState(CGXDevice *pRender):
		m_pRender(pRender)
	{
	}
	~CGXRasterizerState();

	CGXDevice *m_pRender;
	ID3D11RasterizerState *m_pStateBlock = NULL;
	BOOL m_isScissorsEnabled = 0;
	GXRasterizerDesc m_desc;

	void onDevLost();
	void onDevRst();
};

#endif
