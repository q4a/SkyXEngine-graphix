#ifndef _CGXSurface_H_
#define _CGXSurface_H_

#include "GXContext.h"

class CGXSurface: public IGXSurface
{					  
	friend class CGXContext;
	friend class CGXTexture2D;
	friend class CGXTextureCube;

	~CGXSurface();

	CGXContext * m_pRender;
	IDirect3DSurface9 *m_pSurface = NULL;

	UINT m_uWidth;
	UINT m_uHeight;
	GXFORMAT m_format;

	D3DMULTISAMPLE_TYPE m_multisampleType;

	UINT m_uMipmapNumber;
	GXCUBEMAP_FACES m_face;

	bool m_bAutoResize = false;
	float m_fSizeCoeffW = 1.0f;
	float m_fSizeCoeffH = 1.0f;

	void onDevLost();
	void onDevRst(UINT uScreenWidth, UINT uScreenHeight);

public:
	CGXSurface(CGXContext * pRender, UINT uWidth, UINT uHeight, GXFORMAT format, IDirect3DSurface9 *pSurface):
		m_pRender(pRender),
		m_uWidth(uWidth),
		m_uHeight(uHeight),
		m_format(format),
		m_pSurface(pSurface)
	{
	};

	void Release();

	UINT getWidth();
	UINT getHeight();
	GXFORMAT getFormat();
};

#endif
