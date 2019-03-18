#ifndef _CGXContext_H_
#define _CGXContext_H_

#include <graphix/graphix.h>
#include <common/array.h>

#include <d3d11.h>
#include <D3DX11tex.h>
#include <DxErr.h>
#include <d3dcompiler.h>

#if defined(_WINDOWS)
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#	pragma comment(lib, "d3d11.lib")
#	pragma comment(lib, "d3dx11.lib")
#	pragma comment(lib, "DxErr.lib")
#	pragma comment(lib, "d3dcompiler.lib")
#endif

enum GX_LOG
{
	GX_LOG_INFO,
	GX_LOG_WARN,
	GX_LOG_ERROR
};

#define DX_CALL(code) ([](HRESULT hr, const char *szCode){if(FAILED(hr)){CGXContext::logDXcall(szCode, hr);}return(hr);})((code), #code)

/*
Applications are also free to request that Visual Studio 
not support capturing them via an API, which is possible 
here as well. This is done via the D3DPerf_SetOptions(1) 
method, use of the D3D11_CREATE_DEVICE_PREVENT_ALTERING_LAYER_SETTINGS_FROM_REGISTRY 
flag, or by reacting to the value returned from 
IsAnnotationEnabled:GetStatus or ID3D11DeviceContext2::IsAnnotationEnabled.
*/

class CGXContext: public IGXContext
{
protected:
	~CGXContext();
public:
	BOOL initContext(SXWINDOW wnd, int iWidth, int iHeight, bool isWindowed);
	CGXContext();
	void Release();

	void resize(int iWidth, int iHeight, bool isWindowed);

	void swapBuffers();

	bool beginFrame();
	void endFrame();
	bool canBeginFrame();

	bool wasReset();

	void clear(UINT what, GXCOLOR color = 0, float fDepth = 1.0f, UINT uStencil = 0);

	IGXVertexBuffer * createVertexBuffer(size_t size, UINT flags, void * pInitData = NULL);
	IGXIndexBuffer * createIndexBuffer(size_t size, UINT flags, GXINDEXTYPE it, void * pInitData = NULL);
	
	void destroyIndexBuffer(IGXIndexBuffer * pBuff);
	void destroyVertexBuffer(IGXVertexBuffer * pBuff);

	IGXVertexDeclaration * createVertexDeclaration(const GXVERTEXELEMENT * pDecl);
	void destroyVertexDeclaration(IGXVertexDeclaration * pDecl);

	void setIndexBuffer(IGXIndexBuffer * pBuff);
	void setRenderBuffer(IGXRenderBuffer * pBuff);
	//void setVertexBuffers(UINT startSlot, UINT countSlots, IDSRvertexBuffer ** pBuff);

	//void setInputLayout(IDSGvertexDeclaration * pDecl);

	void drawIndexed(UINT uVertexCount, UINT uPrimitiveCount, UINT uStartIndexLocation, int iBaseVertexLocation);
	void drawIndexedInstanced(UINT uInstanceCount, UINT uVertexCount, UINT uPrimitiveCount, UINT uStartIndexLocation, int iBaseVertexLocation);
	void drawPrimitive(UINT uStartVertex, UINT uPrimitiveCount);
	void drawPrimitiveInstanced(UINT uInstanceCount, UINT uStartVertex, UINT uPrimitiveCount);

	void setPrimitiveTopology(GXPT pt);

	//IGXShader * createShader(const char * pName, UINT flags = 0);
	//void destroyShader(IGXShader * pSH);
	//void setShader(IGXShader * pSH);

	IGXVertexShader * createVertexShader(const char * szFile, GXMACRO *pDefs = NULL);
	IGXVertexShader * createVertexShaderFromString(const char * szCode, GXMACRO *pDefs = NULL);
	IGXVertexShader * createVertexShader(void *pData, UINT uSize);
	void destroyVertexShader(IGXVertexShader * pSH);
	void setVertexShaderConstant(IGXConstantBuffer *pBuffer, UINT uSlot = 0);

	IGXPixelShader * createPixelShader(const char * szFile, GXMACRO *pDefs = NULL);
	IGXPixelShader * createPixelShaderFromString(const char * szCode, GXMACRO *pDefs = NULL);
	IGXPixelShader * createPixelShader(void *pData, UINT uSize);
	void destroyPixelShader(IGXPixelShader * pSH); 
	void setPixelShaderConstant(IGXConstantBuffer *pBuffer, UINT uSlot = 0);

	IGXGeometryShader * createGeometryShader(const char * szFile, GXMACRO *pDefs = NULL);
	IGXGeometryShader * createGeometryShaderFromString(const char * szCode, GXMACRO *pDefs = NULL);
	IGXGeometryShader * createGeometryShader(void *pData, UINT uSize);
	void destroyGeometryShader(IGXGeometryShader * pSH);
	void setGeometryShaderConstant(IGXConstantBuffer *pBuffer, UINT uSlot = 0);


	//void setVertexShader(IGXVertexShader * pSH);
	//void setPixelShader(IGXPixelShader * pSH);

	IGXShader *createShader(IGXVertexShader *pVS = NULL, IGXPixelShader *pPS = NULL, IGXGeometryShader *pGS = NULL);
	void destroyShader(IGXShader *pSH);
	void setShader(IGXShader *pSH);
	IGXShader *getShader();

	IGXRenderBuffer * createRenderBuffer(UINT countSlots, IGXVertexBuffer ** pBuff, IGXVertexDeclaration * pDecl);
	void destroyRenderBuffer(IGXRenderBuffer * pDecl);

	IGXDepthStencilSurface *createDepthStencilSurface(UINT uWidth, UINT uHeight, GXFORMAT format, GXMULTISAMPLE_TYPE multisampleType, bool bAutoResize = false);
	void destroyDepthStencilSurface(IGXDepthStencilSurface *pSurface);
	void setDepthStencilSurface(IGXDepthStencilSurface *pSurface);
	IGXDepthStencilSurface *getDepthStencilSurface();

	IGXSurface *createColorTarget(UINT uWidth, UINT uHeight, GXFORMAT format, GXMULTISAMPLE_TYPE multisampleType, bool bAutoResize = false);
	void destroyColorTarget(IGXSurface *pSurface);
	void downsampleColorTarget(IGXSurface *pSource, IGXSurface *pTarget);

	void setColorTarget(IGXSurface *pSurf, UINT idx = 0);
	IGXSurface *getColorTarget(UINT idx = 0);

	IGXTexture2D *createTexture2D(UINT uWidth, UINT uHeight, UINT uMipLevels, UINT uTexUsageFlags, GXFORMAT format, void * pInitData = NULL);
	IGXTexture3D *createTexture3D(UINT uWidth, UINT uHeight, UINT uDepth, UINT uMipLevels, UINT uTexUsageFlags, GXFORMAT format, void * pInitData = NULL);
	IGXTextureCube *createTextureCube(UINT uSize, UINT uMipLevels, UINT uTexUsageFlags, GXFORMAT format, void * pInitData = NULL);
	void destroyTexture2D(IGXTexture2D * pTexture);
	void destroyTextureCube(IGXTextureCube * pTexture);

	IGXTexture2D *createTexture2DFromFile(const char *szFileName, UINT uTexUsageFlags, bool bAllowNonPowerOf2 = false);
	IGXTextureCube *createTextureCubeFromFile(const char *szFileName, UINT uTexUsageFlags, bool bAllowNonPowerOf2 = false);

	void setTexture(IGXBaseTexture *pTexture, UINT uStage = 0);
	IGXBaseTexture *getTexture(UINT uStage = 0);

	IGXBlendState *createBlendState(GXBLEND_DESC *pBlendDesc);
	void destroyBlendState(IGXBlendState *pState);
	void setBlendState(IGXBlendState *pState);
	IGXBlendState *getBlendState();
	void setBlendFactor(GXCOLOR val);

	IGXDepthStencilState *createDepthStencilState(GXDEPTH_STENCIL_DESC *pDSDesc);
	void destroyDepthStencilState(IGXDepthStencilState *pState);
	void setDepthStencilState(IGXDepthStencilState *pState);
	IGXDepthStencilState *getDepthStencilState();
	void setStencilRef(UINT uVal);

	IGXRasterizerState *createRasterizerState(GXRASTERIZER_DESC *pDSDesc);
	void destroyRasterizerState(IGXRasterizerState *pState);
	void setRasterizerState(IGXRasterizerState *pState);
	IGXRasterizerState *getRasterizerState();
	void setScissorRect(int iTop, int iRight, int iBottom, int iLeft);

	IGXSamplerState *createSamplerState(GXSAMPLER_DESC *pSamplerDesc);
	void destroySamplerState(IGXSamplerState *pState);
	void setSamplerState(IGXSamplerState *pState, UINT uSlot);
	IGXSamplerState *getSamplerState(UINT uSlot);

	IGXSwapChain *createSwapChain(UINT uWidth, UINT uHeight, SXWINDOW wnd);
	IGXSwapChain *createSwapChain(UINT uWidth, UINT uHeight, SXWINDOW wnd, bool bWindowed);
	void destroySwapChain(IGXSwapChain *pSwapChain);

	IGXConstantBuffer *createConstantBuffer(UINT uSize);

	static void debugMessage(GX_LOG, const char *msg);
	static void logDXcall(const char *szCodeString, HRESULT hr);

	
	ID3D11Device *getDXDevice();
	ID3D11DeviceContext *getDXDeviceContext();
	IDXGIFactory *getDXGIFactory();
	//const D3DCAPS9 *getDXDeviceCaps();

	DXGI_FORMAT getDXFormat(GXFORMAT format);
	GXFORMAT getGXFormat(DXGI_FORMAT format);

	UINT getTextureDataSize(UINT uPitch, UINT uHeight, GXFORMAT format);
	UINT getTextureMemPitch(UINT uWidth, GXFORMAT format);
	UINT getBitsPerPixel(GXFORMAT format);

	GXTEXTURE_TYPE getTextureTypeFromFile(const char *szFile);
	bool saveTextureToFile(const char *szTarget, IGXBaseTexture *pTexture);

	const GX_FRAME_STATS *getFrameStats()
	{
		return(&m_frameStats);
	}
	const GX_MEMORY_STATS *getMemoryStats()
	{
		return(&m_memoryStats);
	}
	const GX_ADAPTER_DESC *getAdapterDesc()
	{
		return(&m_adapterDesc);
	}

	void addBytesTextures(int uBytes, bool bTotal = false, bool isRT = false)
	{
		if(bTotal)
		{
			if(isRT)
			{
				m_memoryStats.uRenderTargetBytes += uBytes;
			}
			else
			{
				m_memoryStats.uTextureBytes += uBytes;
			}

			if(uBytes < 0)
			{
				uBytes = 0;
			}
		}
		m_frameStats.uUploadedBuffersTextures += uBytes;
	}
	void addBytesVertices(int uBytes)
	{
		m_frameStats.uUploadedBuffersVertexes += uBytes;
	}
	void addBytesIndices(UINT uBytes)
	{
		m_frameStats.uUploadedBuffersIndices += uBytes;
	}
	void addBytesShaderConst(int uBytes, bool bTotal = false)
	{
		if(bTotal)
		{
			m_memoryStats.uShaderConstBytes += uBytes;

			if(uBytes < 0)
			{
				uBytes = 0;
			}
		}
		m_frameStats.uUploadedBuffersShaderConst += uBytes;
	}
protected:
	
	ID3D11Device *m_pDevice = NULL;
	ID3D11DeviceContext *m_pDeviceContext = NULL;
	IDXGIDevice *m_pDXGIDevice = NULL;
	IDXGIFactory *m_pDXGIFactory = NULL;
	//D3DPRESENT_PARAMETERS m_oD3DAPP;
	//D3DCAPS9 m_dxCaps;
	UINT m_uWindowWidth;
	UINT m_uWindowHeight;
	BOOL m_isWindowed;
	
	void onLostDevice();
	void onResetDevice();

	GX_FRAME_STATS m_frameStats;
	GX_MEMORY_STATS m_memoryStats;
	GX_ADAPTER_DESC m_adapterDesc;

	HWND m_hWnd;

	//IDSRvertexBuffer * m_pCurVertexBuffer[MAXDSGVSTREAM];
	IGXRenderBuffer * m_pCurRenderBuffer;
	IGXIndexBuffer * m_pCurIndexBuffer;

	IGXVertexDeclaration * m_pCurVertexDecl;
	D3D_PRIMITIVE_TOPOLOGY m_drawPT;

	IGXSamplerState * m_pSamplerState[MAX_GXSAMPLERS];
	//DWORD m_dwCurrentSamplerStates[MAX_GXSAMPLERS][D3DSAMP_SRGBTEXTURE];
	IGXSamplerState *m_pDefaultSamplerState = NULL;

	IGXRasterizerState *m_pRasterizerState = NULL;
	IGXRasterizerState *m_pDefaultRasterizerState = NULL;

	IGXDepthStencilState *m_pDepthStencilState = NULL;
	IGXDepthStencilState *m_pDefaultDepthStencilState = NULL;
	UINT m_uStencilRef = 0;

	IGXBlendState *m_pBlendState = NULL;
	IGXBlendState *m_pDefaultBlendState = NULL;
	GXCOLOR m_blendFactor = GXCOLOR_ARGB(255, 255, 255, 255);

	IGXDepthStencilSurface *m_pDepthStencilSurface = NULL;
	IGXDepthStencilSurface *m_pDefaultDepthStencilSurface = NULL;

	IGXSurface *m_pColorTarget[MAXGXCOLORTARGETS];
	ID3D11RenderTargetView *m_pDXColorTarget[MAXGXCOLORTARGETS];
	//ID3D11RenderTargetView *m_pDefaultColorTarget = NULL;

	IGXSwapChain *m_pDefaultSwapChain = NULL;

	IGXBaseTexture *m_pTextures[MAXGXTEXTURES];

	IGXShader *m_pShader = NULL;

	struct _sync_state
	{
		//BOOL bVertexLayout;
		BOOL bIndexBuffer;
		BOOL bRenderBuffer;
		BOOL bSamplerState[MAX_GXSAMPLERS];
		BOOL bRasterizerState;
		BOOL bDepthStencilState;
		BOOL bBlendState;
		BOOL bRenderTarget;
		BOOL bTexture[MAXGXTEXTURES];
		BOOL bShader;
		BOOL bScissorsRect;
		//BOOL bVertexBuffers[MAXDSGVSTREAM];
	};

	_sync_state m_sync_state;
	bool m_bDeviceWasReset = false;

	void syncronize(UINT flags=0);
	UINT getPTcount(UINT idxCount);
	UINT getIDXcount(UINT ptCount);
	UINT getIndexSize(DXGI_FORMAT idx);
	void _updateStats(UINT uPrimCount);

	Array<IGXTexture2D*> m_aResettableTextures2D;
	Array<IGXTextureCube*> m_aResettableTexturesCube;
	Array<IGXDepthStencilSurface*> m_aResettableDSSurfaces;
	Array<IGXSurface*> m_aResettableColorSurfaces;
	Array<IGXSwapChain*> m_aResettableSwapChains;

	BOOL m_isScissorsEnable = 0;
	RECT m_rcScissors;

};

#endif
