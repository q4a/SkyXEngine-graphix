#ifndef _CGXSwapChain_H_
#define _CGXSwapChain_H_

#include "GXDevice.h"

class CGXSwapChain: public IGXSwapChain
{
	friend class CGXDevice;

	CGXSwapChain(CGXDevice *pRender, int iWidth, int iHeight, SXWINDOW wnd, bool isWindowed = true);
	~CGXSwapChain();

	CGXDevice *m_pRender;
	IDXGISwapChain *m_pSwapChain = NULL;
	DXGI_SWAP_CHAIN_DESC m_presentParameters;
	IGXSurface *m_pColorSurface;


	void onDevLost();
	void onDevRst();

	void resize(int iWidth, int iHeight, bool isWindowed = true);

public:
	void swapBuffers() override;
	IGXSurface* getColorTarget() override;
};

#endif
