#include "GXTexture.h"
#include "GXSurface.h"

CGXTexture2D::~CGXTexture2D()
{
	mem_release(m_pTexture);
}

GXFORMAT CGXTexture2D::getFormat()
{
	return(m_format);
}

bool CGXTexture2D::wasReset()
{
	return(m_bWasReset);
}

UINT CGXTexture2D::getWidth()
{
	return(m_uWidth);
}

UINT CGXTexture2D::getHeight()
{
	return(m_uHeight);
}

void CGXTexture2D::Release()
{
	--m_uRefCount;
	if(!m_uRefCount)
	{
		for(UINT i = 0, l = m_apSurfaces.size(); i < l; ++i)
		{
			mem_release(m_apSurfaces[i]);
		}
		m_pRender->destroyTexture2D(this);
	}
}

IGXSurface *CGXTexture2D::getMipmap(UINT n)
{
	for(UINT i = 0, l = m_apSurfaces.size(); i < l; ++i)
	{
		if(((CGXSurface*)m_apSurfaces[i])->m_uMipmapNumber == n)
		{
			m_apSurfaces[i]->AddRef();
			return(m_apSurfaces[i]);
		}
	}

	IDirect3DSurface9 *pDXSurface;

	if(!SUCCEEDED(DX_CALL(m_pTexture->GetSurfaceLevel(n, &pDXSurface))))
	{
		return(NULL);
	}

	CGXSurface *pSurface = new CGXSurface(m_pRender, max(1, m_uWidth >> n), max(1, m_uHeight >> n), m_format, pDXSurface);
	pSurface->m_uMipmapNumber = n;
	m_apSurfaces.push_back(pSurface);
	pSurface->AddRef();
	return(pSurface);
}

bool CGXTexture2D::lock(void **ppData, GXTEXLOCK mode)
{
	D3DLOCKED_RECT rect;
	if(FAILED(DX_CALL(m_pTexture->LockRect(0, &rect, NULL, mode == GXTL_WRITE ? 0 : D3DLOCK_READONLY))))
	{
		return(false);
	}
	//CGXContext::debugMessage(GX_LOG_ERROR, "Not implemented: CGXTexture2D::lock");
	*ppData = rect.pBits;
	return(true);
}

void CGXTexture2D::unlock()
{
	DX_CALL(m_pTexture->UnlockRect(0));
	//CGXContext::debugMessage(GX_LOG_ERROR, "Not implemented: CGXTexture2D::unlock");
}

IDirect3DBaseTexture9 *CGXTexture2D::getDXTexture()
{
	return(m_pTexture);
}

void CGXTexture2D::onDevLost()
{
	mem_release(m_pTexture);
	for(UINT i = 0, l = m_apSurfaces.size(); i < l; ++i)
	{
		mem_release(((CGXSurface*)m_apSurfaces[i])->m_pSurface);
	}
}
void CGXTexture2D::onDevRst(UINT uScreenWidth, UINT uScreenHeight)
{
	if(m_bAutoResize)
	{
		m_uWidth = (UINT)((float)uScreenWidth * m_fSizeCoeffW);
		m_uHeight = (UINT)((float)uScreenHeight * m_fSizeCoeffH);
	}
	m_bWasReset = true;
	DX_CALL(m_pRender->getDXDevice()->CreateTexture(m_uWidth, m_uHeight, m_uMipLevels, m_uUsage, m_pRender->getDXFormat(m_format), D3DPOOL_DEFAULT, &m_pTexture, NULL));

	for(UINT i = 0, l = m_apSurfaces.size(); i < l; ++i)
	{
		CGXSurface *pSurf = (CGXSurface*)m_apSurfaces[i];
		pSurf->m_uWidth = max(1, m_uWidth >> pSurf->m_uMipmapNumber);
		pSurf->m_uHeight = max(1, m_uHeight >> pSurf->m_uMipmapNumber);
		DX_CALL(m_pTexture->GetSurfaceLevel(pSurf->m_uMipmapNumber, &(pSurf->m_pSurface)));
	}
}

GXTEXTURE_TYPE CGXTexture2D::getType()
{
	return(GXTEXTURE_TYPE_2D);
}

//##########################################################################

CGXTextureCube::~CGXTextureCube()
{
	mem_release(m_pTexture);
}

void CGXTextureCube::Release()
{
	--m_uRefCount;
	if(!m_uRefCount)
	{
		for(UINT i = 0, l = m_apSurfaces.size(); i < l; ++i)
		{
			mem_release(m_apSurfaces[i]);
		}
		m_pRender->destroyTextureCube(this);
	}
}

IGXSurface *CGXTextureCube::getMipmap(GXCUBEMAP_FACES face, UINT n)
{
	for(UINT i = 0, l = m_apSurfaces.size(); i < l; ++i)
	{
		if(((CGXSurface*)m_apSurfaces[i])->m_uMipmapNumber == n && ((CGXSurface*)m_apSurfaces[i])->m_face == face)
		{
			m_apSurfaces[i]->AddRef();
			return(m_apSurfaces[i]);
		}
	}

	IDirect3DSurface9 *pDXSurface;
	if(!SUCCEEDED(DX_CALL(m_pTexture->GetCubeMapSurface((D3DCUBEMAP_FACES)face, n, &pDXSurface))))
	{
		return(NULL);
	}

	CGXSurface *pSurface = new CGXSurface(m_pRender, max(1, m_uSize >> n), max(1, m_uSize >> n), m_format, pDXSurface);
	pSurface->m_uMipmapNumber = n;
	pSurface->m_face = face;
	m_apSurfaces.push_back(pSurface);
	pSurface->AddRef();
	return(pSurface);
}

UINT CGXTextureCube::getSize()
{
	return(m_uSize);
}

IDirect3DBaseTexture9 *CGXTextureCube::getDXTexture()
{
	return(m_pTexture);
}

bool CGXTextureCube::lock(void **ppData, GXCUBEMAP_FACES face, GXTEXLOCK mode)
{
	CGXContext::debugMessage(GX_LOG_ERROR, "Not implemented: CGXTexture2D::lock");
	return(false);
}

void CGXTextureCube::unlock()
{
	CGXContext::debugMessage(GX_LOG_ERROR, "Not implemented: CGXTexture2D::unlock");
}

GXFORMAT CGXTextureCube::getFormat()
{
	return(m_format);
}

bool CGXTextureCube::wasReset()
{
	return(m_bWasReset);
}

void CGXTextureCube::onDevLost()
{
	mem_release(m_pTexture);
	for(UINT i = 0, l = m_apSurfaces.size(); i < l; ++i)
	{
		mem_release(((CGXSurface*)m_apSurfaces[i])->m_pSurface);
	}
}
void CGXTextureCube::onDevRst(UINT uScreenHeight)
{
	if(m_bAutoResize)
	{
		m_uSize = (UINT)((float)uScreenHeight * m_fSizeCoeff);
	}
	m_bWasReset = true;
	DX_CALL(m_pRender->getDXDevice()->CreateCubeTexture(m_uSize, m_uMipLevels, m_uUsage, m_pRender->getDXFormat(m_format), D3DPOOL_DEFAULT, &m_pTexture, NULL));

	for(UINT i = 0, l = m_apSurfaces.size(); i < l; ++i)
	{
		CGXSurface *pSurf = (CGXSurface*)m_apSurfaces[i];
		pSurf->m_uWidth = pSurf->m_uHeight = max(1, m_uSize >> pSurf->m_uMipmapNumber);

		DX_CALL(m_pTexture->GetCubeMapSurface((D3DCUBEMAP_FACES)(pSurf->m_face), pSurf->m_uMipmapNumber, &(pSurf->m_pSurface)));
	}
}

GXTEXTURE_TYPE CGXTextureCube::getType()
{
	return(GXTEXTURE_TYPE_CUBE);
}
