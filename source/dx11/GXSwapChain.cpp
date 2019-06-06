#include "GXSwapChain.h"
#include "GXSurface.h"

void CGXSwapChain::onDevLost()
{
	mem_release(((CGXSurface*)m_pColorSurface)->m_pSurface);
	mem_release(((CGXSurface*)m_pColorSurface)->m_pRTV);

	if(m_pSwapChain)
	{
		m_pSwapChain->SetFullscreenState(FALSE, NULL);
	}
	
	mem_release(m_pSwapChain);
}
void CGXSwapChain::onDevRst()
{
	DX_CALL(m_pRender->getDXGIFactory()->CreateSwapChain(m_pRender->getDXDevice(), &m_presentParameters, &m_pSwapChain));

	ID3D11Texture2D* pBackBuffer = NULL;
	DX_CALL(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer));
	ID3D11RenderTargetView* pRenderTargetView = NULL;
	DX_CALL(m_pRender->getDXDevice()->CreateRenderTargetView(pBackBuffer, NULL, &pRenderTargetView));

	CGXSurface *pSurface = (CGXSurface*)m_pColorSurface;
	pSurface->m_pSurface = pBackBuffer;
	pSurface->m_pRTV = pRenderTargetView;
}

void CGXSwapChain::resize(int iWidth, int iHeight, bool isWindowed)
{
	if(iWidth < 1)
	{
		iWidth = 1;
	}
	if(iHeight < 1)
	{
		iHeight = 1;
	}

	m_presentParameters.BufferDesc.Width = iWidth;
	m_presentParameters.BufferDesc.Height = iHeight;
	m_presentParameters.Windowed = isWindowed;
	CGXSurface *pSurface = (CGXSurface*)m_pColorSurface;

	pSurface->m_uWidth = iWidth;
	pSurface->m_uHeight = iHeight;

	onDevLost();
	onDevRst();
}

CGXSwapChain::CGXSwapChain(CGXContext *pRender, int iWidth, int iHeight, SXWINDOW wnd, bool isWindowed):m_pRender(pRender)
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

	m_presentParameters.BufferCount = 1;
	m_presentParameters.BufferDesc.Width = iWidth;
	m_presentParameters.BufferDesc.Height = iHeight;
	m_presentParameters.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	m_presentParameters.BufferDesc.RefreshRate.Numerator = 0;
	m_presentParameters.BufferDesc.RefreshRate.Denominator = 1;
	m_presentParameters.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	m_presentParameters.OutputWindow = (HWND)wnd;
	m_presentParameters.SampleDesc.Count = 1;
	m_presentParameters.SampleDesc.Quality = 0;
	m_presentParameters.Windowed = isWindowed;



	m_pColorSurface = new CGXSurface(m_pRender, m_presentParameters.BufferDesc.Width, m_presentParameters.BufferDesc.Height, m_pRender->getGXFormat(m_presentParameters.BufferDesc.Format), NULL);
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
		DX_CALL(m_pSwapChain->Present(NULL, NULL));
	}
}
IGXSurface* CGXSwapChain::getColorTarget()
{
	m_pColorSurface->AddRef();
	return(m_pColorSurface);
}
