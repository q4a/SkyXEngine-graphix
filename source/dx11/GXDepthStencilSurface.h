#ifndef _CGXDepthStencilSurface_H_
#define _CGXDepthStencilSurface_H_

#include "GXContext.h"

class CGXDepthStencilSurface: public IGXDepthStencilSurface
{					  
	friend class CGXContext;

	CGXDepthStencilSurface(CGXContext * pRender):m_pRender(pRender)
	{
	};
	~CGXDepthStencilSurface();

	CGXContext * m_pRender;
	ID3D11DepthStencilView *m_pSurface = NULL;
	ID3D11Texture2D *m_pBuffer = NULL;

	bool m_bAutoResize = false;
	float m_fSizeCoeffW = 1.0f;
	float m_fSizeCoeffH = 1.0f;

	D3D11_TEXTURE2D_DESC m_descTex;
	D3D11_DEPTH_STENCIL_VIEW_DESC m_desc;

	void onDevLost();
	void onDevRst(UINT uScreenWidth, UINT uScreenHeight);

public:
	void Release();
};

#endif
