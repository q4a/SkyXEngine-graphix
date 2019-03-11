#ifndef _CGXSurface_H_
#define _CGXSurface_H_

#include "GXContext.h"

class CGXSurface: public IGXSurface
{					  
	friend class CGXContext;
	friend class CGXTexture2D;
	friend class CGXTextureCube;
	friend class CGXSwapChain;

	~CGXSurface();

	CGXContext * m_pRender;
	D3D11_TEXTURE2D_DESC m_descTex2D;
	ID3D11Texture2D *m_pSurface = NULL;

	D3D11_RENDER_TARGET_VIEW_DESC m_descRTV;
	ID3D11RenderTargetView *m_pRTV = NULL;

	UINT m_uWidth;
	UINT m_uHeight;
	GXFORMAT m_format;

	UINT m_uMipmapNumber = 0;
	UINT m_uMipmapTotal = 0;
	GXCUBEMAP_FACES m_face = GXCUBEMAP_FACE_POSITIVE_X;

	bool m_bAutoResize = false;
	float m_fSizeCoeffW = 1.0f;
	float m_fSizeCoeffH = 1.0f;

	void onDevLost();
	void onDevRst(UINT uScreenWidth, UINT uScreenHeight);

	void initRT();
	void releaseRT();

public:
	CGXSurface(CGXContext * pRender, UINT uWidth, UINT uHeight, GXFORMAT format, ID3D11Texture2D *pSurface):
		m_pRender(pRender),
		m_uWidth(uWidth),
		m_uHeight(uHeight),
		m_format(format),
		m_pSurface(pSurface)
	{
		m_descTex2D.Width = uWidth;
		m_descTex2D.Height = uHeight;
		m_descTex2D.Format = m_pRender->getDXFormat(format);
	}

	void Release();

	UINT getWidth();
	UINT getHeight();
	GXFORMAT getFormat();
};

#endif
