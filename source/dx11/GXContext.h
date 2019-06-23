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
	BOOL initContext(SXWINDOW wnd, int iWidth, int iHeight, bool isWindowed) override;
	CGXContext();
	void Release() override;

	void resize(int iWidth, int iHeight, bool isWindowed) override;

	void swapBuffers() override;

	bool beginFrame() override;
	void endFrame() override;
	bool canBeginFrame() override;

	bool wasReset() override;

	void clear(UINT what, GXCOLOR color = 0, float fDepth = 1.0f, UINT uStencil = 0) override;

	IGXVertexBuffer* createVertexBuffer(size_t size, GXBUFFER_USAGE usage, void *pInitData = NULL) override;
	IGXIndexBuffer* createIndexBuffer(size_t size, GXBUFFER_USAGE usage, GXINDEXTYPE it, void *pInitData = NULL) override;
	
	void destroyIndexBuffer(IGXIndexBuffer * pBuff);
	void destroyVertexBuffer(IGXVertexBuffer * pBuff);

	IGXVertexDeclaration* createVertexDeclaration(const GXVertexElement *pDecl) override;
	void destroyVertexDeclaration(IGXVertexDeclaration * pDecl);

	void setIndexBuffer(IGXIndexBuffer *pBuff) override;
	void setRenderBuffer(IGXRenderBuffer *pBuff) override;
	//void setVertexBuffers(UINT startSlot, UINT countSlots, IDSRvertexBuffer ** pBuff);

	//void setInputLayout(IDSGvertexDeclaration * pDecl);

	void drawIndexed(UINT uVertexCount, UINT uPrimitiveCount, UINT uStartIndexLocation, int iBaseVertexLocation) override;
	void drawIndexedInstanced(UINT uInstanceCount, UINT uVertexCount, UINT uPrimitiveCount, UINT uStartIndexLocation, int iBaseVertexLocation) override;
	void drawPrimitive(UINT uStartVertex, UINT uPrimitiveCount) override;
	void drawPrimitiveInstanced(UINT uInstanceCount, UINT uStartVertex, UINT uPrimitiveCount);

	void computeDispatch(UINT uThreadGroupCountX, UINT uThreadGroupCountY, UINT uThreadGroupCountZ) override;

	void setPrimitiveTopology(GXPRIMITIVETOPOLOGY pt) override;

	//IGXShader * createShader(const char * pName, UINT flags = 0);
	//void destroyShader(IGXShader * pSH);
	//void setShader(IGXShader * pSH);

	IGXVertexShader* createVertexShader(const char *szFile, GXMacro *pDefs = NULL) override;
	IGXVertexShader* createVertexShaderFromString(const char * szCode, GXMacro *pDefs = NULL) override;
	IGXVertexShader* createVertexShaderFromBin(void *pData, UINT uSize) override;
	void setVertexShaderConstant(IGXConstantBuffer *pBuffer, UINT uSlot = 0) override;

	IGXPixelShader* createPixelShader(const char *szFile, GXMacro *pDefs = NULL) override;
	IGXPixelShader* createPixelShaderFromString(const char * szCode, GXMacro *pDefs = NULL) override;
	IGXPixelShader* createPixelShaderFromBin(void *pData, UINT uSize) override;
	void setPixelShaderConstant(IGXConstantBuffer *pBuffer, UINT uSlot = 0) override;

	IGXGeometryShader* createGeometryShader(const char *szFile, GXMacro *pDefs = NULL) override;
	IGXGeometryShader* createGeometryShaderFromString(const char * szCode, GXMacro *pDefs = NULL) override;
	IGXGeometryShader* createGeometryShaderFromBin(void *pData, UINT uSize) override;
	void setGeometryShaderConstant(IGXConstantBuffer *pBuffer, UINT uSlot = 0) override;

	IGXComputeShader* createComputeShader(const char *szFile, GXMacro *pDefs = NULL) override;
	IGXComputeShader* createComputeShaderFromString(const char * szCode, GXMacro *pDefs = NULL) override;
	IGXComputeShader* createComputeShaderFromBin(void *pData, UINT uSize) override;
	void setComputeShaderConstant(IGXConstantBuffer *pBuffer, UINT uSlot = 0) override;


	//void setVertexShader(IGXVertexShader * pSH);
	//void setPixelShader(IGXPixelShader * pSH);

	IGXShaderSet* createShader(IGXVertexShader *pVS = NULL, IGXPixelShader *pPS = NULL, IGXGeometryShader *pGS = NULL, IGXComputeShader *pCS = NULL) override;
	void setShader(IGXShaderSet *pSH) override;
	IGXShaderSet* getShader() override;

	IGXRenderBuffer* createRenderBuffer(UINT countSlots, IGXVertexBuffer ** pBuff, IGXVertexDeclaration *pDecl) override;
	void destroyRenderBuffer(IGXRenderBuffer * pDecl);

	IGXDepthStencilSurface* createDepthStencilSurface(UINT uWidth, UINT uHeight, GXFORMAT format, GXMULTISAMPLE_TYPE multisampleType, bool bAutoResize = false) override;
	IGXDepthStencilSurface* createDepthStencilSurfaceCube(UINT uSize, GXFORMAT format, GXMULTISAMPLE_TYPE multisampleType, bool bAutoResize = false) override;
	void destroyDepthStencilSurface(IGXDepthStencilSurface *pSurface);
	void setDepthStencilSurface(IGXDepthStencilSurface *pSurface) override;
	void setDepthStencilSurfaceNULL() override;
	IGXDepthStencilSurface* getDepthStencilSurface() override;

	IGXSurface* createColorTarget(UINT uWidth, UINT uHeight, GXFORMAT format, GXMULTISAMPLE_TYPE multisampleType, bool bAutoResize = false) override;
	void destroyColorTarget(IGXSurface *pSurface);
	void downsampleColorTarget(IGXSurface *pSource, IGXSurface *pTarget) override;

	void setColorTarget(IGXSurface *pSurf, UINT idx = 0) override;
	IGXSurface* getColorTarget(UINT idx = 0) override;

	IGXTexture2D* createTexture2D(UINT uWidth, UINT uHeight, UINT uMipLevels, UINT uTexUsageFlags, GXFORMAT format, void * pInitData = NULL) override;
	IGXTexture3D* createTexture3D(UINT uWidth, UINT uHeight, UINT uDepth, UINT uMipLevels, UINT uTexUsageFlags, GXFORMAT format, void * pInitData = NULL) override;
	IGXTextureCube* createTextureCube(UINT uSize, UINT uMipLevels, UINT uTexUsageFlags, GXFORMAT format, void * pInitData = NULL) override;
	void destroyTexture2D(IGXTexture2D * pTexture);
	void destroyTextureCube(IGXTextureCube * pTexture);

	IGXTexture2D* createTexture2DFromFile(const char *szFileName, UINT uTexUsageFlags, bool bAllowNonPowerOf2 = false) override;
	IGXTextureCube* createTextureCubeFromFile(const char *szFileName, UINT uTexUsageFlags, bool bAllowNonPowerOf2 = false) override;

	void setTexture(IGXBaseTexture *pTexture, UINT uStage = 0) override;
	IGXBaseTexture* getTexture(UINT uStage = 0) override;
	void setTextureVS(IGXBaseTexture *pTexture, UINT uStage = 0) override;
	IGXBaseTexture* getTextureVS(UINT uStage = 0) override;
	void setTextureCS(IGXBaseTexture *pTexture, UINT uStage = 0) override;
	IGXBaseTexture* getTextureCS(UINT uStage = 0) override;
	void setUnorderedAccessViewCS(IGXBaseTexture *pUAV, UINT uStage = 0) override;
	IGXBaseTexture* getUnorderedAccessViewCS(UINT uStage = 0) override;
	
	IGXBlendState* createBlendState(GXBlendDesc *pBlendDesc) override;
	void destroyBlendState(IGXBlendState *pState);
	void setBlendState(IGXBlendState *pState) override;
	IGXBlendState* getBlendState() override;
	void setBlendFactor(GXCOLOR val) override;

	IGXDepthStencilState* createDepthStencilState(GXDepthStencilDesc *pDSDesc) override;
	void destroyDepthStencilState(IGXDepthStencilState *pState);
	void setDepthStencilState(IGXDepthStencilState *pState) override;
	IGXDepthStencilState* getDepthStencilState() override;
	void setStencilRef(UINT uVal) override;

	IGXRasterizerState* createRasterizerState(GXRasterizerDesc *pDSDesc) override;
	void destroyRasterizerState(IGXRasterizerState *pState);
	void setRasterizerState(IGXRasterizerState *pState) override;
	IGXRasterizerState* getRasterizerState() override;
	void setScissorRect(int iTop, int iRight, int iBottom, int iLeft) override;

	IGXSamplerState* createSamplerState(GXSamplerDesc *pSamplerDesc) override;
	void destroySamplerState(IGXSamplerState *pState);
	void setSamplerState(IGXSamplerState *pState, UINT uSlot) override;
	IGXSamplerState* getSamplerState(UINT uSlot) override;

	IGXSwapChain* createSwapChain(UINT uWidth, UINT uHeight, SXWINDOW wnd) override;
	IGXSwapChain* createSwapChain(UINT uWidth, UINT uHeight, SXWINDOW wnd, bool bWindowed);
	void destroySwapChain(IGXSwapChain *pSwapChain);

	IGXConstantBuffer* createConstantBuffer(UINT uSize) override;

	static void debugMessage(GX_LOG, const char *msg);
	static void logDXcall(const char *szCodeString, HRESULT hr);

	
	ID3D11Device* getDXDevice();
	ID3D11DeviceContext* getDXDeviceContext();
	IDXGIFactory* getDXGIFactory();
	//const D3DCAPS9 *getDXDeviceCaps();

	DXGI_FORMAT getDXFormat(GXFORMAT format);
	GXFORMAT getGXFormat(DXGI_FORMAT format);

	UINT getTextureDataSize(UINT uPitch, UINT uHeight, GXFORMAT format);
	UINT getTextureMemPitch(UINT uWidth, GXFORMAT format);
	UINT getBitsPerPixel(GXFORMAT format);

	GXTEXTURE_TYPE getTextureTypeFromFile(const char *szFile) override;
	bool saveTextureToFile(const char *szTarget, IGXBaseTexture *pTexture) override;

	const GXFrameStats* getFrameStats() override
	{
		return(&m_frameStats);
	}
	const GXAdapterMemoryStats* getMemoryStats() override
	{
		return(&m_memoryStats);
	}
	const GXAdapterDesc* getAdapterDesc() override
	{
		return(&m_adapterDesc);
	}

	void addBytesTextures(int uBytes, bool bTotal = false, bool isRT = false)
	{
		if(bTotal)
		{
			if(isRT)
			{
				m_memoryStats.sizeRenderTargetBytes += uBytes;
			}
			else
			{
				m_memoryStats.sizeTextureBytes += uBytes;
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
			m_memoryStats.sizeShaderConstBytes += uBytes;

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

	GXFrameStats m_frameStats;
	GXAdapterMemoryStats m_memoryStats;
	GXAdapterDesc m_adapterDesc;

	HWND m_hWnd;

	//IDSRvertexBuffer * m_pCurVertexBuffer[MAXDSGVSTREAM];
	IGXRenderBuffer *m_pCurRenderBuffer;
	IGXIndexBuffer *m_pCurIndexBuffer;

	IGXVertexDeclaration *m_pCurVertexDecl;
	D3D_PRIMITIVE_TOPOLOGY m_drawPT;

	IGXSamplerState *m_pSamplerState[GX_MAX_SAMPLERS];
	//DWORD m_dwCurrentSamplerStates[GX_MAX_SAMPLERS][D3DSAMP_SRGBTEXTURE];
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
	IGXDepthStencilSurface *m_pDefaultDepthStencilSurface = NULL;

	IGXSurface *m_pColorTarget[GX_MAX_COLORTARGETS];
	ID3D11RenderTargetView *m_pDXColorTarget[GX_MAX_COLORTARGETS];
	//ID3D11RenderTargetView *m_pDefaultColorTarget = NULL;

	IGXSwapChain *m_pDefaultSwapChain = NULL;

	IGXBaseTexture *m_pTextures[GX_MAX_TEXTURES];
	IGXBaseTexture *m_pTexturesVS[GX_MAX_TEXTURES];
	IGXBaseTexture *m_pTexturesCS[GX_MAX_TEXTURES];
	IGXBaseTexture *m_pUAVsCS[GX_MAX_UAV_TEXTURES];

	IGXShaderSet *m_pShader = NULL;

	struct _sync_state
	{
		//BOOL bVertexLayout;
		BOOL bIndexBuffer;
		BOOL bRenderBuffer;
		BOOL bSamplerState[GX_MAX_SAMPLERS];
		BOOL bRasterizerState;
		BOOL bDepthStencilState;
		BOOL bBlendState;
		BOOL bRenderTarget;
		BOOL bTexture[GX_MAX_TEXTURES];
		BOOL bTextureVS[GX_MAX_TEXTURES];
		BOOL bTextureCS[GX_MAX_TEXTURES];
		BOOL bUAVsCS[GX_MAX_UAV_TEXTURES];
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
