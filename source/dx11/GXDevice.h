#ifndef _GXDEVICE_H_
#define _GXDEVICE_H_

#include "../../graphix.h"
#include <common/array.h>

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <D3DX11tex.h>
#include <DxErr.h>
#include <d3dcompiler.h>

// #include <Windows.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "DxErr.lib")
#pragma comment(lib, "d3dcompiler.lib")
#if _MSC_VER >= 1900
#	pragma comment(lib, "legacy_stdio_definitions.lib")
#endif


enum GX_LOG
{
	GX_LOG_INFO,
	GX_LOG_WARN,
	GX_LOG_ERROR
};

#define DX_CALL(code) ([](HRESULT hr, const char *szCode){if(FAILED(hr)){CGXDevice::logDXcall(szCode, hr);}return(hr);})((code), #code)

/*
Applications are also free to request that Visual Studio 
not support capturing them via an API, which is possible 
here as well. This is done via the D3DPerf_SetOptions(1) 
method, use of the D3D11_CREATE_DEVICE_PREVENT_ALTERING_LAYER_SETTINGS_FROM_REGISTRY 
flag, or by reacting to the value returned from 
IsAnnotationEnabled:GetStatus or ID3D11DeviceContext2::IsAnnotationEnabled.
*/

class CGXIndexBuffer;
class CGXVertexBuffer;
class CGXDevice: public IGXDevice
{
	friend class CGXContext;
protected:
	~CGXDevice();
public:
	BOOL initContext(SXWINDOW wnd, int iWidth, int iHeight, bool isWindowed) override;
	CGXDevice();
	void Release() override;
	void setLogger(IGXLogger *pLogger) override;

	void resize(int iWidth, int iHeight, bool isWindowed) override;

	IGXContext* getThreadContext() override;
	void executeThreadContexts() override;
	IGXContext* createIndirectContext() override;
	
	IGXSwapChain* createSwapChain(UINT uWidth, UINT uHeight, SXWINDOW wnd) override;
	IGXSwapChain* createSwapChain(UINT uWidth, UINT uHeight, SXWINDOW wnd, bool bWindowed);
	void destroySwapChain(IGXSwapChain *pSwapChain);

	void swapBuffers() override;

	IGXVertexBuffer* createVertexBuffer(size_t size, GXBUFFER_USAGE usage, void *pInitData = NULL) override;
	IGXIndexBuffer* createIndexBuffer(size_t size, GXBUFFER_USAGE usage, GXINDEXTYPE it, void *pInitData = NULL) override;
	
	void destroyIndexBuffer(CGXIndexBuffer * pBuff);
	void destroyVertexBuffer(CGXVertexBuffer * pBuff);

	IGXVertexDeclaration* createVertexDeclaration(const GXVertexElement *pDecl) override;

	IGXVertexShader* createVertexShader(const char *szFile, GXMacro *pDefs = NULL) override;
	IGXVertexShader* createVertexShaderFromString(const char * szCode, GXMacro *pDefs = NULL) override;
	IGXVertexShader* createVertexShaderFromBin(void *pData, UINT uSize) override;

	IGXPixelShader* createPixelShader(const char *szFile, GXMacro *pDefs = NULL) override;
	IGXPixelShader* createPixelShaderFromString(const char * szCode, GXMacro *pDefs = NULL) override;
	IGXPixelShader* createPixelShaderFromBin(void *pData, UINT uSize) override;

	IGXGeometryShader* createGeometryShader(const char *szFile, GXMacro *pDefs = NULL) override;
	IGXGeometryShader* createGeometryShaderFromString(const char * szCode, GXMacro *pDefs = NULL) override;
	IGXGeometryShader* createGeometryShaderFromBin(void *pData, UINT uSize) override;

	IGXComputeShader* createComputeShader(const char *szFile, GXMacro *pDefs = NULL) override;
	IGXComputeShader* createComputeShaderFromString(const char * szCode, GXMacro *pDefs = NULL) override;
	IGXComputeShader* createComputeShaderFromBin(void *pData, UINT uSize) override;

	IGXShaderSet* createShader(IGXVertexShader *pVS = NULL, IGXPixelShader *pPS = NULL, IGXGeometryShader *pGS = NULL, IGXComputeShader *pCS = NULL) override;

	IGXRenderBuffer* createRenderBuffer(UINT countSlots, IGXVertexBuffer ** pBuff, IGXVertexDeclaration *pDecl) override;

	IGXDepthStencilSurface* createDepthStencilSurface(UINT uWidth, UINT uHeight, GXFORMAT format, GXMULTISAMPLE_TYPE multisampleType, bool bAutoResize = false) override;
	IGXDepthStencilSurface* createDepthStencilSurfaceCube(UINT uSize, GXFORMAT format, GXMULTISAMPLE_TYPE multisampleType, bool bAutoResize = false) override;
	void destroyDepthStencilSurface(IGXDepthStencilSurface *pSurface);

	IGXSurface* createColorTarget(UINT uWidth, UINT uHeight, GXFORMAT format, GXMULTISAMPLE_TYPE multisampleType, bool bAutoResize = false) override;
	void destroyColorTarget(IGXSurface *pSurface);
	
	IGXTexture2D* createTexture2D(UINT uWidth, UINT uHeight, UINT uMipLevels, UINT uTexUsageFlags, GXFORMAT format, void * pInitData = NULL) override;
	IGXTexture3D* createTexture3D(UINT uWidth, UINT uHeight, UINT uDepth, UINT uMipLevels, UINT uTexUsageFlags, GXFORMAT format, void * pInitData = NULL) override;
	IGXTextureCube* createTextureCube(UINT uSize, UINT uMipLevels, UINT uTexUsageFlags, GXFORMAT format, void * pInitData = NULL) override;
	void destroyTexture2D(IGXTexture2D * pTexture);
	void destroyTextureCube(IGXTextureCube * pTexture);

	IGXTexture2D* createTexture2DFromFile(const char *szFileName, UINT uTexUsageFlags, bool bAllowNonPowerOf2 = false) override;
	IGXTextureCube* createTextureCubeFromFile(const char *szFileName, UINT uTexUsageFlags, bool bAllowNonPowerOf2 = false) override;

	IGXBlendState* createBlendState(GXBlendDesc *pBlendDesc) override;
	IGXDepthStencilState* createDepthStencilState(GXDepthStencilDesc *pDSDesc) override;
	IGXRasterizerState* createRasterizerState(GXRasterizerDesc *pDSDesc) override;
	IGXSamplerState* createSamplerState(GXSamplerDesc *pSamplerDesc) override;

	IGXConstantBuffer* createConstantBuffer(UINT uSize, bool isPartialUpdateAllowed = false) override;

	void log(GX_LOG lvl, const char *msg, ...);
	static void debugMessage(GX_LOG, const char *msg);
	static void logDXcall(const char *szCodeString, HRESULT hr);

	
	ID3D11Device* getDXDevice();
	ID3D11DeviceContext* getDXDeviceContext();
	IDXGIFactory* getDXGIFactory();

	DXGI_FORMAT getDXFormat(GXFORMAT format);
	GXFORMAT getGXFormat(DXGI_FORMAT format);

	UINT getTextureDataSize(UINT uPitch, UINT uHeight, GXFORMAT format);
	UINT getTextureMemPitch(UINT uWidth, GXFORMAT format);
	UINT getBitsPerPixel(GXFORMAT format);

	bool saveTextureToFile(const char *szTarget, IGXBaseTexture *pTexture) override;

	const GXAdapterMemoryStats* getMemoryStats() override
	{
		return(&m_memoryStats);
	}
	const GXAdapterDesc* getAdapterDesc() override
	{
		return(&m_adapterDesc);
	}

	void addBytesTextures(int uBytes, bool isRT = false)
	{
		if(isRT)
		{
			m_memoryStats.sizeRenderTargetBytes += uBytes;
		}
		else
		{
			m_memoryStats.sizeTextureBytes += uBytes;
		}
	}
	void addBytesShaderConst(int uBytes)
	{
		m_memoryStats.sizeShaderConstBytes += uBytes;
	}

protected:
	
	ID3D11Device *m_pDevice = NULL;
	ID3D11DeviceContext *m_pDeviceContext;
	IDXGIDevice *m_pDXGIDevice = NULL;
	IDXGIFactory *m_pDXGIFactory = NULL;
	IGXLogger *m_pLogger = NULL;
	static CGXDevice *ms_pInstance;
	CGXContext *m_pDirectContext = NULL;
	IGXContext *m_pContexts[GX_MAX_THREADS];
	int m_iLastIndirectContext = 0;

	UINT m_uWindowWidth;
	UINT m_uWindowHeight;
	BOOL m_isWindowed;
	
	void onLostDevice();
	void onResetDevice();

	GXAdapterMemoryStats m_memoryStats;
	GXAdapterDesc m_adapterDesc;

	HWND m_hWnd;

	IGXSwapChain *m_pDefaultSwapChain = NULL;

	Array<IGXTexture2D*> m_aResettableTextures2D;
	Array<IGXTextureCube*> m_aResettableTexturesCube;
	Array<IGXDepthStencilSurface*> m_aResettableDSSurfaces;
	Array<IGXSurface*> m_aResettableColorSurfaces;
	Array<IGXSwapChain*> m_aResettableSwapChains;


	IGXSamplerState *m_pDefaultSamplerState = NULL;
	IGXRasterizerState *m_pDefaultRasterizerState = NULL;
	IGXDepthStencilState *m_pDefaultDepthStencilState = NULL;
	IGXBlendState *m_pDefaultBlendState = NULL;
	IGXDepthStencilSurface *m_pDefaultDepthStencilSurface = NULL;
};

#endif
