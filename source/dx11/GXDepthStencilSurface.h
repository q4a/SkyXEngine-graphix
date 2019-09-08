#ifndef _CGXDepthStencilSurface_H_
#define _CGXDepthStencilSurface_H_

#include "GXDevice.h"

class CGXDepthStencilSurface: public IGXDepthStencilSurface
{					  
	friend class CGXContext;
	friend class CGXDevice;

	CGXDepthStencilSurface(CGXDevice * pRender):
		m_pRender(pRender)
	{
	}
	~CGXDepthStencilSurface();

	CGXDevice *m_pRender;
	ID3D11DepthStencilView *m_pSurface = NULL;
	ID3D11Texture2D *m_pBuffer = NULL;

	bool m_bAutoResize = false;
	float m_fSizeCoeffW = 1.0f;
	float m_fSizeCoeffH = 1.0f;

	D3D11_TEXTURE2D_DESC m_descTex;
	D3D11_DEPTH_STENCIL_VIEW_DESC m_desc;

	void onDevLost();
	void onDevRst(UINT uScreenWidth, UINT uScreenHeight);
};

#endif
