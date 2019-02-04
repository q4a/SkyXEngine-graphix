#ifndef _CGXSwapChain_H_
#define _CGXSwapChain_H_

#include "GXContext.h"

class CGXSwapChain: public IGXSwapChain
{
	friend class CGXContext;

	CGXSwapChain(CGXContext * pRender, int iWidth, int iHeight, SXWINDOW wnd);
	~CGXSwapChain();

	CGXContext * m_pRender;
	IDirect3DSwapChain9 *m_pSwapChain = NULL;
	D3DPRESENT_PARAMETERS m_presentParameters;
	IGXSurface *m_pColorSurface;


	void onDevLost();
	void onDevRst();

public:
	void Release();
	void swapBuffers();
	IGXSurface *getColorTarget();
};

#endif
