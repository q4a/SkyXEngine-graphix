#include "GXSurface.h"

void CGXSurface::Release()
{
	--m_uRefCount;
	if(!m_uRefCount)
	{
		m_pRender->destroyColorTarget(this);
	}
}

CGXSurface::~CGXSurface()
{
	releaseRT();
	mem_release(m_pSurface);
}

UINT CGXSurface::getWidth()
{
	return(m_uWidth);
}
UINT CGXSurface::getHeight()
{
	return(m_uHeight);
}
GXFORMAT CGXSurface::getFormat()
{
	return(m_format);
}

void CGXSurface::onDevLost()
{
	releaseRT();
	mem_release(m_pSurface);
}
void CGXSurface::onDevRst(UINT uScreenWidth, UINT uScreenHeight)
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
	}

	DX_CALL(m_pRender->getDXDevice()->CreateTexture2D(&m_descTex2D, NULL, &m_pSurface));

	initRT();
}

void CGXSurface::initRT()
{
	memset(&m_descRTV, 0, sizeof(m_descRTV));
	m_descRTV.Format = m_descTex2D.Format;
	m_descRTV.ViewDimension = m_descTex2D.SampleDesc.Count > 1 ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
	m_descRTV.Texture2D.MipSlice = D3D11CalcSubresource(m_uMipmapNumber, m_face, m_uMipmapTotal);
	DX_CALL(m_pRender->getDXDevice()->CreateRenderTargetView(m_pSurface, &m_descRTV, &m_pRTV));
}

void CGXSurface::releaseRT()
{
	mem_release(m_pRTV);
}
