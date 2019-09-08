#ifndef _CGXDepthStencilState_H_
#define _CGXDepthStencilState_H_

#include "GXDevice.h"

class CGXDepthStencilState: public IGXDepthStencilState
{
	friend class CGXContext;
	friend class CGXDevice;

	CGXDepthStencilState(CGXDevice *pRender):
		m_pRender(pRender)
	{
	};
	~CGXDepthStencilState();

	CGXDevice *m_pRender;
	ID3D11DepthStencilState *m_pStateBlock = NULL;
	GXDepthStencilDesc m_desc;

	void onDevLost();
	void onDevRst();
};

#endif
