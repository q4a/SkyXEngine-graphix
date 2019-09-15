#include "GXTexture.h"
#include "GXSurface.h"
#include "GXDevice.h"
#include "GXContext.h"

CGXTexture2D::~CGXTexture2D()
{
	for(UINT i = 0, l = m_apSurfaces.size(); i < l; ++i)
	{
		mem_release(m_apSurfaces[i]);
	}

	//m_pRender->addBytesTextures(-(int)m_pRender->getTextureMemPitch(m_uWidth, m_format) * m_uHeight, true, m_descTex2D.BindFlags & D3D11_BIND_RENDER_TARGET);
	m_pRender->addBytesTextures(-m_iTotalSize, !!(m_descTex2D.BindFlags & D3D11_BIND_RENDER_TARGET));
	mem_release(m_pTexture);
	mem_release(m_pSRV);
	mem_release(m_pUAV);

	m_pRender->destroyTexture2D(this);
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

IGXSurface* CGXTexture2D::getMipmap(UINT n)
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
	pSurface->m_descTex2D.SampleDesc = m_descTex2D.SampleDesc;
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
	if(FAILED(DX_CALL(((CGXContext*)m_pRender->getThreadContext())->getDXDeviceContext()->Map(m_pTexture, 0, (mode == GXTL_WRITE) ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_READ, 0, &sr))))
	{
		return(false);
	}

	((CGXContext*)m_pRender->getThreadContext())->addBytesTextures(sr.RowPitch * m_uHeight);

	*ppData = sr.pData;
	return(true);
}

void CGXTexture2D::unlock()
{
	((CGXContext*)m_pRender->getThreadContext())->getDXDeviceContext()->Unmap(m_pTexture, 0);
}

void CGXTexture2D::update(void *pData)
{
	((CGXContext*)m_pRender->getThreadContext())->getDXDeviceContext()->UpdateSubresource(m_pTexture, 0, NULL, pData, m_pRender->getTextureMemPitch(m_uWidth, m_format), 0);
}

ID3D11ShaderResourceView* CGXTexture2D::getDXTexture()
{
	return(m_pSRV);
}

void CGXTexture2D::onDevLost()
{
	mem_release(m_pTexture);
	mem_release(m_pSRV);
	mem_release(m_pUAV);
	for(UINT i = 0, l = m_apSurfaces.size(); i < l; ++i)
	{
		mem_release(((CGXSurface*)m_apSurfaces[i])->m_pSurface);
		if(m_descTex2D.BindFlags & D3D11_BIND_RENDER_TARGET)
		{
			((CGXSurface*)m_apSurfaces[i])->releaseRT();
		}
	}

	m_pRender->addBytesTextures(-(int)m_pRender->getTextureMemPitch(m_uWidth, m_format) * m_uHeight, !!(m_descTex2D.BindFlags & D3D11_BIND_RENDER_TARGET));
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
	if(m_descTex2D.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
	{
		DX_CALL(m_pRender->getDXDevice()->CreateUnorderedAccessView(m_pTexture, &m_descUAV, &m_pUAV));
	}

	m_pRender->addBytesTextures(m_pRender->getTextureMemPitch(m_uWidth, m_format) * m_uHeight, !!(m_descTex2D.BindFlags & D3D11_BIND_RENDER_TARGET));

	for(UINT i = 0, l = m_apSurfaces.size(); i < l; ++i)
	{
		CGXSurface *pSurf = (CGXSurface*)m_apSurfaces[i];
		pSurf->m_uWidth = max(1, m_uWidth >> pSurf->m_uMipmapNumber);
		pSurf->m_uHeight = max(1, m_uHeight >> pSurf->m_uMipmapNumber);
		pSurf->m_pSurface = m_pTexture;
		m_pTexture->AddRef();

		if(m_descTex2D.BindFlags & D3D11_BIND_RENDER_TARGET)
		{
			((CGXSurface*)m_apSurfaces[i])->initRT();
		}
	}
}

GXTEXTURE_TYPE CGXTexture2D::getType()
{
	return(GXTEXTURE_TYPE_2D);
}

IGXSurface* CGXTexture2D::asRenderTarget()
{
	if(!(m_descTex2D.BindFlags & D3D11_BIND_RENDER_TARGET))
	{
		m_pRender->debugMessage(GX_LOG_ERROR, "Unable to use CGXTexture2D::asRenderTarget() on texture created without GX_TEXFLAG_RENDERTARGET flag!");
		return(NULL);
	}
	
	return(getMipmap());
}

//##########################################################################

CGXTexture3D::~CGXTexture3D()
{
	m_pRender->addBytesTextures(-(int)m_pRender->getTextureMemPitch(m_uDepth, m_format) * m_uWidth * m_uHeight, !!(m_descTex3D.BindFlags & D3D11_BIND_RENDER_TARGET));
	mem_release(m_pTexture);
	mem_release(m_pSRV);
	mem_release(m_pSurfaceRT);
	mem_release(m_pUAV);
}

GXFORMAT CGXTexture3D::getFormat()
{
	return(m_format);
}

bool CGXTexture3D::wasReset()
{
	return(m_bWasReset);
}

UINT CGXTexture3D::getWidth()
{
	return(m_uWidth);
}

UINT CGXTexture3D::getHeight()
{
	return(m_uHeight);
}

UINT CGXTexture3D::getDepth()
{
	return(m_uDepth);
}

IGXSurface* CGXTexture3D::asRenderTarget()
{
	if(!(m_descTex3D.BindFlags & D3D11_BIND_RENDER_TARGET))
	{
		m_pRender->debugMessage(GX_LOG_ERROR, "Unable to use CGXTexture3D::asRenderTarget() on texture created without GX_TEXFLAG_RENDERTARGET flag!");
		return(NULL);
	}
	if(m_pSurfaceRT)
	{
		m_pSurfaceRT->AddRef();
		return(m_pSurfaceRT);
	}

	CGXSurface3D *pSurface = new CGXSurface3D(m_pRender, m_uWidth, m_uHeight, m_uDepth, m_format, m_pTexture);
	if(m_descTex3D.BindFlags & D3D11_BIND_RENDER_TARGET)
	{
		pSurface->initRT();
	}
	m_pSurfaceRT = pSurface;
	pSurface->AddRef();
	return(pSurface);
}

void CGXTexture3D::update(void *pData)
{
	m_pRender->addBytesTextures(m_pRender->getTextureMemPitch(m_uDepth, m_format) * m_uWidth * m_uHeight);
	m_pRender->getDXDeviceContext()->UpdateSubresource(m_pTexture, 0, NULL, pData, m_pRender->getTextureMemPitch(m_uWidth, m_format), m_pRender->getTextureMemPitch(m_uDepth, m_format) * m_uWidth);
}

ID3D11ShaderResourceView* CGXTexture3D::getDXTexture()
{
	return(m_pSRV);
}

GXTEXTURE_TYPE CGXTexture3D::getType()
{
	return(GXTEXTURE_TYPE_3D);
}

//##########################################################################

CGXTextureCube::~CGXTextureCube()
{
	m_pRender->addBytesTextures(-m_iTotalSize, !!(m_descTex2D.BindFlags & D3D11_BIND_RENDER_TARGET));
	//m_pRender->addBytesTextures(-(int)m_pRender->getTextureMemPitch(m_uSize, m_format) * m_uSize * 6, true, m_descTex2D.BindFlags & D3D11_BIND_RENDER_TARGET);
	mem_release(m_pTexture);
	mem_release(m_pSRV);
	mem_release(m_pSurfaceRT);
	mem_release(m_pUAV);

	for(UINT i = 0, l = m_apSurfaces.size(); i < l; ++i)
	{
		mem_release(m_apSurfaces[i]);
	}
	m_pRender->destroyTextureCube(this);
}

IGXSurface* CGXTextureCube::getMipmap(GXCUBEMAP_FACES face, UINT n)
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
	if(m_descTex2D.BindFlags & D3D11_BIND_RENDER_TARGET)
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

ID3D11ShaderResourceView* CGXTextureCube::getDXTexture()
{
	return(m_pSRV);
}

bool CGXTextureCube::lock(void **ppData, GXCUBEMAP_FACES face, GXTEXLOCK mode)
{
	CGXDevice::debugMessage(GX_LOG_ERROR, "Not implemented: CGXTexture2D::lock");
	return(false);
}

void CGXTextureCube::unlock()
{
	CGXDevice::debugMessage(GX_LOG_ERROR, "Not implemented: CGXTexture2D::unlock");
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
	mem_release(m_pUAV);
	for(UINT i = 0, l = m_apSurfaces.size(); i < l; ++i)
	{
		mem_release(((CGXSurface*)m_apSurfaces[i])->m_pSurface);
		if(m_descTex2D.BindFlags & D3D11_BIND_RENDER_TARGET)
		{
			((CGXSurface*)m_apSurfaces[i])->releaseRT();
		}
	}
	m_pRender->addBytesTextures(-(int)m_pRender->getTextureMemPitch(m_uSize, m_format) * m_uSize * 6, !!(m_descTex2D.BindFlags & D3D11_BIND_RENDER_TARGET));
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
	if(m_descTex2D.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
	{
		DX_CALL(m_pRender->getDXDevice()->CreateUnorderedAccessView(m_pTexture, &m_descUAV, &m_pUAV));
	}

	m_pRender->addBytesTextures(m_pRender->getTextureMemPitch(m_uSize, m_format) * m_uSize * 6, !!(m_descTex2D.BindFlags & D3D11_BIND_RENDER_TARGET));

	for(UINT i = 0, l = m_apSurfaces.size(); i < l; ++i)
	{
		CGXSurface *pSurf = (CGXSurface*)m_apSurfaces[i];
		pSurf->m_uWidth = pSurf->m_uHeight = max(1, m_uSize >> pSurf->m_uMipmapNumber);
		pSurf->m_pSurface = m_pTexture;
		m_pTexture->AddRef();

		if(m_descTex2D.BindFlags & D3D11_BIND_RENDER_TARGET)
		{
			((CGXSurface*)m_apSurfaces[i])->initRT();
		}
	}
}

GXTEXTURE_TYPE CGXTextureCube::getType()
{
	return(GXTEXTURE_TYPE_CUBE);
}

IGXSurface* CGXTextureCube::asRenderTarget()
{
	if(!(m_descTex2D.BindFlags & D3D11_BIND_RENDER_TARGET))
	{
		m_pRender->debugMessage(GX_LOG_ERROR, "Unable to use CGXTextureCube::asRenderTarget() on texture created without GX_TEXFLAG_RENDERTARGET flag!");
		return(NULL);
	}
	if(m_pSurfaceRT)
	{
		m_pSurfaceRT->AddRef();
		return(m_pSurfaceRT);
	}

	CGXSurfaceCube *pSurface = new CGXSurfaceCube(m_pRender, m_uSize, m_format, m_pTexture);
	if(m_descTex2D.BindFlags & D3D11_BIND_RENDER_TARGET)
	{
		pSurface->initRT();
	}
	m_pSurfaceRT = pSurface;
	pSurface->AddRef();
	return(pSurface);
}
