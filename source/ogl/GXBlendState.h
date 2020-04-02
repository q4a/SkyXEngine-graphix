#ifndef _CGXBlendState_H_
#define _CGXBlendState_H_

#include "GXDevice.h"

class CGXBlendState: public IGXBlendState
{
	friend class CGXContext;
	friend class CGXDevice;

	CGXBlendState(CGXDevice *pRender):
		m_pRender(pRender)
	{
	};
	~CGXBlendState();

	CGXDevice *m_pRender;
	IGXBlendState *m_pStateBlock = NULL; // ID3D11BlendState or GLenum
	GXBlendDesc m_desc;

	void onDevLost();
	void onDevRst();
};

#endif
