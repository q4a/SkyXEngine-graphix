#ifndef _CGXTexture_H_
#define _CGXTexture_H_

#include "GXDevice.h"

class CGXTexture2D: public IGXTexture2D
{
	friend class CGXContext;
	friend class CGXDevice;

protected:
	CGXDevice *m_pRender;
	CGXTexture2D(CGXDevice *pRender):
		m_pRender(pRender)
	{
	}
	~CGXTexture2D();
	GXFORMAT m_format;
	bool m_bWasReset;
	bool m_isResettable = false;
	bool m_bAutoResize = false;

	int m_iTotalSize = 0;

	UINT m_uWidth;
	UINT m_uHeight;
	D3D11_TEXTURE2D_DESC m_descTex2D;
	ID3D11Texture2D *m_pTexture = NULL;
	D3D11_SHADER_RESOURCE_VIEW_DESC m_descSRV;
	ID3D11ShaderResourceView *m_pSRV = NULL;
	D3D11_UNORDERED_ACCESS_VIEW_DESC m_descUAV;
	ID3D11UnorderedAccessView *m_pUAV = NULL;
	float m_fSizeCoeffW = 1.0f;
	float m_fSizeCoeffH = 1.0f;
	UINT m_uMipLevels;

	void onDevLost();
	void onDevRst(UINT uScreenWidth, UINT uScreenHeight);

	Array<IGXSurface*> m_apSurfaces;
public:
	IGXSurface* getMipmap(UINT i = 0) override;

	UINT getWidth() override;
	UINT getHeight() override;

	bool lock(void **ppData, GXTEXLOCK mode) override;
	void unlock() override;

	void update(void *pData) override;

	GXFORMAT getFormat() override;
	bool wasReset() override;

	ID3D11ShaderResourceView* getDXTexture();

	GXTEXTURE_TYPE getType() override;

	IGXSurface* asRenderTarget() override;
};

class CGXTexture3D: public IGXTexture3D
{
	friend class CGXContext;
	friend class CGXDevice;

protected:
	CGXDevice *m_pRender;
	CGXTexture3D(CGXDevice *pRender):
		m_pRender(pRender)
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
	D3D11_UNORDERED_ACCESS_VIEW_DESC m_descUAV;
	ID3D11UnorderedAccessView *m_pUAV = NULL;
	UINT m_uMipLevels;

	IGXSurface *m_pSurfaceRT = NULL;
public:
	UINT getWidth() override;
	UINT getHeight() override;
	UINT getDepth() override;

	IGXSurface* asRenderTarget() override;

	void update(void *pData) override;

	GXFORMAT getFormat() override;
	bool wasReset() override;

	ID3D11ShaderResourceView* getDXTexture();

	GXTEXTURE_TYPE getType() override;
};

class CGXTextureCube: public IGXTextureCube
{
	friend class CGXContext;
	friend class CGXDevice;

protected:
	CGXDevice *m_pRender;
	CGXTextureCube(CGXDevice *pRender):
		m_pRender(pRender)
	{
	}
	~CGXTextureCube();
	GXFORMAT m_format;
	bool m_bWasReset;
	bool m_isResettable = false;
	bool m_bAutoResize = false;

	int m_iTotalSize = 0;

	UINT m_uSize;
	D3D11_TEXTURE2D_DESC m_descTex2D;
	ID3D11Texture2D *m_pTexture = NULL;
	D3D11_SHADER_RESOURCE_VIEW_DESC m_descSRV;
	ID3D11ShaderResourceView *m_pSRV = NULL;
	D3D11_UNORDERED_ACCESS_VIEW_DESC m_descUAV;
	ID3D11UnorderedAccessView *m_pUAV = NULL;
	float m_fSizeCoeff = 1.0f;

	UINT m_uMipLevels;
	UINT m_uUsage;

	Array<IGXSurface*> m_apSurfaces;


	void onDevLost();
	void onDevRst(UINT uScreenHeight);

	IGXSurface *m_pSurfaceRT = NULL;
public:
	IGXSurface* getMipmap(GXCUBEMAP_FACES face, UINT i = 0) override;

	UINT getSize() override;

	bool lock(void **ppData, GXCUBEMAP_FACES face, GXTEXLOCK mode) override;
	void unlock() override;

	GXFORMAT getFormat() override;
	bool wasReset() override;

	ID3D11ShaderResourceView* getDXTexture();

	GXTEXTURE_TYPE getType() override;

	IGXSurface* asRenderTarget() override;
};

#endif
