#ifndef _CGXBlendState_H_
#define _CGXBlendState_H_

#include "GXContext.h"

class CGXBlendState: public IGXBlendState
{
	friend class CGXContext;

	CGXBlendState(CGXContext *pRender):
		m_pRender(pRender)
	{
	};
	~CGXBlendState();

	CGXContext *m_pRender;
	ID3D11BlendState *m_pStateBlock = NULL;
	GXBlendDesc m_desc;

	void onDevLost();
	void onDevRst();

public:
	void Release() override;
};

#endif
