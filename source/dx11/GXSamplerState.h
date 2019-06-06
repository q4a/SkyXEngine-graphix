#ifndef _CGXSamplerState_H_
#define _CGXSamplerState_H_

#include "GXContext.h"

class CGXSamplerState: public IGXSamplerState
{
	friend class CGXContext;

	CGXSamplerState(CGXContext *pRender):
		m_pRender(pRender)
	{
	}

	CGXContext *m_pRender;
	ID3D11SamplerState *m_pStateBlock;

	GXSAMPLER_DESC m_desc;

	void onDevRst();

public:
	void Release() override;
};

#endif
