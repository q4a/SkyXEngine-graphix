#include "GXDepthStencilSurface.h"

void CGXDepthStencilSurface::Release()
{
	--m_uRefCount;
	if(!m_uRefCount)
	{
		m_pRender->destroyDepthStencilSurface(this);
	}
}

CGXDepthStencilSurface::~CGXDepthStencilSurface()
{
	mem_release(m_pSurface);
}

void CGXDepthStencilSurface::onDevLost()
{
	mem_release(m_pSurface);
}
void CGXDepthStencilSurface::onDevRst(UINT uScreenWidth, UINT uScreenHeight)
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
	}

	DX_CALL(m_pRender->getDXDevice()->CreateDepthStencilSurface(m_uWidth, m_uHeight, m_format, m_multisampleType, 0, FALSE, &m_pSurface, NULL));
}
