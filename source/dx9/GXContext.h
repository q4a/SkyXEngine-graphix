#ifndef _CGXContext_H_
#define _CGXContext_H_

#include <graphix/graphix.h>
#include <common/array.h>

#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr9.h>

#if defined(_WINDOWS)
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#	pragma comment(lib, "d3d9.lib")
#	pragma comment(lib, "DxErr9.lib")
#	pragma comment(lib, "d3dx9.lib")
#endif

enum GX_LOG
{
	GX_LOG_INFO,
	GX_LOG_WARN,
	GX_LOG_ERROR
};

#define GX_SYNCFLAG_NO_SHADER 0x00000001

#define DX_CALL(code) ([](HRESULT hr, const char *szCode){if(FAILED(hr)){CGXContext::logDXcall(szCode, hr);}return(hr);})((code), #code)

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

	IGXVertexDeclaration * createVertexDeclaration(const GXVertexElement * pDecl);
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

	IGXVertexShader * createVertexShader(const char * szFile, GXMacro *pDefs = NULL);
	IGXVertexShader * createVertexShaderFromString(const char * szCode, GXMacro *pDefs = NULL);
	IGXVertexShader * createVertexShader(void *pData, UINT uSize);
	void destroyVertexShader(IGXVertexShader * pSH);

	IGXPixelShader * createPixelShader(const char * szFile, GXMacro *pDefs = NULL);
	IGXPixelShader * createPixelShaderFromString(const char * szCode, GXMacro *pDefs = NULL);
	IGXPixelShader * createPixelShader(void *pData, UINT uSize);
	void destroyPixelShader(IGXPixelShader * pSH);

	//void setVertexShader(IGXVertexShader * pSH);
	//void setPixelShader(IGXPixelShader * pSH);

	IGXShader *createShader(IGXVertexShader *pVS = NULL, IGXPixelShader *pPS = NULL);
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
	IGXTextureCube *createTextureCube(UINT uSize, UINT uMipLevels, UINT uTexUsageFlags, GXFORMAT format, void * pInitData = NULL);
	void destroyTexture2D(IGXTexture2D * pTexture);
	void destroyTextureCube(IGXTextureCube * pTexture);

	IGXTexture2D *createTexture2DFromFile(const char *szFileName, UINT uTexUsageFlags, bool bAllowNonPowerOf2 = false);
	IGXTextureCube *createTextureCubeFromFile(const char *szFileName, UINT uTexUsageFlags, bool bAllowNonPowerOf2 = false);

	void setTexture(IGXBaseTexture *pTexture, UINT uStage = 0);
	IGXBaseTexture *getTexture(UINT uStage = 0);

	IGXBlendState *createBlendState(GXBlendDesc *pBlendDesc);
	void destroyBlendState(IGXBlendState *pState);
	void setBlendState(IGXBlendState *pState);
	IGXBlendState *getBlendState();
	void setBlendFactor(GXCOLOR val);

	IGXDepthStencilState *createDepthStencilState(GXDepthStencilDesc *pDSDesc);
	void destroyDepthStencilState(IGXDepthStencilState *pState);
	void setDepthStencilState(IGXDepthStencilState *pState);
	IGXDepthStencilState *getDepthStencilState();
	void setStencilRef(UINT uVal);

	IGXRasterizerState *createRasterizerState(GXRasterizerDesc *pDSDesc);
	void destroyRasterizerState(IGXRasterizerState *pState);
	void setRasterizerState(IGXRasterizerState *pState);
	IGXRasterizerState *getRasterizerState();
	void setScissorRect(int iTop, int iRight, int iBottom, int iLeft);

	IGXSamplerState *createSamplerState(GXSamplerDesc *pSamplerDesc);
	void destroySamplerState(IGXSamplerState *pState);
	void setSamplerState(IGXSamplerState *pState, UINT uSlot);
	IGXSamplerState *getSamplerState(UINT uSlot);

	IGXSwapChain *createSwapChain(UINT uWidth, UINT uHeight, SXWINDOW wnd);
	void destroySwapChain(IGXSwapChain *pSwapChain);

	static void debugMessage(GX_LOG, const char *msg);
	static void logDXcall(const char *szCodeString, HRESULT hr);

	IDirect3DDevice9 *getDXDevice();
	const D3DCAPS9 *getDXDeviceCaps();

	D3DFORMAT getDXFormat(GXFORMAT format);
	GXFORMAT getGXFormat(D3DFORMAT format);

	UINT getTextureDataSize(UINT uPitch, UINT uHeight, GXFORMAT format);
	UINT getBitsPerPixel(GXFORMAT format);

	GXTEXTURE_TYPE getTextureTypeFromFile(const char *szFile);
	bool saveTextureToFile(const char *szTarget, IGXBaseTexture *pTexture);

	const GXFrameStats *getFrameStats()
	{
		return(&m_frameStats);
	}

	void addBytesTextures(UINT uBytes)
	{
		m_frameStats.uUploadedBuffersTextures += uBytes;
	}
	void addBytesVertices(UINT uBytes)
	{
		m_frameStats.uUploadedBuffersVertexes += uBytes;
	}
	void addBytesIndices(UINT uBytes)
	{
		m_frameStats.uUploadedBuffersIndices += uBytes;
	}

protected:
	
	IDirect3D9 *m_pD3D = NULL;
	IDirect3DDevice9 *m_pDevice = NULL;
	D3DPRESENT_PARAMETERS m_oD3DAPP;
	D3DCAPS9 m_dxCaps;

	bool m_bDeviceLost = false;
	bool m_bDeviceWasReset = false;
	//bool m_bCanBeginFrameChecked = false;

	void onLostDevice();
	void onResetDevice();

	HWND m_hWnd;

	//IDSRvertexBuffer * m_pCurVertexBuffer[MAXDSGVSTREAM];
	IGXRenderBuffer * m_pCurRenderBuffer;
	IGXIndexBuffer * m_pCurIndexBuffer;

	IGXVertexDeclaration * m_pCurVertexDecl;
	D3DPRIMITIVETYPE m_drawPT;

	IGXSamplerState * m_pSamplerState[GX_MAX_SAMPLERS];
	DWORD m_dwCurrentSamplerStates[GX_MAX_SAMPLERS][D3DSAMP_SRGBTEXTURE];
	IGXSamplerState *m_pDefaultSamplerState = NULL;

	IGXRasterizerState *m_pRasterizerState = NULL;
	IGXRasterizerState *m_pDefaultRasterizerState = NULL;

	IGXDepthStencilState *m_pDepthStencilState = NULL;
	IGXDepthStencilState *m_pDefaultDepthStencilState = NULL;
	UINT m_uStencilRef = 0;

	IGXBlendState *m_pBlendState = NULL;
	IGXBlendState *m_pDefaultBlendState = NULL;
	GXCOLOR m_blendFactor = GX_COLOR_ARGB(255, 255, 255, 255);

	IGXDepthStencilSurface *m_pDepthStencilSurface = NULL;
	IDirect3DSurface9 *m_pDefaultDepthStencilSurface = NULL;

	IGXSurface *m_pColorTarget[GX_MAX_COLORTARGETS];
	IDirect3DSurface9 *m_pDefaultColorTarget = NULL;

	IGXBaseTexture *m_pTextures[GX_MAX_TEXTURES];

	IGXShader *m_pShader = NULL;

	struct _sync_state
	{
		//BOOL bVertexLayout;
		BOOL bIndexBuffer;
		BOOL bRenderBuffer;
		BOOL bSamplerState[GX_MAX_SAMPLERS];
		BOOL bRasterizerState;
		BOOL bDepthStencilState;
		BOOL bStencilRef;
		BOOL bBlendState;
		BOOL bBlendFactor;
		BOOL bDepthStencilSurface;
		BOOL bColorTarget[GX_MAX_COLORTARGETS];
		BOOL bTexture[GX_MAX_TEXTURES];
		BOOL bShader;
		BOOL bScissorsRect;
		//BOOL bVertexBuffers[MAXDSGVSTREAM];
	};

	_sync_state m_sync_state;

	void syncronize(UINT flags=0);
	UINT getPTcount(UINT idxCount);
	UINT getIndexSize(D3DFORMAT idx);

	Array<IGXVertexBuffer*> m_aResettableVertexBuffers;
	Array<IGXIndexBuffer*> m_aResettableIndexBuffers;
	Array<IGXTexture2D*> m_aResettableTextures2D;
	Array<IGXTextureCube*> m_aResettableTexturesCube;
	Array<IGXDepthStencilSurface*> m_aResettableDSSurfaces;
	Array<IGXSurface*> m_aResettableColorSurfaces;
	Array<IGXDepthStencilState*> m_aResettableDepthStencilStates;
	Array<IGXBlendState*> m_aResettableBlendStates;
	Array<IGXRasterizerState*> m_aResettableRasterizerStates;
	Array<IGXSwapChain*> m_aResettableSwapChains;

	BOOL m_isScissorsEnable = 0;
	RECT m_rcScissors;

	void _beginInstancing(UINT uInstanceCount);
	void _endInstancing();
	void _updateStats(UINT uPrimCount);

	GXFrameStats m_frameStats;
};

#endif
