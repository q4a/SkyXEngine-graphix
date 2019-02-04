#include "GXSwapChain.h"
#include "GXSurface.h"

void CGXSwapChain::onDevLost()
{
	mem_release(((CGXSurface*)m_pColorSurface)->m_pSurface);
	mem_release(m_pSwapChain);
}
void CGXSwapChain::onDevRst()
{
	DX_CALL(m_pRender->getDXDevice()->CreateAdditionalSwapChain(&m_presentParameters, &m_pSwapChain));
	DX_CALL(m_pSwapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &(((CGXSurface*)m_pColorSurface)->m_pSurface)));
}

CGXSwapChain::CGXSwapChain(CGXContext * pRender, int iWidth, int iHeight, SXWINDOW wnd):m_pRender(pRender)
{
	memset(&m_presentParameters, 0, sizeof(m_presentParameters));

	if(iWidth < 1)
	{
		iWidth = 1;
	}
	if(iHeight < 1)
	{
		iHeight = 1;
	}
	
	m_presentParameters.BackBufferWidth = iWidth;
	m_presentParameters.BackBufferHeight = iHeight;
	m_presentParameters.BackBufferFormat = D3DFMT_A8R8G8B8;
	m_presentParameters.BackBufferCount = 1;
	m_presentParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
	m_presentParameters.MultiSampleQuality = 0;
	m_presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_presentParameters.hDeviceWindow = (HWND)wnd;
	m_presentParameters.Windowed = 1;
	m_presentParameters.EnableAutoDepthStencil = 0;
//	m_presentParameters.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	m_presentParameters.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	m_presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	m_pColorSurface = new CGXSurface(m_pRender, m_presentParameters.BackBufferWidth, m_presentParameters.BackBufferHeight, m_pRender->getGXFormat(m_presentParameters.BackBufferFormat), NULL);
	onDevRst();
}

CGXSwapChain::~CGXSwapChain()
{
	onDevLost();
	mem_release(m_pColorSurface);
}

void CGXSwapChain::Release()
{
	--m_uRefCount;
	if(!m_uRefCount)
	{
		m_pRender->destroySwapChain(this);
	}
}
void CGXSwapChain::swapBuffers()
{
	if(m_pSwapChain)
	{
		m_pSwapChain->Present(NULL, NULL, NULL, NULL, NULL);
	}
}
IGXSurface *CGXSwapChain::getColorTarget()
{
	m_pColorSurface->AddRef();
	return(m_pColorSurface);
}
