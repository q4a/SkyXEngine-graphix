#include "GXTexture.h"
#include "GXSurface.h"

CGXTexture2D::~CGXTexture2D()
{
	mem_release(m_pTexture);
	mem_release(m_pSRV);
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
	
	CGXSurface *pSurface = new CGXSurface(m_pRender, max(1, m_uWidth >> n), max(1, m_uHeight >> n), m_format, m_pTexture);
	pSurface->m_uMipmapNumber = n;
	if(m_descTex2D.BindFlags & D3D11_BIND_RENDER_TARGET)
	{
		pSurface->initRT();
	}
	m_apSurfaces.push_back(pSurface);
	pSurface->AddRef();
	return(pSurface);
}

bool CGXTexture2D::lock(void **ppData, GXTEXLOCK mode)
{
	D3D11_MAPPED_SUBRESOURCE sr;
	if(FAILED(DX_CALL(m_pRender->getDXDeviceContext()->Map(m_pTexture, 0, (mode == GXTL_WRITE) ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_READ, 0, &sr))))
	{
		return(false);
	}

	m_pRender->addBytesTextures(sr.RowPitch * m_uHeight);

	*ppData = sr.pData;
	return(true);
}

void CGXTexture2D::unlock()
{
	m_pRender->getDXDeviceContext()->Unmap(m_pTexture, 0);
}

void CGXTexture2D::update(void *pData)
{
	m_pRender->getDXDeviceContext()->UpdateSubresource(m_pTexture, 0, NULL, pData, m_pRender->getTextureMemPitch(m_uWidth, m_format), 0);
}

ID3D11ShaderResourceView *CGXTexture2D::getDXTexture()
{
	return(m_pSRV);
}

void CGXTexture2D::onDevLost()
{
	mem_release(m_pTexture);
	mem_release(m_pSRV);
	for(UINT i = 0, l = m_apSurfaces.size(); i < l; ++i)
	{
		mem_release(((CGXSurface*)m_apSurfaces[i])->m_pSurface);
		if(m_descTex2D.BindFlags | D3D11_BIND_RENDER_TARGET)
		{
			((CGXSurface*)m_apSurfaces[i])->releaseRT();
		}
	}
}
void CGXTexture2D::onDevRst(UINT uScreenWidth, UINT uScreenHeight)
{
	if(m_bAutoResize)
	{
		m_uWidth = (UINT)((float)uScreenWidth * m_fSizeCoeffW);
		m_uHeight = (UINT)((float)uScreenHeight * m_fSizeCoeffH);
		if(m_uWidth < 1)
		{
			m_uWidth = 1;
		}
		if(m_uHeight < 1)
		{
			m_uHeight = 1;
		}
		m_descTex2D.Width = m_uWidth;
		m_descTex2D.Height = m_uHeight;

		if(!m_descTex2D.MipLevels)
		{
			m_descSRV.Texture2D.MipLevels = 0;
			UINT uSize = max(m_uHeight, m_uWidth);
			do
			{
				++m_descSRV.Texture2D.MipLevels;
			}
			while(uSize >>= 1);
		}
	}

	m_bWasReset = true;
	DX_CALL(m_pRender->getDXDevice()->CreateTexture2D(&m_descTex2D, NULL, &m_pTexture));
	DX_CALL(m_pRender->getDXDevice()->CreateShaderResourceView(m_pTexture, &m_descSRV, &m_pSRV));

	for(UINT i = 0, l = m_apSurfaces.size(); i < l; ++i)
	{
		CGXSurface *pSurf = (CGXSurface*)m_apSurfaces[i];
		pSurf->m_uWidth = max(1, m_uWidth >> pSurf->m_uMipmapNumber);
		pSurf->m_uHeight = max(1, m_uHeight >> pSurf->m_uMipmapNumber);
		pSurf->m_pSurface = m_pTexture;
		m_pTexture->AddRef();

		if(m_descTex2D.BindFlags | D3D11_BIND_RENDER_TARGET)
		{
			((CGXSurface*)m_apSurfaces[i])->initRT();
		}
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
	mem_release(m_pSRV);
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
	
	CGXSurface *pSurface = new CGXSurface(m_pRender, max(1, m_uSize >> n), max(1, m_uSize >> n), m_format, m_pTexture);
	pSurface->m_uMipmapNumber = n;
	pSurface->m_uMipmapTotal = m_descTex2D.MipLevels;
	pSurface->m_face = face;
	if(m_descTex2D.BindFlags | D3D11_BIND_RENDER_TARGET)
	{
		pSurface->initRT();
	}
	m_apSurfaces.push_back(pSurface);
	pSurface->AddRef();
	return(pSurface);
}

UINT CGXTextureCube::getSize()
{
	return(m_uSize);
}

ID3D11ShaderResourceView *CGXTextureCube::getDXTexture()
{
	return(m_pSRV);
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
	mem_release(m_pSRV);
	for(UINT i = 0, l = m_apSurfaces.size(); i < l; ++i)
	{
		mem_release(((CGXSurface*)m_apSurfaces[i])->m_pSurface);
		if(m_descTex2D.BindFlags | D3D11_BIND_RENDER_TARGET)
		{
			((CGXSurface*)m_apSurfaces[i])->releaseRT();
		}
	}
}
void CGXTextureCube::onDevRst(UINT uScreenHeight)
{
	if(m_bAutoResize)
	{
		m_uSize = (UINT)((float)uScreenHeight * m_fSizeCoeff);
		if(m_uSize < 1)
		{
			m_uSize = 1;
		}
		m_descTex2D.Width = m_uSize;
		m_descTex2D.Height = m_uSize;
	}
	m_bWasReset = true;

	DX_CALL(m_pRender->getDXDevice()->CreateTexture2D(&m_descTex2D, NULL, &m_pTexture));
	DX_CALL(m_pRender->getDXDevice()->CreateShaderResourceView(m_pTexture, &m_descSRV, &m_pSRV));

	for(UINT i = 0, l = m_apSurfaces.size(); i < l; ++i)
	{
		CGXSurface *pSurf = (CGXSurface*)m_apSurfaces[i];
		pSurf->m_uWidth = pSurf->m_uHeight = max(1, m_uSize >> pSurf->m_uMipmapNumber);
		pSurf->m_pSurface = m_pTexture;
		m_pTexture->AddRef();

		if(m_descTex2D.BindFlags | D3D11_BIND_RENDER_TARGET)
		{
			((CGXSurface*)m_apSurfaces[i])->initRT();
		}
	}
}

GXTEXTURE_TYPE CGXTextureCube::getType()
{
	return(GXTEXTURE_TYPE_CUBE);
}
