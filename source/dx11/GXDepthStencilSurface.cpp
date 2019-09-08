#include "GXDepthStencilSurface.h"


CGXDepthStencilSurface::~CGXDepthStencilSurface()
{
	onDevLost();
	m_pRender->destroyDepthStencilSurface(this);
}

void CGXDepthStencilSurface::onDevLost()
{
	mem_release(m_pBuffer);
	mem_release(m_pSurface);
}
void CGXDepthStencilSurface::onDevRst(UINT uScreenWidth, UINT uScreenHeight)
{
	if(m_bAutoResize)
	{
		
		m_descTex.Width = (UINT)((float)uScreenWidth * m_fSizeCoeffW);
		m_descTex.Height = (UINT)((float)uScreenHeight * m_fSizeCoeffH);

		if(m_descTex.Width < 1)
		{
			m_descTex.Width = 1;
		}
		if(m_descTex.Height < 1)
		{
			m_descTex.Height = 1;
		}
	}

	DX_CALL(m_pRender->getDXDevice()->CreateTexture2D(&m_descTex, NULL, &m_pBuffer));
	DX_CALL(m_pRender->getDXDevice()->CreateDepthStencilView(m_pBuffer, &m_desc, &m_pSurface));
}
