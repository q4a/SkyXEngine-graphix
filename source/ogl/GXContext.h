#ifndef _GXCONTEXT_H_
#define _GXCONTEXT_H_

#include "../../graphix.h"

#include "GLPFN.h"
#include "GXDevice.h"

#if defined(_WINDOWS)
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#	pragma comment(lib, "opengl32.lib")
#endif

class CGXContext: public IGXContext
{
	friend class CGXDevice;

public:
	BOOL initContext(SXWINDOW wnd);
	CGXContext(CGXContext *pDXContext, CGXDevice *pGXDevice, bool isDirect);
	~CGXContext();

	void swapBuffers();

	void beginIndirect() override {};
	void endIndirect() override {};

	void executeIndirectContext(IGXContext* pContext) override {};
	bool beginFrame()
	{
		memset(&m_frameStats, 0, sizeof(m_frameStats));

		return(true);
	};
	void endFrame()
	{
	};
	bool isDirect() override
	{
		return(m_isDirect);
	}

	IGXContextState* getCurrentState() override { return nullptr; };
	void setFullState(IGXContextState* pState) override {};

	void clear(UINT what, GXCOLOR color = 0, float fDepth = 1.0f, UINT uStencil = 0) override {};

	void drawIndexed(UINT uVertexCount, UINT uPrimitiveCount, UINT uStartIndexLocation, int iBaseVertexLocation) override {};
	void drawIndexedInstanced(UINT uInstanceCount, UINT uVertexCount, UINT uPrimitiveCount, UINT uStartIndexLocation, int iBaseVertexLocation) override {};
	void drawPrimitive(UINT uStartVertex, UINT uPrimitiveCount) override {};

	void computeDispatch(UINT uThreadGroupCountX, UINT uThreadGroupCountY, UINT uThreadGroupCountZ) override {};

	void setPrimitiveTopology(GXPRIMITIVETOPOLOGY pt) override;

	void setVSConstant(IGXConstantBuffer* pBuffer, UINT uSlot = 0) override {};
	void setPSConstant(IGXConstantBuffer* pBuffer, UINT uSlot = 0) override {};
	void setGSConstant(IGXConstantBuffer* pBuffer, UINT uSlot = 0) override {};
	void setCSConstant(IGXConstantBuffer* pBuffer, UINT uSlot = 0) override {};

	void setShader(IGXShaderSet* pSH) override {};
	IGXShaderSet* getShader() override { return nullptr; };

	void setDepthStencilSurface(IGXDepthStencilSurface* pSurface) override {};
	void unsetDepthStencilSurface() override {};
	IGXDepthStencilSurface* getDepthStencilSurface() override { return nullptr; };

	void downsampleColorTarget(IGXSurface* pSource, IGXSurface* pTarget) override {};

	void setColorTarget(IGXSurface* pSurf, UINT idx = 0) override {};
	IGXSurface* getColorTarget(UINT idx = 0) override { return nullptr; };

	void setPSTexture(IGXBaseTexture* pTexture, UINT uStage = 0) override {};
	IGXBaseTexture* getPSTexture(UINT uStage = 0) override { return nullptr; };
	void setVSTexture(IGXBaseTexture* pTexture, UINT uStage = 0) override {};
	IGXBaseTexture* getVSTexture(UINT uStage = 0) override { return nullptr; };
	void setCSTexture(IGXBaseTexture* pTexture, UINT uStage = 0) override {};
	IGXBaseTexture* getCSTexture(UINT uStage = 0) override { return nullptr; };
	void setCSUnorderedAccessView(IGXBaseTexture* pUAV, UINT uStage = 0) override {};
	IGXBaseTexture* getCSUnorderedAccessView(UINT uStage = 0) override { return nullptr; };

	void setBlendState(IGXBlendState* pState) override {};
	IGXBlendState* getBlendState() override { return nullptr; };
	void setBlendFactor(GXCOLOR val) override {};

	void setDepthStencilState(IGXDepthStencilState* pState) override {};
	IGXDepthStencilState* getDepthStencilState() override { return nullptr; };
	void setStencilRef(UINT uVal) override {};

	void setRasterizerState(IGXRasterizerState* pState) override {};
	IGXRasterizerState* getRasterizerState() override { return nullptr; };
	void setScissorRect(int iTop, int iRight, int iBottom, int iLeft) override {};

	void setSamplerState(IGXSamplerState* pState, UINT uSlot) override {};
	IGXSamplerState* getSamplerState(UINT uSlot) override { return nullptr; };

	void setClearColor(const float4_t& color);
	void clearTarget();
	void clearDepth(float val = 1.0f);
	void clearStencil(UINT val = 0);

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

	void drawIndexed(UINT uIndexCount, UINT uStartIndexLocation, int iBaseVertexLocation);

	//IGXShader * createShader(const char * pName, UINT flags = 0);
	//void destroyShader(IGXShader * pSH);
	//void setShader(IGXShader * pSH);

	IGXVertexShader * createVertexShader(const char * szFile);
	IGXVertexShader * createVertexShader(void *pData, UINT uSize);
	void destroyVertexShader(IGXVertexShader * pSH);

	IGXPixelShader * createPixelShader(const char * szFile);
	IGXPixelShader * createPixelShader(void *pData, UINT uSize);
	void destroyPixelShader(IGXPixelShader * pSH);

	void setVertexShader(IGXVertexShader * pSH);
	void setPixelShader(IGXPixelShader * pSH);

	IGXRenderBuffer * createRenderBuffer(UINT countSlots, IGXVertexBuffer ** pBuff, IGXVertexDeclaration * pDecl);
	void destroyRenderBuffer(IGXRenderBuffer * pDecl);

	IDSRGLPFN * m_pGL;

	const GXFrameStats* getFrameStats() override
	{
		return(&m_frameStats);
	}

protected:

	SXWINDOW m_hWnd;

	CGXContext *m_pDeviceContext = NULL;
	CGXDevice *m_pGXDevice;
	bool m_isDirect;
	ID3D11CommandList *m_pCommandList = NULL;

	GXFrameStats m_frameStats;

	//IDSRvertexBuffer * m_pCurVertexBuffer[MAXDSGVSTREAM];
	IGXRenderBuffer * m_pCurRenderBuffer;
	IGXIndexBuffer * m_pCurIndexBuffer;

	IGXVertexDeclaration * m_pCurVertexDecl;
	GLenum m_drawPT;

	IGXSamplerState *m_pSamplerState[GX_MAX_SAMPLERS];
	IGXSamplerState* m_pDefaultSamplerState = NULL;

	IGXRasterizerState* m_pDefaultRasterizerState = NULL;

	IGXDepthStencilState* m_pDefaultDepthStencilState = NULL;

	//IGXBlendState* m_pBlendState = NULL;
	IGXBlendState* m_pDefaultBlendState = NULL;

	IGXDepthStencilSurface* m_pDefaultDepthStencilSurface = NULL;

	IGXSurface *m_pColorTarget[GX_MAX_COLORTARGETS];
	GLuint *m_pDXColorTarget[GX_MAX_COLORTARGETS];

	IGXBaseTexture* m_pTextures[GX_MAX_TEXTURES];
	IGXBaseTexture* m_pTexturesVS[GX_MAX_TEXTURES];
	IGXBaseTexture* m_pTexturesCS[GX_MAX_TEXTURES];
	IGXBaseTexture* m_pUAVsCS[GX_MAX_UAV_TEXTURES];

	IGXConstantBuffer* m_pVSConstant[GX_MAX_SHADER_CONST];
	IGXConstantBuffer* m_pPSConstant[GX_MAX_SHADER_CONST];
	IGXConstantBuffer* m_pGSConstant[GX_MAX_SHADER_CONST];
	IGXConstantBuffer* m_pCSConstant[GX_MAX_SHADER_CONST];

	_sync_state m_sync_state;

	//struct _shcache_item
	//{
	//	GLuint pShader;
	//};

	//AssotiativeArray<DString, _shcache_item> m_mShaderCache;

	//struct _shader_source
	//{
	//	const char * source;
	//};

	//AssotiativeArray<DString, _shader_source> m_mShaderSources;

	//DSourcePreprocessor * m_pSPP;

	void syncronize(UINT flags=0);
	UINT getPTcount(UINT idxCount);
	UINT getIndexSize(UINT idx);
	GLuint getShaderPart(GLenum type, const char *szName, UINT flags = 0, const char ** defs = NULL);
};

#endif
