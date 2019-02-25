#ifndef _CGXSwapChain_H_
#define _CGXSwapChain_H_

#include "GXContext.h"

class CGXSwapChain: public IGXSwapChain
{
	friend class CGXContext;

	CGXSwapChain(CGXContext * pRender, int iWidth, int iHeight, SXWINDOW wnd, bool isFullscreen=false);
	~CGXSwapChain();

	CGXContext * m_pRender;
	IDXGISwapChain *m_pSwapChain = NULL;
	DXGI_SWAP_CHAIN_DESC m_presentParameters;
	IGXSurface *m_pColorSurface;


	void onDevLost();
	void onDevRst();

public:
	void Release();
	void swapBuffers();
	IGXSurface *getColorTarget();
};

#endif
