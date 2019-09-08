#ifndef _CGXSamplerState_H_
#define _CGXSamplerState_H_

#include "GXDevice.h"

class CGXSamplerState: public IGXSamplerState
{
	friend class CGXContext;
	friend class CGXDevice;

	CGXSamplerState(CGXDevice *pRender):
		m_pRender(pRender)
	{
	}

	~CGXSamplerState()
	{
		mem_release(m_pStateBlock);
	}

	CGXDevice *m_pRender;
	ID3D11SamplerState *m_pStateBlock;

	GXSamplerDesc m_desc;

	void onDevRst();
};

#endif
