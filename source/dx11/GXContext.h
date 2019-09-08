#ifndef _GXCONTEXT_H_
#define _GXCONTEXT_H_

#include "../../graphix.h"
#include <common/array.h>

#include "GXDevice.h"

class CGXContext: public IGXContext
{
	friend class CGXContext;

public:
	CGXContext(ID3D11DeviceContext *pDXContext, CGXDevice *pGXDevice, bool isDirect);
	~CGXContext();

	void Release() override
	{
		--m_uRefCount;
		if(!m_uRefCount)
		{
			if(m_isDirect)
			{
				assert(!"Cannot Release() direct context!\n");
				CGXDevice::debugMessage(GX_LOG_ERROR, "Cannot Release() direct context!\n");
			}
			else
			{
				delete this;
			}
		}
	}

	void beginIndirect() override;
	void endIndirect() override;

	void executeIndirectContext(IGXContext *pContext) override;
	void cloneState(IGXContext *pContext) override;
	bool beginFrame() override;
	void endFrame() override;
	bool isDirect() override
	{
		return(m_isDirect);
	}

	void clear(UINT what, GXCOLOR color = 0, float fDepth = 1.0f, UINT uStencil = 0) override;

	void setIndexBuffer(IGXIndexBuffer *pBuff) override;
	void setRenderBuffer(IGXRenderBuffer *pBuff) override;

	void drawIndexed(UINT uVertexCount, UINT uPrimitiveCount, UINT uStartIndexLocation, int iBaseVertexLocation) override;
	void drawIndexedInstanced(UINT uInstanceCount, UINT uVertexCount, UINT uPrimitiveCount, UINT uStartIndexLocation, int iBaseVertexLocation) override;
	void drawPrimitive(UINT uStartVertex, UINT uPrimitiveCount) override;
	void drawPrimitiveInstanced(UINT uInstanceCount, UINT uStartVertex, UINT uPrimitiveCount);

	void computeDispatch(UINT uThreadGroupCountX, UINT uThreadGroupCountY, UINT uThreadGroupCountZ) override;

	void setPrimitiveTopology(GXPRIMITIVETOPOLOGY pt) override;

	void setVSConstant(IGXConstantBuffer *pBuffer, UINT uSlot = 0) override;
	void setPSConstant(IGXConstantBuffer *pBuffer, UINT uSlot = 0) override;
	void setGSConstant(IGXConstantBuffer *pBuffer, UINT uSlot = 0) override;
	void setCSConstant(IGXConstantBuffer *pBuffer, UINT uSlot = 0) override;

	void setShader(IGXShaderSet *pSH) override;
	IGXShaderSet* getShader() override;

	void setDepthStencilSurface(IGXDepthStencilSurface *pSurface) override;
	void unsetDepthStencilSurface() override;
	IGXDepthStencilSurface* getDepthStencilSurface() override;

	void downsampleColorTarget(IGXSurface *pSource, IGXSurface *pTarget) override;

	void setColorTarget(IGXSurface *pSurf, UINT idx = 0) override;
	IGXSurface* getColorTarget(UINT idx = 0) override;

	void setPSTexture(IGXBaseTexture *pTexture, UINT uStage = 0) override;
	IGXBaseTexture* getPSTexture(UINT uStage = 0) override;
	void setVSTexture(IGXBaseTexture *pTexture, UINT uStage = 0) override;
	IGXBaseTexture* getVSTexture(UINT uStage = 0) override;
	void setCSTexture(IGXBaseTexture *pTexture, UINT uStage = 0) override;
	IGXBaseTexture* getCSTexture(UINT uStage = 0) override;
	void setCSUnorderedAccessView(IGXBaseTexture *pUAV, UINT uStage = 0) override;
	IGXBaseTexture* getCSUnorderedAccessView(UINT uStage = 0) override;
	
	void setBlendState(IGXBlendState *pState) override;
	IGXBlendState* getBlendState() override;
	void setBlendFactor(GXCOLOR val) override;

	void setDepthStencilState(IGXDepthStencilState *pState) override;
	IGXDepthStencilState* getDepthStencilState() override;
	void setStencilRef(UINT uVal) override;

	void setRasterizerState(IGXRasterizerState *pState) override;
	IGXRasterizerState* getRasterizerState() override;
	void setScissorRect(int iTop, int iRight, int iBottom, int iLeft) override;

	void setSamplerState(IGXSamplerState *pState, UINT uSlot) override;
	IGXSamplerState* getSamplerState(UINT uSlot) override;

	ID3D11DeviceContext* getDXDeviceContext();
	
	const GXFrameStats* getFrameStats() override
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
	void addBytesShaderConst(UINT uBytes)
	{
		m_frameStats.uUploadedBuffersShaderConst += uBytes;
	}

	UINT getIDXcount(UINT ptCount);

	ID3D11DeviceContext* getDXContext()
	{
		return(m_pDeviceContext);
	}

protected:
	
	ID3D11DeviceContext *m_pDeviceContext = NULL;
	CGXDevice *m_pGXDevice;
	bool m_isDirect;
	ID3D11CommandList *m_pCommandList = NULL;

	GXFrameStats m_frameStats;

	IGXRenderBuffer *m_pCurRenderBuffer = NULL;
	IGXIndexBuffer *m_pCurIndexBuffer = NULL;

	IGXVertexDeclaration *m_pCurVertexDecl;
	D3D_PRIMITIVE_TOPOLOGY m_drawPT = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	IGXSamplerState *m_pSamplerState[GX_MAX_SAMPLERS];
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

	void syncronize(UINT flags=0);
	void _updateStats(UINT uPrimCount);

	BOOL m_isScissorsEnable = 0;
	RECT m_rcScissors;
};

#endif
