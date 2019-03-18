#ifndef _CGXTexture_H_
#define _CGXTexture_H_

#include "GXContext.h"

class CGXTexture2D: public IGXTexture2D
{
	friend class CGXContext;

protected:
	CGXContext * m_pRender;
	CGXTexture2D(CGXContext * pRender): m_pRender(pRender)
	{
	}
	~CGXTexture2D();
	GXFORMAT m_format;
	bool m_bWasReset;
	bool m_isResettable = false;
	bool m_bAutoResize = false;

	UINT m_uWidth;
	UINT m_uHeight;
	D3D11_TEXTURE2D_DESC m_descTex2D;
	ID3D11Texture2D *m_pTexture = NULL;
	D3D11_SHADER_RESOURCE_VIEW_DESC m_descSRV;
	ID3D11ShaderResourceView *m_pSRV = NULL;
	float m_fSizeCoeffW = 1.0f;
	float m_fSizeCoeffH = 1.0f;
	UINT m_uMipLevels;

	void onDevLost();
	void onDevRst(UINT uScreenWidth, UINT uScreenHeight);

	Array<IGXSurface*> m_apSurfaces;
public:
	void Release();

	IGXSurface *getMipmap(UINT i = 0);

	UINT getWidth();
	UINT getHeight();

	bool lock(void **ppData, GXTEXLOCK mode);
	void unlock();

	void update(void *pData);

	GXFORMAT getFormat();
	bool wasReset();

	ID3D11ShaderResourceView *getDXTexture();

	GXTEXTURE_TYPE getType();
};

class CGXTexture3D: public IGXTexture3D
{
	friend class CGXContext;

protected:
	CGXContext * m_pRender;
	CGXTexture3D(CGXContext * pRender): m_pRender(pRender)
	{
	}
	~CGXTexture3D();
	GXFORMAT m_format;
	bool m_bWasReset;
	bool m_isResettable = false;

	UINT m_uWidth;
	UINT m_uHeight;
	UINT m_uDepth;
	D3D11_TEXTURE3D_DESC m_descTex3D;
	ID3D11Texture3D *m_pTexture = NULL;
	D3D11_SHADER_RESOURCE_VIEW_DESC m_descSRV;
	ID3D11ShaderResourceView *m_pSRV = NULL;
	UINT m_uMipLevels;

	IGXSurface *m_pSurfaceRT = NULL;
public:
	void Release();

	UINT getWidth();
	UINT getHeight();
	UINT getDepth();

	IGXSurface *asRenderTarget();

	void update(void *pData);

	GXFORMAT getFormat();
	bool wasReset();

	ID3D11ShaderResourceView *getDXTexture();

	GXTEXTURE_TYPE getType();
};

class CGXTextureCube: public IGXTextureCube
{
	friend class CGXContext;

protected:
	CGXContext * m_pRender;
	CGXTextureCube(CGXContext * pRender): m_pRender(pRender)
	{
	}
	~CGXTextureCube();
	GXFORMAT m_format;
	bool m_bWasReset;
	bool m_isResettable = false;
	bool m_bAutoResize = false;

	UINT m_uSize;
	D3D11_TEXTURE2D_DESC m_descTex2D;
	ID3D11Texture2D *m_pTexture = NULL;
	D3D11_SHADER_RESOURCE_VIEW_DESC m_descSRV;
	ID3D11ShaderResourceView *m_pSRV = NULL;
	float m_fSizeCoeff = 1.0f;

	UINT m_uMipLevels;
	UINT m_uUsage;

	Array<IGXSurface*> m_apSurfaces;


	void onDevLost();
	void onDevRst(UINT uScreenHeight);
public:
	void Release();

	IGXSurface *getMipmap(GXCUBEMAP_FACES face, UINT i = 0);

	UINT getSize();

	bool lock(void **ppData, GXCUBEMAP_FACES face, GXTEXLOCK mode);
	void unlock();

	GXFORMAT getFormat();
	bool wasReset();

	ID3D11ShaderResourceView *getDXTexture();

	GXTEXTURE_TYPE getType();
};

#endif
