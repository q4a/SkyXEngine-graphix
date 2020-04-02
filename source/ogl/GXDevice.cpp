#include "GXDevice.h"

#include "GXIndexBuffer.h"
#include "GXVertexBuffer.h"
#include "GXVertexDeclaration.h"
#include "GXShader.h"
#include "GXRenderBuffer.h"
//#include "GXSamplerState.h"
//#include "GXRasterizerState.h"
//#include "GXDepthStencilState.h"
//#include "GXDepthStencilSurface.h"
//#include "GXSurface.h"
#include "GXBlendState.h"
//#include "GXTexture.h"
//#include "GXSwapChain.h"
//#include "GXConstantBuffer.h"

#include "GXContext.h"

#include <cstdio>
#include <mutex>

//##########################################################################

class CThreadsafeCounter
{
	ID m_idCounter;
	std::mutex m_mutex;

public:
	CThreadsafeCounter()
	{
		m_idCounter = -1;
	}

	ID getNext()
	{
		m_mutex.lock();
		++m_idCounter;

		if(m_idCounter >= GX_MAX_THREADS)
		{
			assert(!"thread counter exceeded");
			// wrap back to the first worker index
			m_idCounter = 1;
		}

		ID val = m_idCounter;
		m_mutex.unlock();
		return(val);
	}
};

static ID GXGetThreadID()
{
	static CThreadsafeCounter s_threadCounter;

	const ID c_idNullIndex = -1;
	__declspec(thread) static ID s_idThreadIndex = c_idNullIndex;
	if(s_idThreadIndex == c_idNullIndex)
	{
		s_idThreadIndex = s_threadCounter.getNext();
		assert(s_idThreadIndex < GX_MAX_THREADS);
	}

	return(s_idThreadIndex);
}

//##########################################################################

//флаги компиляции шейдеров
#ifdef _DEBUG
/* D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY */
#	define SHADER_FLAGS (D3DCOMPILE_DEBUG | D3DCOMPILE_AVOID_FLOW_CONTROL | D3DCOMPILE_SKIP_OPTIMIZATION | /*D3DCOMPILE_PARTIAL_PRECISION | */D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY)
//#	define SHADER_FLAGS (D3DXSHADER_DEBUG | D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY | D3DXSHADER_AVOID_FLOW_CONTROL | D3DXSHADER_SKIPOPTIMIZATION)
#else
//#	define SHADER_FLAGS (D3DXSHADER_OPTIMIZATION_LEVEL3 | D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY | D3DXSHADER_PARTIALPRECISION | D3DXSHADER_PREFER_FLOW_CONTROL)
#	define SHADER_FLAGS (D3DCOMPILE_OPTIMIZATION_LEVEL3 | /*D3DCOMPILE_PARTIAL_PRECISION | */D3DCOMPILE_PREFER_FLOW_CONTROL | D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY) 
#endif

CGXDevice *CGXDevice::ms_pInstance = NULL;

CGXDevice::CGXDevice()
{
	if(ms_pInstance)
	{
		ms_pInstance->log(GX_LOG_ERROR, "Cannot init second instance!\n");
	}
	ms_pInstance = this;
	memset(&m_memoryStats, 0, sizeof(m_memoryStats));
	memset(&m_adapterDesc, 0, sizeof(m_adapterDesc));
	memset(&m_pContexts, 0, sizeof(m_pContexts));
}

CGXDevice::~CGXDevice()
{
	mem_release(m_pDefaultDepthStencilSurface);
	mem_release(m_pDefaultRasterizerState);
	mem_release(m_pDefaultSamplerState);
	mem_release(m_pDefaultBlendState);
	mem_release(m_pDefaultDepthStencilState);

	ms_pInstance = NULL;

	for(int i = 1; i <= m_iLastIndirectContext; ++i)
	{
		mem_release(m_pContexts[i]);
	}

	//mem_release(m_pDeviceContext);
	mem_delete(m_pDirectContext);
	mem_release(m_pDefaultSwapChain);

	//mem_release(m_pDXGIFactory);
	//mem_release(m_pDXGIDevice);
	mem_release(m_pDevice);
}

void CGXDevice::Release()
{
	delete this;
}

void CGXDevice::setLogger(IGXLogger *pLogger)
{
	m_pLogger = pLogger;
}

void CGXDevice::onLostDevice()
{
	/*for(UINT i = 0, l = m_aResettableDSSurfaces.size(); i < l; ++i)
	{
		((CGXDepthStencilSurface*)m_aResettableDSSurfaces[i])->onDevLost();
	}
	for(UINT i = 0, l = m_aResettableColorSurfaces.size(); i < l; ++i)
	{
		((CGXSurface*)m_aResettableColorSurfaces[i])->onDevLost();
	}
	for(UINT i = 0, l = m_aResettableTextures2D.size(); i < l; ++i)
	{
		dynamic_cast<CGXTexture2D*>(m_aResettableTextures2D[i])->onDevLost();
	}
	for(UINT i = 0, l = m_aResettableTexturesCube.size(); i < l; ++i)
	{
		dynamic_cast<CGXTextureCube*>(m_aResettableTexturesCube[i])->onDevLost();
	}
	/*for(UINT i = 0, l = m_aResettableSwapChains.size(); i < l; ++i)
	{
		((CGXSwapChain*)m_aResettableSwapChains[i])->onDevLost();
	}*/
}

void CGXDevice::onResetDevice()
{
	/*for(UINT i = 0, l = m_aResettableDSSurfaces.size(); i < l; ++i)
	{
		((CGXDepthStencilSurface*)m_aResettableDSSurfaces[i])->onDevRst(m_uWindowWidth, m_uWindowHeight);
	}
	for(UINT i = 0, l = m_aResettableColorSurfaces.size(); i < l; ++i)
	{
		((CGXSurface*)m_aResettableColorSurfaces[i])->onDevRst(m_uWindowWidth, m_uWindowHeight);
	}
	for(UINT i = 0, l = m_aResettableTextures2D.size(); i < l; ++i)
	{
		dynamic_cast<CGXTexture2D*>(m_aResettableTextures2D[i])->onDevRst(m_uWindowWidth, m_uWindowHeight);
	}
	for(UINT i = 0, l = m_aResettableTexturesCube.size(); i < l; ++i)
	{
		dynamic_cast<CGXTextureCube*>(m_aResettableTexturesCube[i])->onDevRst(m_uWindowHeight);
	}*/
}

void CGXDevice::resize(int iWidth, int iHeight, bool isWindowed)
{
	/*if(iWidth < 1)
	{
		iWidth = 1;
	}
	if(iHeight < 1)
	{
		iHeight = 1;
	}
	m_uWindowWidth = iWidth;
	m_uWindowHeight = iHeight;
	m_isWindowed = isWindowed;

	onLostDevice();
//	m_pDevice->Reset(&m_oD3DAPP);
	onResetDevice();

	((CGXSwapChain*)m_pDefaultSwapChain)->resize(iWidth, iHeight, isWindowed);*/
}

BOOL CGXDevice::initContext(SXWINDOW wnd, int iWidth, int iHeight, bool isWindowed)
{
	m_hWnd = (HWND)wnd;
#if defined(_WINDOWS)
	m_hDC = GetDC((HWND)m_hWnd);

	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;
	int iFormat = ChoosePixelFormat(m_hDC, &pfd);
	SetPixelFormat(m_hDC, iFormat, &pfd);

	m_hRC = wglCreateContext(m_hDC);

	wglMakeCurrent(m_hDC, m_hRC);

	//glClearColor(0, 16.0f / 255.0f, 32.0f / 255.0f, 0);
	//glClear(GL_COLOR_BUFFER_BIT);

	//SwapBuffers();

	m_pGL = new IDSRGLPFN();

	//wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

	if(m_pGL->wglSwapIntervalEXT)
	{
		m_pGL->wglSwapIntervalEXT(0); ///< disables vsync
	}
	
	return(TRUE);
	//
#else
	//GtkWidget * da = gtk_drawing_area_new();

	//gtk_container_set_border_width(GTK_CONTAINER(wnd), 100);
	//gtk_container_add(GTK_CONTAINER(wnd), da);
	//gtk_widget_set_double_buffered(GTK_WIDGET(da), FALSE);
	//gtk_widget_show(da);

	GdkDisplay * pGtkDisplay = gtk_widget_get_display(GTK_WIDGET(wnd));
	Display * pDisplay = gdk_x11_display_get_xdisplay(pGtkDisplay);
	//GdkVisual * visual;

	GLint attribs[] = {GLX_RGBA, GLX_RED_SIZE, 8, GLX_GREEN_SIZE, 8, GLX_BLUE_SIZE, 8, GLX_DEPTH_SIZE, 24, GLX_STENCIL_SIZE, 8, GLX_DOUBLEBUFFER, None};
	XVisualInfo * xvinfo = glXChooseVisual(pDisplay, XDefaultScreen(pDisplay),  attribs);

	GdkWindow * gdk_window = gtk_widget_get_window(GTK_WIDGET(wnd));

	if(!xvinfo)
	{
		return(FALSE);
	}

	//visual = gdkx_visual_get(xvinfo->visualid);

	GLXContext ctx = glXCreateContext(pDisplay, xvinfo, 0, True);

	if(ctx == NULL)
	{
		return(FALSE);
	}

	glXMakeCurrent(pDisplay, gdk_x11_window_get_xid(gdk_window), ctx);

	XFree(xvinfo);

	//GtkWidget * pGLarea = gtk_gl_area_new();
	//gtk_container_add(GTK_CONTAINER(wnd), pGLarea);
	//gtk_widget_show(pGLarea);

	//g_signal_connect(pGLarea, "render", G_CALLBACK(render), NULL);

	//printf("e: %s\n", gtk_gl_area_get_error((GtkGLArea*)(pGLarea))->message);


//glXCreateContext()


	printf("Version: %s\n", glGetString(GL_VERSION));
	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Exts: %s\n", glGetString(GL_EXTENSIONS));
	//

	glClearColor(0, 16.0f / 255.0f, 32.0f / 255.0f, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glXSwapBuffers(pDisplay, gdk_x11_window_get_xid(gdk_window));

	log(GX_LOG_INFO, "Context ready!\n\n");
	return(TRUE);
#endif
	return(FALSE);
	/*DX11
	m_hWnd = (HWND)wnd;

	assert(GXGetThreadID() == 0);

	UINT creationFlags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#if defined(_DEBUG)
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	log(GX_LOG_INFO, "Initializing %s context\n", (creationFlags & D3D11_CREATE_DEVICE_DEBUG) ? "debug" : "release");
	D3D_FEATURE_LEVEL featureLevels111[] =
	{
		D3D_FEATURE_LEVEL_11_1
	};
	if(FAILED(DX_CALL(D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, featureLevels111, ARRAYSIZE(featureLevels111), D3D11_SDK_VERSION, &m_pDevice, nullptr, &m_pDeviceContext))))
	{
		log(GX_LOG_INFO, "Unable to use DirectX11.1! Falling back to previous version\n");

		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1
		};
		log(GX_LOG_INFO, "Initializing %s context\n", (creationFlags & D3D11_CREATE_DEVICE_DEBUG) ? "debug" : "release");
		if(FAILED(DX_CALL(D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &m_pDevice, nullptr, &m_pDeviceContext))))
		{
			return(FALSE);
		}
	}
	

	if(FAILED(DX_CALL(m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&m_pDXGIDevice))))
	{
		return(FALSE);
	}

	IDXGIAdapter * pDXGIAdapter = NULL;
	if(FAILED(DX_CALL(m_pDXGIDevice->GetAdapter(&pDXGIAdapter))))
	{
		return(FALSE);
	}


	if(FAILED(DX_CALL(pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&m_pDXGIFactory))))
	{
		return(FALSE);
	}

	DXGI_ADAPTER_DESC adapterDesc;
	if(!FAILED(DX_CALL(pDXGIAdapter->GetDesc(&adapterDesc))))
	{
		lstrcpyW(m_adapterDesc.szDescription, adapterDesc.Description);
		m_adapterDesc.sizeTotalMemory = adapterDesc.DedicatedVideoMemory;
	}
	else
	{
		lstrcpyW(m_adapterDesc.szDescription, L"Unknown device");
	}

	log(GX_LOG_INFO, "Adapter: %S\n", m_adapterDesc.szDescription);
	log(GX_LOG_INFO, "Available video memory: %uMB\n", (UINT)(m_adapterDesc.sizeTotalMemory / 1024 / 1024));

	mem_release(pDXGIAdapter);

	m_uWindowWidth = iWidth;
	m_uWindowHeight = iHeight;
	m_isWindowed = isWindowed;


	m_pDefaultDepthStencilSurface = createDepthStencilSurface(m_uWindowWidth, m_uWindowHeight, GXFMT_D24S8, GXMULTISAMPLE_NONE, true);

	GXSamplerDesc samplerDesc;
	memset(&samplerDesc, 0, sizeof(samplerDesc));
	samplerDesc.uMaxAnisotropy = 1;
	m_pDefaultSamplerState = createSamplerState(&samplerDesc);
	m_pDeviceContext->VSSetSamplers(0, 1, &((CGXSamplerState*)m_pDefaultSamplerState)->m_pStateBlock);


	GXRasterizerDesc rasterizerDesc;
	memset(&rasterizerDesc, 0, sizeof(rasterizerDesc));
	rasterizerDesc.fillMode = GXFILL_SOLID;
	rasterizerDesc.cullMode = GXCULL_BACK;
	rasterizerDesc.useDepthClip = TRUE;
	m_pDefaultRasterizerState = createRasterizerState(&rasterizerDesc);

	GXDepthStencilDesc depthStencilDesc;
	memset(&depthStencilDesc, 0, sizeof(depthStencilDesc));
	depthStencilDesc.useDepthTest = TRUE;
	depthStencilDesc.useDepthWrite = TRUE;
	depthStencilDesc.cmpFuncDepth = GXCMP_LESS_EQUAL;
	depthStencilDesc.useStencilTest = FALSE;

	m_pDefaultDepthStencilState = createDepthStencilState(&depthStencilDesc);

	GXBlendDesc blendDesc;
	memset(&blendDesc, 0, sizeof(blendDesc));
	blendDesc.renderTarget[0].u8RenderTargetWriteMask = GXCOLOR_WRITE_ENABLE_ALL;
	
	m_pDefaultBlendState = createBlendState(&blendDesc);


	m_pDirectContext = new CGXContext(m_pDeviceContext, this, true);
	m_pContexts[0] = m_pDirectContext;

	m_pDefaultSwapChain = createSwapChain(m_uWindowWidth, m_uWindowHeight, wnd, isWindowed);
	m_pDirectContext->setColorTarget(NULL);


	m_pDirectContext->setDepthStencilSurface(m_pDefaultDepthStencilSurface);


	IGXSurface *pColorTarget = m_pDefaultSwapChain->getColorTarget();
	m_pDirectContext->setColorTarget(pColorTarget);
	mem_release(pColorTarget);
	
	log(GX_LOG_INFO, "Context ready!\n\n");
	return(TRUE);*/
}

void CGXDevice::swapBuffers()
{
	m_pDefaultSwapChain->swapBuffers();
}


IGXVertexBuffer* CGXDevice::createVertexBuffer(size_t size, GXBUFFER_USAGE flags, void *pInitData)
{
	//PFNGLGENVERTEXARRAYSPROC

	CGXVertexBuffer* pBuff = new CGXVertexBuffer(this);
	m_pGL->glGenBuffers(1, &pBuff->m_pBuffer);

	m_pGL->glBindBuffer(GL_ARRAY_BUFFER, pBuff->m_pBuffer);
	UINT usage = GL_STATIC_DRAW;

	if (flags & GXBUFFER_USAGE_STATIC)
	{
		usage = GL_STATIC_DRAW;
	}
	if (flags & GXBUFFER_USAGE_DYNAMIC)
	{
		usage = GL_DYNAMIC_DRAW;
	}
	if (flags & GXBUFFER_USAGE_STREAM)
	{
		usage = GL_STREAM_DRAW;
	}
	//if(flags & GX_BUFFER_POOL_DEFAULT)
	//{
	//}

	m_pGL->glBufferData(GL_ARRAY_BUFFER, size, pInitData, usage);

	//restore pprev buffer
	return(pBuff);
	/*DX11
	CGXVertexBuffer * pBuff = new CGXVertexBuffer(this);

	D3D11_BUFFER_DESC bd;
	memset(&bd, 0, sizeof(bd));
	if(flags & GXBUFFER_USAGE_STATIC)
	{
		pBuff->m_isLockable = false;
		bd.Usage = D3D11_USAGE_DEFAULT;
	}
	else
	{
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	bd.ByteWidth = (UINT)size;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA *pSubresourceData = NULL;
	D3D11_SUBRESOURCE_DATA initData;
	if(pInitData)
	{
		memset(&initData, 0, sizeof(initData));
		initData.pSysMem = pInitData;
		initData.SysMemPitch = bd.ByteWidth;
		pSubresourceData = &initData;

		m_pDirectContext->addBytesVertices((int)size);
	}

	if(FAILED(DX_CALL(m_pDevice->CreateBuffer(&bd, pSubresourceData, &pBuff->m_pBuffer))))
	{
		mem_delete(pBuff);
		return(NULL);
	}

	pBuff->m_uSize = (UINT)size;
	m_memoryStats.sizeVertexBufferBytes += pBuff->m_uSize;

	return(pBuff);*/
}

IGXIndexBuffer* CGXDevice::createIndexBuffer(size_t size, GXBUFFER_USAGE flags, GXINDEXTYPE it, void *pInitData)
{
	CGXIndexBuffer * pBuff = new CGXIndexBuffer(this);
	m_pGL->glGenBuffers(1, &pBuff->m_pBuffer);

	m_pGL->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pBuff->m_pBuffer);
	UINT usage = GL_STATIC_DRAW;

	if(flags & GXBUFFER_USAGE_STATIC)
	{
		usage = GL_STATIC_DRAW;
	}
	if(flags & GXBUFFER_USAGE_DYNAMIC)
	{
		usage = GL_DYNAMIC_DRAW;
	}
	if(flags & GXBUFFER_USAGE_STREAM)
	{
		usage = GL_STREAM_DRAW;
	}

	m_pGL->glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, pInitData, usage);

	m_sync_state.bIndexBuffer = TRUE;

	switch(it)
	{
	//case GXIT_UBYTE:
	//	pBuff->m_uIndexSize = GL_UNSIGNED_BYTE;
	//	break;
	case GXIT_UINT16:
		pBuff->m_uSize = GL_UNSIGNED_SHORT;
		break;
	case GXIT_UINT32:
		pBuff->m_uSize = GL_UNSIGNED_INT;
		break;
	}

	return(pBuff);
	/*DX11
	CGXIndexBuffer * pBuff = new CGXIndexBuffer(this);
	
	D3D11_BUFFER_DESC bd;
	memset(&bd, 0, sizeof(bd));
	if(flags & GXBUFFER_USAGE_STATIC)
	{
		pBuff->m_isLockable = false;
		bd.Usage = D3D11_USAGE_DEFAULT;
	}
	else
	{
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	bd.ByteWidth = (UINT)size;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA *pSubresourceData = NULL;
	D3D11_SUBRESOURCE_DATA initData;
	if(pInitData)
	{
		memset(&initData, 0, sizeof(initData));
		initData.pSysMem = pInitData;
		initData.SysMemPitch = bd.ByteWidth;
		pSubresourceData = &initData;

		m_pDirectContext->addBytesIndices((int)size);
	}

	if(FAILED(DX_CALL(m_pDevice->CreateBuffer(&bd, pSubresourceData, &pBuff->m_pBuffer))))
	{
		mem_delete(pBuff);
		return(NULL);
	}

	switch(it)
	{
	case GXIT_UINT16:
		pBuff->m_format = DXGI_FORMAT_R16_UINT;
		break;
	case GXIT_UINT32:
		pBuff->m_format = DXGI_FORMAT_R32_UINT;
		break;
	}

	pBuff->m_uSize = (UINT)size;
	m_memoryStats.sizeIndexBufferBytes += pBuff->m_uSize;
		
	return(pBuff);*/
	assert(!"Not imlemented");
	return(NULL);
}

void CGXDevice::destroyIndexBuffer(CGXIndexBuffer *pBuff)
{
	assert(pBuff);

	m_memoryStats.sizeIndexBufferBytes -= pBuff->m_uSize;
	/*OGL
	if(pBuff)
	{
		m_pGL->glDeleteBuffers(1, &(((CGXIndexBuffer*)pBuff)->m_pBuffer));
		if(m_pCurIndexBuffer == pBuff)
		{
			m_pCurIndexBuffer = NULL;
			m_sync_state.bIndexBuffer = TRUE;
		}
	}
	mem_delete(pBuff);*/
}

void CGXDevice::destroyVertexBuffer(CGXVertexBuffer *pBuff)
{
	if(pBuff)
	{
		m_pGL->glDeleteBuffers(1, &(((CGXVertexBuffer*)pBuff)->m_pBuffer));
		/*for(int i = 0; i < MAXDSGVSTREAM; ++i)
		{
			if(m_pCurVertexBuffer[i] == pBuff)
			{
				m_sync_state.bVertexBuffers[i] = TRUE;
				m_sync_state.bVertexLayout = TRUE;
			}
		}*/
	}
	mem_delete(pBuff);
	/*DX11
	assert(pBuff);
	m_memoryStats.sizeVertexBufferBytes -= pBuff->m_uSize;*/
}

IGXVertexDeclaration* CGXDevice::createVertexDeclaration(const GXVertexElement *pDecl)
{
	CGXVertexDeclaration * vd = new CGXVertexDeclaration(m_pDevice, this, pDecl);

//	m_pGL->glGenVertexArrays(1, &vd->m_pVAO);
	//m_pGL->glBindVertexArray(vd->m_pVAO);
	//m_sync_state.bVertexLayout = FALSE;
	return(vd);
}

static ID3DBlob* CompileShader(CGXDevice *pDevice, const char *szFile, GXMacro *pDefs, const char *szProfile)
{
	ID3DBlob *pShaderBlob = NULL;
	ID3DBlob *pErrorBlob = NULL;
	/*bool isFailed = FAILED(DX_CALL(D3DX11CompileFromFileA(szFile, (D3D_SHADER_MACRO*)pDefs, NULL, "main", szProfile, SHADER_FLAGS, 0, NULL, &pShaderBlob, &pErrorBlob, NULL)));
	
	if(pErrorBlob)
	{
		pDevice->log(GX_LOG_INFO, "Compiling shader %s\n", szFile);
		size_t s = strlen((char*)pErrorBlob->GetBufferPointer());
		char *str = (char*)alloca(s + 27);
		sprintf(str, "%s: \n%s", isFailed ? "Unable to create shader" : "Shader warning", (char*)pErrorBlob->GetBufferPointer());
		CGXDevice::debugMessage(isFailed ? GX_LOG_ERROR : GX_LOG_WARN, str);
		mem_release(pErrorBlob);
	}

	if(isFailed)
	{
		return(NULL);
	}*/

	return(pShaderBlob);
}

static ID3DBlob* CompileShaderFromString(CGXDevice *pDevice, const char *szCode, GXMacro *pDefs, const char *szProfile)
{
	ID3DBlob *pShaderBlob = NULL;
	ID3DBlob *pErrorBlob = NULL;

	const char *szShaderFileName = "memory.vs";
	char szFileName[256];
	if(!memcmp(szCode, "#line ", 6))
	{
 		UINT idx = 6;
		while(szCode[idx] && szCode[idx] != '"')
		{
			++idx;
		}
		UINT uStartIdx = idx++ + 1;
		while(szCode[idx] && szCode[idx] != '"')
		{
			++idx;
		}
		memcpy(szFileName, szCode + uStartIdx, idx - uStartIdx);
		szFileName[idx - uStartIdx] = 0;
		szShaderFileName = szFileName;
	}

	/*bool isFailed = FAILED(DX_CALL(D3DX11CompileFromMemory(szCode, strlen(szCode), szShaderFileName, (D3D_SHADER_MACRO*)pDefs, NULL, "main", szProfile, SHADER_FLAGS, 0, NULL, &pShaderBlob, &pErrorBlob, NULL)));

	if(pErrorBlob)
	{
		size_t s = strlen((char*)pErrorBlob->GetBufferPointer());
		char *str = (char*)alloca(s + 27);
		sprintf(str, "%s: \n%s", isFailed ? "Unable to create shader" : "Shader warning", (char*)pErrorBlob->GetBufferPointer());
		CGXDevice::debugMessage(isFailed ? GX_LOG_ERROR : GX_LOG_WARN, str);
		mem_release(pErrorBlob);
	}

	if(isFailed)
	{
		return(NULL);
	}*/

	return(pShaderBlob);
}

IGXVertexShader* CGXDevice::createVertexShader(const char *szFile, GXMacro *pDefs)
{
	/*ID3DBlob *pShaderBlob = CompileShader(this, szFile, pDefs, "vs_4_0");

	if(!pShaderBlob)
	{
		return(NULL);
	}

	IGXVertexShader *pShader = createVertexShaderFromBin(pShaderBlob->GetBufferPointer(), (UINT)pShaderBlob->GetBufferSize());

	mem_release(pShaderBlob);
	
	return(pShader);*/
	assert(!"Not imlemented");
	return(NULL);
}
IGXVertexShader* CGXDevice::createVertexShaderFromString(const char *szCode, GXMacro *pDefs)
{
	/*ID3DBlob *pShaderBlob = CompileShaderFromString(this, szCode, pDefs, "vs_4_0");

	if(!pShaderBlob)
	{
		return(NULL);
	}

	IGXVertexShader *pShader = createVertexShaderFromBin(pShaderBlob->GetBufferPointer(), (UINT)pShaderBlob->GetBufferSize());

	mem_release(pShaderBlob);

	return(pShader);*/
	assert(!"Not imlemented");
	return(NULL);
}
IGXVertexShader* CGXDevice::createVertexShaderFromBin(void *_pData, UINT uSize)
{
	CGXVertexShader *pShader = new CGXVertexShader(this);

	/*DX_CALL(D3DCreateBlob(uSize, &pShader->m_pShaderBlob));
	memcpy(pShader->m_pShaderBlob->GetBufferPointer(), _pData, uSize);

	if(FAILED(DX_CALL(m_pDevice->CreateVertexShader(pShader->m_pShaderBlob->GetBufferPointer(), uSize, NULL, &pShader->m_pShader))))
	{
		mem_delete(pShader);
		return(NULL);
	}*/

	return(pShader);
}

IGXPixelShader* CGXDevice::createPixelShader(const char *szFile, GXMacro *pDefs)
{
	/*ID3DBlob *pShaderBlob = CompileShader(this, szFile, pDefs, "ps_4_0");

	if(!pShaderBlob)
	{
		return(NULL);
	}

	IGXPixelShader *pShader = createPixelShaderFromBin(pShaderBlob->GetBufferPointer(), (UINT)pShaderBlob->GetBufferSize());

	mem_release(pShaderBlob);

	return(pShader);*/
	assert(!"Not imlemented");
	return(NULL);
}
IGXPixelShader* CGXDevice::createPixelShaderFromBin(void *_pData, UINT uSize)
{
	/*CGXPixelShader *pShader = new CGXPixelShader(this);

	DX_CALL(D3DCreateBlob(uSize, &pShader->m_pShaderBlob));
	memcpy(pShader->m_pShaderBlob->GetBufferPointer(), _pData, uSize);

	if(FAILED(DX_CALL(m_pDevice->CreatePixelShader(pShader->m_pShaderBlob->GetBufferPointer(), uSize, NULL, &pShader->m_pShader))))
	{
		mem_delete(pShader);
		return(NULL);
	}

	return(pShader);*/
	assert(!"Not imlemented");
	return(NULL);
}
IGXPixelShader* CGXDevice::createPixelShaderFromString(const char *szCode, GXMacro *pDefs)
{
	/*ID3DBlob *pShaderBlob = CompileShaderFromString(this, szCode, pDefs, "ps_4_0");

	if(!pShaderBlob)
	{
		return(NULL);
	}

	IGXPixelShader *pShader = createPixelShaderFromBin(pShaderBlob->GetBufferPointer(), (UINT)pShaderBlob->GetBufferSize());

	mem_release(pShaderBlob);

	return(pShader);*/
	assert(!"Not imlemented");
	return(NULL);
}

IGXGeometryShader* CGXDevice::createGeometryShader(const char *szFile, GXMacro *pDefs)
{
	/*ID3DBlob *pShaderBlob = CompileShader(this, szFile, pDefs, "gs_4_0");

	if(!pShaderBlob)
	{
		return(NULL);
	}

	IGXGeometryShader *pShader = createGeometryShaderFromBin(pShaderBlob->GetBufferPointer(), (UINT)pShaderBlob->GetBufferSize());

	mem_release(pShaderBlob);

	return(pShader);*/
	assert(!"Not imlemented");
	return(NULL);
}
IGXGeometryShader* CGXDevice::createGeometryShaderFromBin(void *_pData, UINT uSize)
{
	/*CGXGeometryShader *pShader = new CGXGeometryShader(this);

	DX_CALL(D3DCreateBlob(uSize, &pShader->m_pShaderBlob));
	memcpy(pShader->m_pShaderBlob->GetBufferPointer(), _pData, uSize);

	if(FAILED(DX_CALL(m_pDevice->CreateGeometryShader(pShader->m_pShaderBlob->GetBufferPointer(), uSize, NULL, &pShader->m_pShader))))
	{
		mem_delete(pShader);
		return(NULL);
	}

	return(pShader);*/
	assert(!"Not imlemented");
	return(NULL);
}
IGXGeometryShader* CGXDevice::createGeometryShaderFromString(const char *szCode, GXMacro *pDefs)
{
	/*ID3DBlob *pShaderBlob = CompileShaderFromString(this, szCode, pDefs, "gs_4_0");

	if(!pShaderBlob)
	{
		return(NULL);
	}
	IGXGeometryShader *pShader = createGeometryShaderFromBin(pShaderBlob->GetBufferPointer(), (UINT)pShaderBlob->GetBufferSize());

	mem_release(pShaderBlob);

	return(pShader);*/
	assert(!"Not imlemented");
	return(NULL);
}

IGXComputeShader* CGXDevice::createComputeShader(const char *szFile, GXMacro *pDefs)
{
	// Prefer higher CS shader profile when possible as CS 5.0 provides better performance on 11-class hardware
	/*D3D_FEATURE_LEVEL fl = m_pDevice->GetFeatureLevel();
	const char *szVersion = m_pDevice->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0 ? "cs_5_0" : "cs_4_0";

	ID3DBlob *pShaderBlob = CompileShader(this, szFile, pDefs, szVersion);
	
	if(!pShaderBlob)
	{
		return(NULL);
	}

	IGXComputeShader *pShader = createComputeShaderFromBin(pShaderBlob->GetBufferPointer(), (UINT)pShaderBlob->GetBufferSize());

	mem_release(pShaderBlob);

	return(pShader);*/
	assert(!"Not imlemented");
	return(NULL);
}
IGXComputeShader* CGXDevice::createComputeShaderFromBin(void *_pData, UINT uSize)
{
	/*CGXComputeShader *pShader = new CGXComputeShader(this);

	DX_CALL(D3DCreateBlob(uSize, &pShader->m_pShaderBlob));
	memcpy(pShader->m_pShaderBlob->GetBufferPointer(), _pData, uSize);

	if(FAILED(DX_CALL(m_pDevice->CreateComputeShader(pShader->m_pShaderBlob->GetBufferPointer(), uSize, NULL, &pShader->m_pShader))))
	{
		mem_delete(pShader);
		return(NULL);
	}

	return(pShader);*/
	assert(!"Not imlemented");
	return(NULL);
}
IGXComputeShader* CGXDevice::createComputeShaderFromString(const char *szCode, GXMacro *pDefs)
{
	/*D3D_FEATURE_LEVEL fl = m_pDevice->GetFeatureLevel();
	const char *szVersion = m_pDevice->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0 ? "cs_5_0" : "cs_4_0";
	ID3DBlob *pShaderBlob = CompileShaderFromString(this, szCode, pDefs, szVersion);

	if(!pShaderBlob)
	{
		return(NULL);
	}

	IGXComputeShader *pShader = createComputeShaderFromBin(pShaderBlob->GetBufferPointer(), (UINT)pShaderBlob->GetBufferSize());

	mem_release(pShaderBlob);

	return(pShader);*/
	assert(!"Not imlemented");
	return(NULL);
}

IGXShaderSet* CGXDevice::createShader(IGXVertexShader *pVS, IGXPixelShader *pPS, IGXGeometryShader *pGS, IGXComputeShader *pCS)
{
	return(new CGXShader(this, pVS, pPS, pGS, pCS));
}

IGXRenderBuffer* CGXDevice::createRenderBuffer(UINT countSlots, IGXVertexBuffer **pBuff, IGXVertexDeclaration *pDecl)
{
	assert(countSlots);
	assert(pBuff);
	assert(pDecl);

	CGXRenderBuffer* pRB = new CGXRenderBuffer(this, countSlots, pBuff, pDecl);

	m_pGL->glGenVertexArrays(1, &pRB->m_uVAO);

	m_pGL->glBindVertexArray(pRB->m_uVAO);
	m_sync_state.bRenderBuffer = TRUE;


	CGXVertexDeclaration* vd = (CGXVertexDeclaration*)pDecl;

	UINT buff = 0;

	for (UINT i = 0; i < vd->m_uDeclCount; i++)
	{
		if (countSlots <= vd->m_pDeclItems[i].stream)
		{
			m_pGL->glBindBuffer(GL_ARRAY_BUFFER, 0);
			buff = 0;
		}
		else
		{
			UINT cb = ((CGXVertexBuffer*)pBuff[vd->m_pDeclItems[i].stream])->m_pBuffer;
			if (buff != cb || i == 0)
			{
				m_pGL->glBindBuffer(GL_ARRAY_BUFFER, cb);
				buff = cb;
			}
		}
		m_pGL->glVertexAttribPointer(vd->m_pDeclItems[i].index, vd->m_pDeclItems[i].size, vd->m_pDeclItems[i].type​, vd->m_pDeclItems[i].normalized, vd->m_pDeclItems[i].stride​, vd->m_pDeclItems[i].pointer​);
		m_pGL->glEnableVertexAttribArray(vd->m_pDeclItems[i].index);
	}
	return(pRB);
	/*DX11
	assert(countSlots);
	assert(pBuff);
	assert(pDecl);

	CGXRenderBuffer * pRB = new CGXRenderBuffer(this, countSlots, pBuff, pDecl);

	return(pRB);*/
}

void CGXDevice::log(GX_LOG lvl, const char *szFormat, ...)
{
	if(m_pLogger)
	{
		va_list va;
		va_start(va, szFormat);
		size_t len = _vscprintf(szFormat, va) + 1;
		char * buf = (char*)alloca(len * sizeof(char));
		vsprintf(buf, szFormat, va);
		va_end(va);

		switch(lvl)
		{
		case GX_LOG_INFO:
			m_pLogger->logInfo(buf);
			break;
		case GX_LOG_WARN:
			m_pLogger->logWarning(buf);
			break;
		case GX_LOG_ERROR:
			m_pLogger->logError(buf);
			break;
		default:
			break;
		}
	}
}
void CGXDevice::debugMessage(GX_LOG lvl, const char *msg)
{
	switch(lvl)
	{
	case GX_LOG_INFO:
		OutputDebugString("[GX]:INFO:");
		break;
	case GX_LOG_WARN:
		OutputDebugString("[GX]:WARN:");
		break;
	case GX_LOG_ERROR:
		OutputDebugString("[GX]:ERR:");
		break;
	}
	OutputDebugString(msg);
	OutputDebugString("\n");

	ms_pInstance->log(lvl, "%s\n", msg);
}

void CGXDevice::logDXcall(const char *szCondeString, HRESULT hr)
{
	static char str[4096];
	
	//sprintf_s(str, "GX call failed: %s\n    %s, %s", szCondeString, DXGetErrorString(hr), DXGetErrorDescription(hr));
	debugMessage(GX_LOG_ERROR, str);
}

IGXSamplerState* CGXDevice::createSamplerState(GXSamplerDesc *pSamplerDesc)
{
	/*CGXSamplerState *pSS = new CGXSamplerState(this);

	pSS->m_desc = *pSamplerDesc;
	pSS->onDevRst();

	return(pSS);*/
	assert(!"Not imlemented");
	return(NULL);

}

IGXRasterizerState* CGXDevice::createRasterizerState(GXRasterizerDesc *pRSDesc)
{
	/*CGXRasterizerState *pRS = new CGXRasterizerState(this);

	pRS->m_isScissorsEnabled = pRSDesc->useScissorTest;
	pRS->m_desc = *pRSDesc;
	pRS->onDevRst();

	return(pRS);*/
	assert(!"Not imlemented");
	return(NULL);
}

IGXDepthStencilState* CGXDevice::createDepthStencilState(GXDepthStencilDesc *pDSDesc)
{
	/*CGXDepthStencilState *pDS = new CGXDepthStencilState(this);
	pDS->m_desc = *pDSDesc;
	pDS->onDevRst();

	return(pDS);*/
	assert(!"Not imlemented");
	return(NULL);
}

IGXBlendState* CGXDevice::createBlendState(GXBlendDesc *pBSDesc)
{
	CGXBlendState *pBS = new CGXBlendState(this);
	
	pBS->m_desc = *pBSDesc;
	pBS->onDevRst();
	return(pBS);
}


IGXDepthStencilSurface* CGXDevice::createDepthStencilSurface(UINT uWidth, UINT uHeight, GXFORMAT format, GXMULTISAMPLE_TYPE multisampleType, bool bAutoResize)
{
	/*CGXDepthStencilSurface *pDSSurface = new CGXDepthStencilSurface(this);

	if(!uWidth)
	{
		uWidth = 1;
	}
	if(!uHeight)
	{
		uHeight = 1;
	}

	D3D11_TEXTURE2D_DESC *pDescTex = &pDSSurface->m_descTex;
	memset(pDescTex, 0, sizeof(D3D11_TEXTURE2D_DESC));
	pDescTex->Width = uWidth;
	pDescTex->Height = uHeight;
	pDescTex->MipLevels = 1;
	pDescTex->ArraySize = 1;
	pDescTex->Format = getDXFormat(format);
	pDescTex->SampleDesc.Count = max(multisampleType, 1);
	pDescTex->SampleDesc.Quality = 0;
	pDescTex->Usage = D3D11_USAGE_DEFAULT;
	pDescTex->BindFlags = D3D11_BIND_DEPTH_STENCIL;

	D3D11_DEPTH_STENCIL_VIEW_DESC *pDSVDesc = &pDSSurface->m_desc;
	memset(pDSVDesc, 0, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	pDSVDesc->Format = pDescTex->Format;
	pDSVDesc->ViewDimension = pDescTex->SampleDesc.Count > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
	pDSVDesc->Texture2D.MipSlice = 0;

	pDSSurface->onDevRst(m_uWindowWidth, m_uWindowHeight);

	m_aResettableDSSurfaces.push_back(pDSSurface);

	if(bAutoResize)
	{
		pDSSurface->m_bAutoResize = true;

		pDSSurface->m_fSizeCoeffH = (float)uHeight / (float)m_uWindowHeight;
		pDSSurface->m_fSizeCoeffW = (float)uWidth / (float)m_uWindowWidth;
	}

	return(pDSSurface);*/
	assert(!"Not imlemented");
	return(NULL);
}
IGXDepthStencilSurface* CGXDevice::createDepthStencilSurfaceCube(UINT uSize, GXFORMAT format, GXMULTISAMPLE_TYPE multisampleType, bool bAutoResize)
{
	/*CGXDepthStencilSurface *pDSSurface = new CGXDepthStencilSurface(this);

	if(!uSize)
	{
		uSize = 1;
	}

	D3D11_TEXTURE2D_DESC *pDescTex = &pDSSurface->m_descTex;
	memset(pDescTex, 0, sizeof(D3D11_TEXTURE2D_DESC));
	pDescTex->Width = uSize;
	pDescTex->Height = uSize;
	pDescTex->MipLevels = 1;
	pDescTex->ArraySize = 6;
	pDescTex->Format = getDXFormat(format);
	pDescTex->SampleDesc.Count = max(multisampleType, 1);
	pDescTex->SampleDesc.Quality = 0;
	pDescTex->Usage = D3D11_USAGE_DEFAULT;
	pDescTex->BindFlags = D3D11_BIND_DEPTH_STENCIL;

	D3D11_DEPTH_STENCIL_VIEW_DESC *pDSVDesc = &pDSSurface->m_desc;
	memset(pDSVDesc, 0, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	pDSVDesc->Format = pDescTex->Format;
	pDSVDesc->ViewDimension = pDescTex->SampleDesc.Count > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY : D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	pDSVDesc->Texture2DArray.ArraySize = pDescTex->ArraySize;
	pDSVDesc->Texture2DArray.MipSlice = 0;

	pDSSurface->onDevRst(m_uWindowWidth, m_uWindowHeight);

	m_aResettableDSSurfaces.push_back(pDSSurface);

	if(bAutoResize)
	{
		pDSSurface->m_bAutoResize = true;

		pDSSurface->m_fSizeCoeffW = pDSSurface->m_fSizeCoeffH = (float)uSize / (float)m_uWindowHeight;
	}

	return(pDSSurface);*/
	assert(!"Not imlemented");
	return(NULL);
}
void CGXDevice::destroyDepthStencilSurface(IGXDepthStencilSurface *pSurface)
{
	if(pSurface)
	{
		for(UINT i = 0, l = m_aResettableDSSurfaces.size(); i < l; ++i)
		{
			if(m_aResettableDSSurfaces[i] == pSurface)
			{
				m_aResettableDSSurfaces.erase(i);
				break;
			}
		}
	}
}


IGXSurface* CGXDevice::createColorTarget(UINT uWidth, UINT uHeight, GXFORMAT format, GXMULTISAMPLE_TYPE multisampleType, bool bAutoResize)
{
	/*CGXSurface *pColorSurface = new CGXSurface(this, uWidth, uHeight, format, NULL);

	memset(&pColorSurface->m_descTex2D, 0, sizeof(pColorSurface->m_descTex2D));
	pColorSurface->m_descTex2D.ArraySize = 1;
	pColorSurface->m_descTex2D.BindFlags = D3D11_BIND_RENDER_TARGET;
	pColorSurface->m_descTex2D.Format = getDXFormat(format);
	pColorSurface->m_descTex2D.Height = uHeight;
	pColorSurface->m_descTex2D.MipLevels = 1;
	pColorSurface->m_descTex2D.SampleDesc.Count = max(multisampleType, 1);
	pColorSurface->m_descTex2D.SampleDesc.Quality = 0;
	pColorSurface->m_descTex2D.Width = uWidth;
	pColorSurface->m_descTex2D.Usage = D3D11_USAGE_DEFAULT;

	m_aResettableColorSurfaces.push_back(pColorSurface);

	if(bAutoResize)
	{
		pColorSurface->m_bAutoResize = true;

		pColorSurface->m_fSizeCoeffH = (float)uHeight / (float)m_uWindowHeight;
		pColorSurface->m_fSizeCoeffW = (float)uWidth / (float)m_uWindowWidth;
	}

	pColorSurface->onDevRst(uWidth, uHeight);
	return(pColorSurface);*/
	assert(!"Not imlemented");
	return(NULL);
}
void CGXDevice::destroyColorTarget(IGXSurface *pSurface)
{
	if(pSurface)
	{
		for(UINT i = 0, l = m_aResettableColorSurfaces.size(); i < l; ++i)
		{
			if(m_aResettableColorSurfaces[i] == pSurface)
			{
				m_aResettableColorSurfaces.erase(i);
				break;
			}
		}

		mem_delete(pSurface);
	}
}

IGXTexture2D* CGXDevice::createTexture2D(UINT uWidth, UINT uHeight, UINT uMipLevels, UINT uTexUsageFlags, GXFORMAT format, void *pInitData)
{
	/*CGXTexture2D *pTex = new CGXTexture2D(this);

	pTex->m_format = format;
	pTex->m_uHeight = uHeight;
	pTex->m_uWidth = uWidth;
	pTex->m_bWasReset = true;
	pTex->m_bAutoResize = !!(uTexUsageFlags & GX_TEXFLAG_AUTORESIZE);

	if(pTex->m_bAutoResize)
	{
		pTex->m_fSizeCoeffH = (float)uHeight / (float)m_uWindowHeight;
		pTex->m_fSizeCoeffW = (float)uWidth / (float)m_uWindowWidth;
	}

	memset(&pTex->m_descTex2D, 0, sizeof(pTex->m_descTex2D));
	pTex->m_descTex2D.ArraySize = 1;
	pTex->m_descTex2D.Width = uWidth;
	pTex->m_descTex2D.Height = uHeight;
	pTex->m_descTex2D.MipLevels = uMipLevels;
	pTex->m_descTex2D.Format = getDXFormat(format);
	pTex->m_descTex2D.SampleDesc.Count = 1;
	pTex->m_descTex2D.SampleDesc.Quality = 0;
	pTex->m_descTex2D.Usage = D3D11_USAGE_DEFAULT;
	pTex->m_descTex2D.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	if(uTexUsageFlags & GX_TEXFLAG_RENDERTARGET)
	{
		pTex->m_descTex2D.BindFlags |= D3D11_BIND_RENDER_TARGET;
	}
	if(uTexUsageFlags & GX_TEXFLAG_UNORDERED_ACCESS)
	{
		pTex->m_descTex2D.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
	}
	if(uTexUsageFlags & GX_TEXFLAG_AUTOGENMIPMAPS)
	{
		pTex->m_descTex2D.MipLevels = 0;
	}
	pTex->m_uMipLevels = pTex->m_descTex2D.MipLevels;

	if(uTexUsageFlags & GX_TEXFLAG_AUTORESIZE)
	{
		pTex->m_isResettable = true;
		m_aResettableTextures2D.push_back(pTex);
	}

	pTex->m_iTotalSize = 0;

	if(!(uTexUsageFlags & GX_TEXFLAG_RENDERTARGET) && pInitData)
	{
		pTex->m_bWasReset = false;

		if(uTexUsageFlags & GX_TEXFLAG_INIT_ALL_MIPS)
		{
			if(pTex->m_descTex2D.MipLevels == 0)
			{
				debugMessage(GX_LOG_ERROR, "Unsupported: GX_TEXFLAG_INIT_ALL_MIPS is not supported with GX_TEXFLAG_AUTOGENMIPMAPS");
				mem_delete(pTex);
				return(NULL);
			}
			D3D11_SUBRESOURCE_DATA *pSubresourceData = (D3D11_SUBRESOURCE_DATA*)alloca(sizeof(D3D11_SUBRESOURCE_DATA) * pTex->m_descTex2D.MipLevels);
			GXImageMip *pData = (GXImageMip*)pInitData;
			UINT uTmpWidth = uWidth;
			UINT uTmpHeight = uHeight;
			for(UINT i = 0; i < pTex->m_descTex2D.MipLevels; ++i)
			{
				pSubresourceData[i].pSysMem = pData[i].pData;
				pSubresourceData[i].SysMemSlicePitch = 0;
				pSubresourceData[i].SysMemPitch = getTextureMemPitch(uTmpWidth, format);
				pTex->m_iTotalSize += (int)(pSubresourceData[i].SysMemPitch * uTmpHeight);
				uTmpWidth >>= 1;
				uTmpWidth = max(1, uTmpWidth);
				uTmpHeight >>= 1;
				uTmpHeight = max(1, uTmpHeight);
			}

			DX_CALL(m_pDevice->CreateTexture2D(&pTex->m_descTex2D, pSubresourceData, &pTex->m_pTexture));
		}
		else
		{
			D3D11_SUBRESOURCE_DATA initData;
			initData.pSysMem = pInitData;
			initData.SysMemSlicePitch = 0;
			initData.SysMemPitch = getTextureMemPitch(uWidth, format);
			pTex->m_iTotalSize += (int)(initData.SysMemPitch * uHeight);

			if(pTex->m_descTex2D.MipLevels == 1)
			{
				DX_CALL(m_pDevice->CreateTexture2D(&pTex->m_descTex2D, &initData, &pTex->m_pTexture));
			}
			else
			{
				DX_CALL(m_pDevice->CreateTexture2D(&pTex->m_descTex2D, NULL, &pTex->m_pTexture));

				m_pDeviceContext->UpdateSubresource(pTex->m_pTexture, 0, NULL, pInitData, initData.SysMemPitch, 0);
			}
		}
	}
	else
	{
		pTex->m_iTotalSize += (int)(getTextureMemPitch(pTex->m_uWidth, pTex->m_format) * pTex->m_uHeight);
		DX_CALL(m_pDevice->CreateTexture2D(&pTex->m_descTex2D, NULL, &pTex->m_pTexture));
	}

	memset(&pTex->m_descSRV, 0, sizeof(pTex->m_descSRV));
	pTex->m_descSRV.Format = pTex->m_descTex2D.Format;
	pTex->m_descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	pTex->m_descSRV.Texture2D.MipLevels = pTex->m_descTex2D.MipLevels;

	if(!pTex->m_descSRV.Texture2D.MipLevels)
	{
		UINT uSize = max(pTex->m_uHeight, pTex->m_uWidth);
		do
		{
			++pTex->m_descSRV.Texture2D.MipLevels;
		}
		while(uSize >>= 1);
	}

	DX_CALL(m_pDevice->CreateShaderResourceView(pTex->m_pTexture, &pTex->m_descSRV, &pTex->m_pSRV));

	m_pDirectContext->addBytesTextures(pTex->m_iTotalSize);
	addBytesTextures(pTex->m_iTotalSize, !!(pTex->m_descTex2D.BindFlags & D3D11_BIND_RENDER_TARGET));

	if(uTexUsageFlags & GX_TEXFLAG_UNORDERED_ACCESS)
	{
		memset(&pTex->m_descUAV, 0, sizeof(pTex->m_descUAV));
		pTex->m_descUAV.Format = DXGI_FORMAT_UNKNOWN;
		pTex->m_descUAV.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		DX_CALL(m_pDevice->CreateUnorderedAccessView(pTex->m_pTexture, &pTex->m_descUAV, &pTex->m_pUAV));
	}

	return(pTex);*/
	assert(!"Not imlemented");
	return(NULL);
}
IGXTexture3D* CGXDevice::createTexture3D(UINT uWidth, UINT uHeight, UINT uDepth, UINT uMipLevels, UINT uTexUsageFlags, GXFORMAT format, void *pInitData)
{
	assert(!(uTexUsageFlags & GX_TEXFLAG_AUTORESIZE) && "GX_TEXFLAG_AUTORESIZE is not supported on 3D textures");

	/*CGXTexture3D *pTex = new CGXTexture3D(this);

	pTex->m_format = format;
	pTex->m_uHeight = uHeight;
	pTex->m_uWidth = uWidth;
	pTex->m_uDepth = uDepth;
	pTex->m_bWasReset = true;
	
	memset(&pTex->m_descTex3D, 0, sizeof(pTex->m_descTex3D));
	pTex->m_descTex3D.Width = uWidth;
	pTex->m_descTex3D.Height = uHeight;
	pTex->m_descTex3D.Depth = uDepth;
	pTex->m_descTex3D.MipLevels = uMipLevels;
	pTex->m_descTex3D.Format = getDXFormat(format);
	pTex->m_descTex3D.Usage = D3D11_USAGE_DEFAULT;
	pTex->m_descTex3D.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	if(uTexUsageFlags & GX_TEXFLAG_RENDERTARGET)
	{
		pTex->m_descTex3D.BindFlags |= D3D11_BIND_RENDER_TARGET;
	}
	if(uTexUsageFlags & GX_TEXFLAG_UNORDERED_ACCESS)
	{
		pTex->m_descTex3D.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
	}
	if(uTexUsageFlags & GX_TEXFLAG_AUTOGENMIPMAPS)
	{
		pTex->m_descTex3D.MipLevels = 0;
	}
	pTex->m_uMipLevels = pTex->m_descTex3D.MipLevels;
	
	if(uTexUsageFlags & GX_TEXFLAG_INIT_ALL_MIPS)
	{
		debugMessage(GX_LOG_ERROR, "Not implemented: GX_TEXFLAG_INIT_ALL_MIPS is not supported with 3D texture");
		mem_delete(pTex);
		return(NULL);
	}

	if(/*!(uTexUsageFlags & GX_TEXFLAG_RENDERTARGET) && *//*pInitData)
	{
		pTex->m_bWasReset = false;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = pInitData;
		initData.SysMemSlicePitch = getTextureMemPitch(uDepth, format) * uWidth;
		initData.SysMemPitch = getTextureMemPitch(uWidth, format);
		addBytesTextures(/*initData.SysMemPitch * *//*initData.SysMemSlicePitch * uHeight);

		if(pTex->m_descTex3D.MipLevels == 1)
		{
			DX_CALL(m_pDevice->CreateTexture3D(&pTex->m_descTex3D, &initData, &pTex->m_pTexture));
		}
		else
		{
			DX_CALL(m_pDevice->CreateTexture3D(&pTex->m_descTex3D, NULL, &pTex->m_pTexture));

			m_pDeviceContext->UpdateSubresource(pTex->m_pTexture, 0, NULL, pInitData, initData.SysMemPitch, initData.SysMemSlicePitch);
		}
	}
	else
	{
		DX_CALL(m_pDevice->CreateTexture3D(&pTex->m_descTex3D, NULL, &pTex->m_pTexture));
	}

	memset(&pTex->m_descSRV, 0, sizeof(pTex->m_descSRV));
	pTex->m_descSRV.Format = pTex->m_descTex3D.Format;
	pTex->m_descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
	pTex->m_descSRV.Texture3D.MipLevels = pTex->m_descTex3D.MipLevels;

	if(!pTex->m_descSRV.Texture3D.MipLevels)
	{
		UINT uSize = max(pTex->m_uHeight, pTex->m_uWidth);
		uSize = max(uSize, pTex->m_uDepth);
		do
		{
			++pTex->m_descSRV.Texture3D.MipLevels;
		}
		while(uSize >>= 1);
	}

	DX_CALL(m_pDevice->CreateShaderResourceView(pTex->m_pTexture, &pTex->m_descSRV, &pTex->m_pSRV));
	
	m_pDirectContext->addBytesTextures(getTextureMemPitch(pTex->m_uDepth, pTex->m_format) * pTex->m_uWidth * pTex->m_uHeight);
	addBytesTextures(getTextureMemPitch(pTex->m_uDepth, pTex->m_format) * pTex->m_uWidth * pTex->m_uHeight, !!(pTex->m_descTex3D.BindFlags & D3D11_BIND_RENDER_TARGET));

	if(uTexUsageFlags & GX_TEXFLAG_UNORDERED_ACCESS)
	{
		memset(&pTex->m_descUAV, 0, sizeof(pTex->m_descUAV));
		pTex->m_descUAV.Format = DXGI_FORMAT_UNKNOWN;
		pTex->m_descUAV.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
		pTex->m_descUAV.Texture3D.WSize = -1;
		DX_CALL(m_pDevice->CreateUnorderedAccessView(pTex->m_pTexture, &pTex->m_descUAV, &pTex->m_pUAV));
	}

	return(pTex);*/
	assert(!"Not imlemented");
	return(NULL);
}
IGXTextureCube* CGXDevice::createTextureCube(UINT uSize, UINT uMipLevels, UINT uTexUsageFlags, GXFORMAT format, void *pInitData)
{
	/*CGXTextureCube *pTex = new CGXTextureCube(this);

	pTex->m_format = format;
	pTex->m_uSize = uSize;
	pTex->m_bWasReset = true;
	pTex->m_bAutoResize = !!(uTexUsageFlags & GX_TEXFLAG_AUTORESIZE); 
	pTex->m_uMipLevels = uMipLevels;
	if(pTex->m_bAutoResize)
	{
		pTex->m_fSizeCoeff = (float)uSize / (float)m_uWindowHeight;
	}

	memset(&pTex->m_descTex2D, 0, sizeof(pTex->m_descTex2D));
	pTex->m_descTex2D.ArraySize = 6;
	pTex->m_descTex2D.Width = uSize;
	pTex->m_descTex2D.Height = uSize;
	pTex->m_descTex2D.MipLevels = uMipLevels;
	pTex->m_descTex2D.Format = getDXFormat(format);
	pTex->m_descTex2D.SampleDesc.Count = 1;
	pTex->m_descTex2D.SampleDesc.Quality = 0;
	pTex->m_descTex2D.Usage = D3D11_USAGE_DEFAULT;
	pTex->m_descTex2D.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	pTex->m_descTex2D.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	if(uTexUsageFlags & GX_TEXFLAG_RENDERTARGET)
	{
		pTex->m_descTex2D.BindFlags |= D3D11_BIND_RENDER_TARGET;
	}
	if(uTexUsageFlags & GX_TEXFLAG_UNORDERED_ACCESS)
	{
		pTex->m_descTex2D.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
	}
	if(uTexUsageFlags & GX_TEXFLAG_AUTOGENMIPMAPS)
	{
		pTex->m_descTex2D.MipLevels = 0;
	}
	pTex->m_uMipLevels = pTex->m_descTex2D.MipLevels;

	if(uTexUsageFlags & GX_TEXFLAG_AUTORESIZE)
	{
		pTex->m_isResettable = true;
		m_aResettableTexturesCube.push_back(pTex);
	}

	if(pInitData)
	{
		if(uTexUsageFlags & GX_TEXFLAG_INIT_ALL_MIPS)
		{
			if(pTex->m_descTex2D.MipLevels == 0)
			{
				debugMessage(GX_LOG_ERROR, "Unsupported: GX_TEXFLAG_INIT_ALL_MIPS is not supported with GX_TEXFLAG_AUTOGENMIPMAPS");
				mem_delete(pTex);
				return(NULL);
			}
			D3D11_SUBRESOURCE_DATA *pSubresourceData = (D3D11_SUBRESOURCE_DATA*)alloca(sizeof(D3D11_SUBRESOURCE_DATA) * pTex->m_descTex2D.MipLevels * pTex->m_descTex2D.ArraySize);
			GXImageMip *pData = (GXImageMip*)pInitData;

			for(UINT s = 0; s < pTex->m_descTex2D.ArraySize; ++s)
			{
				UINT uTmpSize = uSize;
				for(UINT i = 0; i < pTex->m_descTex2D.MipLevels; ++i)
				{
					UINT idx = i + pTex->m_descTex2D.MipLevels * s;
					pSubresourceData[idx].pSysMem = pData[idx].pData;
					pSubresourceData[idx].SysMemSlicePitch = 0;
					pSubresourceData[idx].SysMemPitch = getTextureMemPitch(uTmpSize, format);
					uTmpSize >>= 1;
					uTmpSize = max(1, uTmpSize);
					pTex->m_iTotalSize += (int)(pSubresourceData[i].SysMemPitch * uTmpSize);
					//addBytesTextures(pInitData[i].SysMemPitch * uTmpSize);
				}
			}
			DX_CALL(m_pDevice->CreateTexture2D(&pTex->m_descTex2D, pSubresourceData, &pTex->m_pTexture));
		}
		else
		{
			debugMessage(GX_LOG_WARN, "Not implemented: Unable to implace init cube texture");
		}
	}
	else
	{
		DX_CALL(m_pDevice->CreateTexture2D(&pTex->m_descTex2D, NULL, &pTex->m_pTexture));
	}

	memset(&pTex->m_descSRV, 0, sizeof(pTex->m_descSRV));
	pTex->m_descSRV.Format = pTex->m_descTex2D.Format;
	pTex->m_descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	pTex->m_descSRV.TextureCube.MipLevels = pTex->m_descTex2D.MipLevels;

	DX_CALL(m_pDevice->CreateShaderResourceView(pTex->m_pTexture, &pTex->m_descSRV, &pTex->m_pSRV));

	if(!pTex->m_iTotalSize)
	{
		pTex->m_iTotalSize = getTextureMemPitch(pTex->m_uSize, pTex->m_format) * pTex->m_uSize * 6;
	}

	m_pDirectContext->addBytesTextures(pTex->m_iTotalSize);
	addBytesTextures(pTex->m_iTotalSize, !!(pTex->m_descTex2D.BindFlags & D3D11_BIND_RENDER_TARGET));

	if(uTexUsageFlags & GX_TEXFLAG_UNORDERED_ACCESS)
	{
		memset(&pTex->m_descUAV, 0, sizeof(pTex->m_descUAV));
		pTex->m_descUAV.Format = DXGI_FORMAT_UNKNOWN;
		pTex->m_descUAV.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
		pTex->m_descUAV.Texture2DArray.ArraySize = 6;
		DX_CALL(m_pDevice->CreateUnorderedAccessView(pTex->m_pTexture, &pTex->m_descUAV, &pTex->m_pUAV));
	}

	return(pTex);*/
	assert(!"Not imlemented");
	return(NULL);
}
void CGXDevice::destroyTexture2D(IGXTexture2D *pTexture)
{
	/*if(pTexture)
	{
		if(((CGXTexture2D*)pTexture)->m_isResettable)
		{
			for(UINT i = 0, l = m_aResettableTextures2D.size(); i < l; ++i)
			{
				if(m_aResettableTextures2D[i] == pTexture)
				{
					m_aResettableTextures2D.erase(i);
					break;
				}
			}
		}
	}*/
}
void CGXDevice::destroyTextureCube(IGXTextureCube *pTexture)
{
	/*if(pTexture)
	{
		if(((CGXTextureCube*)pTexture)->m_isResettable)
		{
			for(UINT i = 0, l = m_aResettableTexturesCube.size(); i < l; ++i)
			{
				if(m_aResettableTexturesCube[i] == pTexture)
				{
					m_aResettableTexturesCube.erase(i);
					break;
				}
			}
		}
	}*/
}

IGXSwapChain* CGXDevice::createSwapChain(UINT uWidth, UINT uHeight, SXWINDOW wnd)
{
	return(createSwapChain(uWidth, uHeight, wnd, true));
}
IGXSwapChain* CGXDevice::createSwapChain(UINT uWidth, UINT uHeight, SXWINDOW wnd, bool bWindowed)
{
	/*CGXSwapChain *pSC = new CGXSwapChain(this, uWidth, uHeight, wnd, bWindowed);
	m_aResettableSwapChains.push_back(pSC);
	return(pSC);*/
	assert(!"Not imlemented");
	return(NULL);
}
void CGXDevice::destroySwapChain(IGXSwapChain *pSwapChain)
{
	if(pSwapChain)
	{
		for(UINT i = 0, l = m_aResettableSwapChains.size(); i < l; ++i)
		{
			if(pSwapChain == m_aResettableSwapChains[i])
			{
				m_aResettableSwapChains.erase(i);
				break;
			}
		}
	}
}


UINT CGXDevice::getTextureDataSize(UINT uPitch, UINT uHeight, GXFORMAT format)
{
	bool bc = true;
	int bcnumBytesPerBlock = 16;

	switch(format)
	{
	case GXFMT_DXT1:
		bc = true;
		bcnumBytesPerBlock = 8;
		break;
	case GXFMT_DXT3:
		bc = true;
		break;
	case GXFMT_DXT5:
		bc = true;
		break;

	case GXFMT_ATI1N:
		bc = true;
		bcnumBytesPerBlock = 8;
		break;
	case GXFMT_ATI2N:
		bc = true;
		break;

	default:
		bc = false;
	}

	if(bc)
	{
		int numBlocksHigh = 0;
		if(uHeight > 0)
		{
			numBlocksHigh = max(1, uHeight / 4);
		}
		return(uPitch * numBlocksHigh);
	}
	else
	{
		UINT bpp = getBitsPerPixel(format);
		//rowBytes = (width * bpp + 7) / 8; // round up to nearest byte
		return(uPitch * uHeight);
	}
}

UINT CGXDevice::getTextureMemPitch(UINT uWidth, GXFORMAT format)
{
	bool bc = true;
	int bcnumBytesPerBlock = 16;

	switch(format)
	{
	case GXFMT_DXT1:
		bc = true;
		bcnumBytesPerBlock = 8;
		break;
	case GXFMT_DXT3:
		bc = true;
		break;
	case GXFMT_DXT5:
		bc = true;
		break;

	case GXFMT_ATI1N:
		bc = true;
		bcnumBytesPerBlock = 8;
		break;
	case GXFMT_ATI2N:
		bc = true;
		break;

	default:
		bc = false;
	}

	if(bc)
	{
		int numBlocksWide = 0;
		if(uWidth > 0)
			numBlocksWide = max(1, (uWidth+3) / 4);
		return(numBlocksWide * bcnumBytesPerBlock);
	}
	else
	{
		UINT bpp = getBitsPerPixel(format);
		return((uWidth * bpp + 7) / 8); // round up to nearest byte
	}
}

UINT CGXDevice::getBitsPerPixel(GXFORMAT format)
{
	switch(format)
	{
	case GXFMT_A32B32G32R32F:  
		return(128);

	case GXFMT_G32R32F:
	case GXFMT_A16B16G16R16F:
	case GXFMT_A16B16G16R16:
		return(64);

	case GXFMT_R32F:  
	case GXFMT_G16R16F:
	case GXFMT_D24X8:
	case GXFMT_D24S8:
	case GXFMT_D32:
	case GXFMT_X8R8G8B8:
	case GXFMT_A8R8G8B8:
		return(32);
		
	case GXFMT_D16:
	case GXFMT_R16F:
	case GXFMT_R5G6B5:
		return(16);

	case GXFMT_DXT1:
	case GXFMT_ATI1N:
		return(4);

	case GXFMT_DXT3:
	case GXFMT_DXT5:
	case GXFMT_ATI2N:
		return(8);
	}
	return(0);
}

IGXTexture2D* CGXDevice::createTexture2DFromFile(const char *szFileName, UINT uTexUsageFlags, bool bAllowNonPowerOf2)
{
	/*CGXTexture2D *pTex = new CGXTexture2D(this);

	if(FAILED(DX_CALL(D3DX11CreateTextureFromFileA(m_pDevice, szFileName, NULL, NULL, (ID3D11Resource**)&(pTex->m_pTexture), NULL))))
	{
		mem_delete(pTex);
		return(NULL);
	}

	pTex->m_pTexture->GetDesc(&pTex->m_descTex2D);

	assert(pTex->m_descTex2D.ArraySize == 1);

	pTex->m_uHeight = pTex->m_descTex2D.Height;
	pTex->m_uWidth = pTex->m_descTex2D.Width;
	pTex->m_format = getGXFormat(pTex->m_descTex2D.Format);
	pTex->m_bWasReset = false;

	memset(&pTex->m_descSRV, 0, sizeof(pTex->m_descSRV));
	pTex->m_descSRV.Format = pTex->m_descTex2D.Format;
	pTex->m_descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	pTex->m_descSRV.Texture2D.MipLevels = pTex->m_descTex2D.MipLevels;

	DX_CALL(m_pDevice->CreateShaderResourceView(pTex->m_pTexture, &pTex->m_descSRV, &pTex->m_pSRV));
	
	m_pDirectContext->addBytesTextures(getTextureMemPitch(pTex->m_uWidth, pTex->m_format) * pTex->m_uHeight);
	addBytesTextures(getTextureMemPitch(pTex->m_uWidth, pTex->m_format) * pTex->m_uHeight, !!(pTex->m_descTex2D.BindFlags & D3D11_BIND_RENDER_TARGET));

	return(pTex);*/
	assert(!"Not imlemented");
	return(NULL);
}

IGXTextureCube* CGXDevice::createTextureCubeFromFile(const char *szFileName, UINT uTexUsageFlags, bool bAllowNonPowerOf2)
{
	/*CGXTextureCube *pTex = new CGXTextureCube(this);
	if(FAILED(DX_CALL(D3DX11CreateTextureFromFileA(m_pDevice, szFileName, NULL, NULL, (ID3D11Resource**)&(pTex->m_pTexture), NULL))))
	{
		mem_delete(pTex);
		return(NULL);
	}

	pTex->m_pTexture->GetDesc(&pTex->m_descTex2D);
	assert(pTex->m_descTex2D.ArraySize == 6);
	pTex->m_uSize = pTex->m_descTex2D.Height;
	pTex->m_format = getGXFormat(pTex->m_descTex2D.Format);
	pTex->m_bWasReset = false;

	memset(&pTex->m_descSRV, 0, sizeof(pTex->m_descSRV));
	pTex->m_descSRV.Format = pTex->m_descTex2D.Format;
	pTex->m_descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	pTex->m_descSRV.Texture2D.MipLevels = pTex->m_descTex2D.MipLevels;

	DX_CALL(m_pDevice->CreateShaderResourceView(pTex->m_pTexture, &pTex->m_descSRV, &pTex->m_pSRV));

	m_pDirectContext->addBytesTextures(getTextureMemPitch(pTex->m_uSize, pTex->m_format) * pTex->m_uSize);
	addBytesTextures(getTextureMemPitch(pTex->m_uSize, pTex->m_format) * pTex->m_uSize, !!(pTex->m_descTex2D.BindFlags & D3D11_BIND_RENDER_TARGET));

	return(pTex);*/
	assert(!"Not imlemented");
	return(NULL);
}

/*
DXGI_FORMAT CGXDevice::getDXFormat(GXFORMAT format)
{
	switch(format)
	{
	case GXFMT_A8R8G8B8:
		//return(DXGI_FORMAT_B8G8R8A8_UNORM);
		return(DXGI_FORMAT_R8G8B8A8_UNORM);
	case GXFMT_X8R8G8B8:
		return(DXGI_FORMAT_B8G8R8X8_UNORM);
	case GXFMT_R5G6B5:
		return(DXGI_FORMAT_B5G6R5_UNORM);
	case GXFMT_A16B16G16R16:
		return(DXGI_FORMAT_R16G16B16A16_UNORM);
	case GXFMT_DXT1:
		return(DXGI_FORMAT_BC1_UNORM);
	case GXFMT_DXT3:
		return(DXGI_FORMAT_BC2_UNORM);
	case GXFMT_DXT5:
		return(DXGI_FORMAT_BC3_UNORM);
	case GXFMT_ATI1N:
		return(DXGI_FORMAT_BC4_UNORM);
	case GXFMT_ATI2N:
		return(DXGI_FORMAT_BC5_UNORM);
	case GXFMT_D32:
		return(DXGI_FORMAT_D32_FLOAT);
	case GXFMT_D24S8:
		return(DXGI_FORMAT_D24_UNORM_S8_UINT);
	case GXFMT_D24X8:
		assert(!"GXFMT_D24X8 is unavailable!");
		return(DXGI_FORMAT_D24_UNORM_S8_UINT);
	case GXFMT_D16:
		return(DXGI_FORMAT_D16_UNORM);
	case GXFMT_R16F:
		return(DXGI_FORMAT_R16_FLOAT);
	case GXFMT_G16R16F:
		return(DXGI_FORMAT_R16G16_FLOAT);
	case GXFMT_A16B16G16R16F:
		return(DXGI_FORMAT_R16G16B16A16_FLOAT);
	case GXFMT_R32F:
		return(DXGI_FORMAT_R32_FLOAT);
	case GXFMT_G32R32F:
		return(DXGI_FORMAT_R32G32_FLOAT);
	case GXFMT_A32B32G32R32F:
		return(DXGI_FORMAT_R32G32B32A32_FLOAT);
	}
	assert(!"Unknown format!");
	return(DXGI_FORMAT_UNKNOWN);
}
GXFORMAT CGXDevice::getGXFormat(DXGI_FORMAT format)
{
	switch(format)
	{
	case DXGI_FORMAT_B8G8R8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
		return(GXFMT_A8R8G8B8);
	case DXGI_FORMAT_B8G8R8X8_UNORM:
		return(GXFMT_X8R8G8B8);
	case DXGI_FORMAT_B5G6R5_UNORM:
		return(GXFMT_R5G6B5);
	case DXGI_FORMAT_R16G16B16A16_UNORM:
		return(GXFMT_A16B16G16R16);
	case DXGI_FORMAT_BC1_UNORM:
		return(GXFMT_DXT1);
	case DXGI_FORMAT_BC2_UNORM:
		return(GXFMT_DXT3);
	case DXGI_FORMAT_BC3_UNORM:
		return(GXFMT_DXT5);
	case DXGI_FORMAT_BC4_UNORM:
		return(GXFMT_ATI1N);
	case DXGI_FORMAT_BC5_UNORM:
		return(GXFMT_ATI2N);
	case DXGI_FORMAT_D32_FLOAT:
		return(GXFMT_D32);
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
		return(GXFMT_D24S8);
	case DXGI_FORMAT_D16_UNORM:
		return(GXFMT_D16);
	case DXGI_FORMAT_R16_FLOAT:
		return(GXFMT_R16F);
	case DXGI_FORMAT_R16G16_FLOAT:
		return(GXFMT_G16R16F);
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
		return(GXFMT_A16B16G16R16F);
	case DXGI_FORMAT_R32_FLOAT:
		return(GXFMT_R32F);
	case DXGI_FORMAT_R32G32_FLOAT:
		return(GXFMT_G32R32F);
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
		return(GXFMT_A32B32G32R32F);
	}
	assert(!"Unknown format!");
	return(GXFMT_UNKNOWN);
}

ID3D11Device* CGXDevice::getDXDevice()
{
	return(m_pDevice);
}

ID3D11DeviceContext* CGXDevice::getDXDeviceContext()
{
	return(m_pDeviceContext);
}
*/

bool CGXDevice::saveTextureToFile(const char *szTarget, IGXBaseTexture *pTexture)
{
	/*ID3D11Texture2D *pTex = NULL;
	switch(pTexture->getType())
	{
	case GXTEXTURE_TYPE_2D:
		pTex = ((CGXTexture2D*)pTexture)->m_pTexture;
		break;
	case GXTEXTURE_TYPE_CUBE:
		pTex = ((CGXTextureCube*)pTexture)->m_pTexture;
		break;
	}
	return(!FAILED(DX_CALL(D3DX11SaveTextureToFileA(m_pDeviceContext, pTex, D3DX11_IFF_DDS, szTarget))));*/
	assert(!"Not imlemented");
	return(NULL);
}

/*
IDXGIFactory* CGXDevice::getDXGIFactory()
{
	return(m_pDXGIFactory);
}
*/

IGXConstantBuffer* CGXDevice::createConstantBuffer(UINT uSize)
{
	assert(uSize && (uSize % 16 == 0));

	//return(new CGXConstantBuffer(this, uSize));
	assert(!"Not imlemented");
	return(NULL);
}

IGXContext* CGXDevice::getThreadContext()
{
	//m_pDirectContext->AddRef();
	ID idThread = GXGetThreadID();

	if(!m_pContexts[idThread])
	{
		m_pContexts[idThread] = createIndirectContext();
		if(m_iLastIndirectContext < idThread)
		{
			m_iLastIndirectContext = idThread;
		}
	}

	return(m_pContexts[idThread]);
}

void CGXDevice::executeThreadContexts()
{
	assert(GXGetThreadID() == 0);

	for(int i = 1; i <= m_iLastIndirectContext; ++i)
	{
		if(m_pContexts[i])
		{
			m_pDirectContext->executeIndirectContext(m_pContexts[i]);
			mem_release(((CGXContext*)m_pContexts[i])->m_pCommandList);
		}
	}
}

IGXContext* CGXDevice::createIndirectContext()
{
	CGXContext *pCtx = NULL;
	//DX_CALL(m_pDevice->CreateDeferredContext(0, &pCtx));

	if(!pCtx)
	{
		return(NULL);
	}

	return(new CGXContext(pCtx, this, false));
}
