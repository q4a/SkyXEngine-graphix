#include "GXSurface.h"

void CGXSurface::Release()
{
	--m_uRefCount;
	if(!m_uRefCount)
	{
		delete this;
	}
}

CGXSurface::~CGXSurface()
{
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
	mem_release(m_pSurface);
}
void CGXSurface::onDevRst(UINT uScreenWidth, UINT uScreenHeight)
{
	DX_CALL(m_pRender->getDXDevice()->CreateRenderTarget(m_uWidth, m_uHeight, m_pRender->getDXFormat(m_format), m_multisampleType, 0, FALSE, &m_pSurface, NULL));
}
