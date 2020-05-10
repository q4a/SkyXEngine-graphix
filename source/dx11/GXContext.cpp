#include "GXContext.h"

#include "GXIndexBuffer.h"
#include "GXVertexBuffer.h"
#include "GXVertexDeclaration.h"
#include "GXShader.h"
#include "GXRenderBuffer.h"
#include "GXSamplerState.h"
#include "GXRasterizerState.h"
#include "GXDepthStencilState.h"
#include "GXDepthStencilSurface.h"
#include "GXSurface.h"
#include "GXBlendState.h"
#include "GXTexture.h"
#include "GXSwapChain.h"
#include "GXConstantBuffer.h"
#include "GXContextState.h"

#include <cstdio>

CGXContext::CGXContext(ID3D11DeviceContext *pDXContext, CGXDevice *pGXDevice, bool isDirect):
	m_pDeviceContext(pDXContext),
	m_pGXDevice(pGXDevice),
	m_isDirect(isDirect)
{
	memset(&m_sync_state, 0, sizeof(m_sync_state));
	memset(&m_pSamplerState, 0, sizeof(m_pSamplerState));
	memset(&m_pColorTarget, 0, sizeof(m_pColorTarget));
	memset(&m_pDXColorTarget, 0, sizeof(m_pDXColorTarget));
	memset(&m_pTextures, 0, sizeof(m_pTextures));
	memset(&m_pTexturesVS, 0, sizeof(m_pTexturesVS));
	memset(&m_pTexturesCS, 0, sizeof(m_pTexturesCS));
	memset(&m_pUAVsCS, 0, sizeof(m_pUAVsCS));
	memset(&m_pPSConstant, 0, sizeof(m_pPSConstant));
	memset(&m_pVSConstant, 0, sizeof(m_pVSConstant));
	memset(&m_pCSConstant, 0, sizeof(m_pCSConstant));
	memset(&m_pGSConstant, 0, sizeof(m_pGSConstant));
	memset(&m_frameStats, 0, sizeof(m_frameStats));

	m_pDefaultDepthStencilSurface = m_pGXDevice->m_pDefaultDepthStencilSurface;
	m_pDefaultRasterizerState = m_pGXDevice->m_pDefaultRasterizerState;
	m_pDefaultSamplerState = m_pGXDevice->m_pDefaultSamplerState;
	m_pDefaultBlendState = m_pGXDevice->m_pDefaultBlendState;
	m_pDefaultDepthStencilState = m_pGXDevice->m_pDefaultDepthStencilState;

	m_pDefaultDepthStencilSurface->AddRef();
	m_pDefaultRasterizerState->AddRef();
	m_pDefaultSamplerState->AddRef();
	m_pDefaultBlendState->AddRef();
	m_pDefaultDepthStencilState->AddRef();
}

CGXContext::~CGXContext()
{
	mem_release(m_pCommandList);
	mem_release(m_pDeviceContext);
	
	//! @todo: release all referenced resources!

	mem_release(m_pDefaultDepthStencilSurface);
	mem_release(m_pDefaultRasterizerState);
	mem_release(m_pDefaultSamplerState);
	mem_release(m_pDefaultBlendState);
	mem_release(m_pDefaultDepthStencilState);

	for(UINT i = 0; i < GX_MAX_SHADER_CONST; ++i)
	{
		mem_release(m_pVSConstant[i]);
		mem_release(m_pPSConstant[i]);
		mem_release(m_pGSConstant[i]);
		mem_release(m_pCSConstant[i]);
	}
}

void CGXContext::beginIndirect()
{
	if(m_isDirect)
	{
		return;
	}

	mem_release(m_pCommandList);
	memset(&m_frameStats, 0, sizeof(m_frameStats));
}
void CGXContext::endIndirect()
{
	if(m_isDirect)
	{
		return;
	}

	mem_release(m_pCommandList);
	DX_CALL(m_pDeviceContext->FinishCommandList(FALSE, &m_pCommandList));


	memset(&m_sync_state, 1, sizeof(m_sync_state));
}

void CGXContext::executeIndirectContext(IGXContext *pContext)
{
	assert(pContext);

	CGXContext *pCtx = (CGXContext*)pContext;

	if(pCtx->m_pCommandList)
	{
		m_pDeviceContext->ExecuteCommandList(pCtx->m_pCommandList, TRUE);

		m_frameStats.uDIPcount += pCtx->m_frameStats.uDIPcount;
		m_frameStats.uLineCount += pCtx->m_frameStats.uLineCount;
		m_frameStats.uPointCount += pCtx->m_frameStats.uPointCount;
		m_frameStats.uPolyCount += pCtx->m_frameStats.uPolyCount;
		m_frameStats.uUploadedBuffersIndices += pCtx->m_frameStats.uUploadedBuffersIndices;
		m_frameStats.uUploadedBuffersShaderConst += pCtx->m_frameStats.uUploadedBuffersShaderConst;
		m_frameStats.uUploadedBuffersTextures += pCtx->m_frameStats.uUploadedBuffersTextures;
		m_frameStats.uUploadedBuffersVertexes += pCtx->m_frameStats.uUploadedBuffersVertexes;
		m_frameStats.uShaderSwitches += pCtx->m_frameStats.uShaderSwitches;
	}
	else
	{
		//CGXDevice::debugMessage(GX_LOG_WARN, "executeIndirectContext(): Context has no command list recorded!\n");
	}
}
bool CGXContext::beginFrame()
{
	memset(&m_frameStats, 0, sizeof(m_frameStats));

	return(true);
}
void CGXContext::endFrame()
{
}

#define GXCOPY_REF(field) field; if(field){field->AddRef();}

IGXContextState* CGXContext::getCurrentState()
{
	CGXContextState *pState = new CGXContextState();

	pState->m_pCurRenderBuffer = GXCOPY_REF(m_pCurRenderBuffer);
	pState->m_pCurIndexBuffer = GXCOPY_REF(m_pCurIndexBuffer);

	for(UINT i = 0; i < GX_MAX_SAMPLERS; ++i)
	{
		pState->m_pSamplerState[i] = GXCOPY_REF(m_pSamplerState[i]);
	}

	pState->m_pRasterizerState = GXCOPY_REF(m_pRasterizerState);
	pState->m_pDepthStencilState = GXCOPY_REF(m_pDepthStencilState);
	pState->m_uStencilRef = m_uStencilRef;
	pState->m_pBlendState = GXCOPY_REF(m_pBlendState);
	pState->m_blendFactor = m_blendFactor;
	pState->m_pDepthStencilSurface = GXCOPY_REF(m_pDepthStencilSurface);

	for(UINT i = 0; i < GX_MAX_COLORTARGETS; ++i)
	{
		pState->m_pColorTarget[i] = GXCOPY_REF(m_pColorTarget[i]);
	}

	for(UINT i = 0; i < GX_MAX_TEXTURES; ++i)
	{
		pState->m_pTextures[i] = GXCOPY_REF(m_pTextures[i]);
		pState->m_pTexturesVS[i] = GXCOPY_REF(m_pTexturesVS[i]);
		pState->m_pTexturesCS[i] = GXCOPY_REF(m_pTexturesCS[i]);
	}

	for(UINT i = 0; i < GX_MAX_UAV_TEXTURES; ++i)
	{
		pState->m_pUAVsCS[i] = GXCOPY_REF(m_pUAVsCS[i]);
	}

	pState->m_pShader = GXCOPY_REF(m_pShader);
	pState->m_rcScissors = m_rcScissors;
	pState->m_gxPT = m_gxPT;

	for(UINT i = 0; i < GX_MAX_SHADER_CONST; ++i)
	{
		pState->m_pVSConstant[i] = GXCOPY_REF(m_pVSConstant[i]);
		pState->m_pPSConstant[i] = GXCOPY_REF(m_pPSConstant[i]);
		pState->m_pGSConstant[i] = GXCOPY_REF(m_pGSConstant[i]);
		pState->m_pCSConstant[i] = GXCOPY_REF(m_pCSConstant[i]);
	}

	return(pState);
}
#undef GXCOPY_REF
void CGXContext::setFullState(IGXContextState *pState_)
{
	assert(pState_);
	CGXContextState *pState = (CGXContextState*)pState_;

	setRenderBuffer(pState->m_pCurRenderBuffer);
	setIndexBuffer(pState->m_pCurIndexBuffer);

	for(UINT i = 0; i < GX_MAX_SAMPLERS; ++i)
	{
		setSamplerState(pState->m_pSamplerState[i], i);
	}

	setRasterizerState(pState->m_pRasterizerState);
	setDepthStencilState(pState->m_pDepthStencilState);
	setStencilRef(pState->m_uStencilRef);
	setBlendState(pState->m_pBlendState);
	setBlendFactor(pState->m_blendFactor);
	if(pState->m_pDepthStencilSurface)
	{
		setDepthStencilSurface(pState->m_pDepthStencilSurface);
	}
	else
	{
		unsetDepthStencilSurface();
	}

	for(UINT i = 0; i < GX_MAX_COLORTARGETS; ++i)
	{
		setColorTarget(pState->m_pColorTarget[i], i);
	}

	for(UINT i = 0; i < GX_MAX_TEXTURES; ++i)
	{
		setPSTexture(pState->m_pTextures[i], i);
		setVSTexture(pState->m_pTexturesVS[i], i);
		setCSTexture(pState->m_pTexturesCS[i], i);
	}

	for(UINT i = 0; i < GX_MAX_UAV_TEXTURES; ++i)
	{
		setCSUnorderedAccessView(pState->m_pUAVsCS[i], i);
	}

	setShader(pState->m_pShader);

	setScissorRect(pState->m_rcScissors.top, pState->m_rcScissors.right, pState->m_rcScissors.bottom, pState->m_rcScissors.left);

	setPrimitiveTopology(pState->m_gxPT);

	for(UINT i = 0; i < GX_MAX_SHADER_CONST; ++i)
	{
		setVSConstant(pState->m_pVSConstant[i], i);
		setPSConstant(pState->m_pPSConstant[i], i);
		setGSConstant(pState->m_pGSConstant[i], i);
		setCSConstant(pState->m_pCSConstant[i], i);
	}
}

void CGXContext::clear(UINT what, const float4 &vColor, float fDepth, UINT uStencil)
{
	if(what & GX_CLEAR_COLOR)
	{
		for(UINT i = 0; i < GX_MAX_COLORTARGETS; ++i)
		{
			if(m_pColorTarget[i])
			{
				m_pDeviceContext->ClearRenderTargetView(((CGXSurface*)m_pColorTarget[i])->m_pRTV, (float*)&vColor);
			}
		}
	}
	if(what & (GX_CLEAR_DEPTH | GX_CLEAR_STENCIL))
	{
		m_pDeviceContext->ClearDepthStencilView(((CGXDepthStencilSurface*)m_pDepthStencilSurface)->m_pSurface, ((what & GX_CLEAR_DEPTH) ? D3D11_CLEAR_DEPTH : 0) | ((what & GX_CLEAR_STENCIL) ? D3D11_CLEAR_STENCIL : 0), fDepth, uStencil);
	}
}


void CGXContext::setIndexBuffer(IGXIndexBuffer *pBuff)
{
	mem_release(m_pCurIndexBuffer);
	m_pCurIndexBuffer = pBuff;
	if(pBuff)
	{
		pBuff->AddRef();
	}
	m_sync_state.bIndexBuffer = TRUE;
}

void CGXContext::computeDispatch(UINT uThreadGroupCountX, UINT uThreadGroupCountY, UINT uThreadGroupCountZ)
{
	syncronize();

	m_pDeviceContext->Dispatch(uThreadGroupCountX, uThreadGroupCountY, uThreadGroupCountZ);
}

void CGXContext::drawIndexed(UINT uVertexCount, UINT uPrimitiveCount, UINT uStartIndexLocation, int iBaseVertexLocation)
{
	syncronize();

	assert(m_pCurIndexBuffer && "Draw indexed without index buffer bound!");

	//@TODO: move to syncronize
	m_pDeviceContext->IASetPrimitiveTopology(m_drawPT);

	m_pDeviceContext->DrawIndexed(getIDXcount(uPrimitiveCount), uStartIndexLocation, iBaseVertexLocation);

	_updateStats(uPrimitiveCount);
}
void CGXContext::drawPrimitive(UINT uStartVertex, UINT uPrimitiveCount)
{
	syncronize();
	//@TODO: move to syncronize
	m_pDeviceContext->IASetPrimitiveTopology(m_drawPT);
	
	m_pDeviceContext->Draw(getIDXcount(uPrimitiveCount), uStartVertex);
	
	_updateStats(uPrimitiveCount);
}
void CGXContext::drawIndexedInstanced(UINT uInstanceCount, UINT uVertexCount, UINT uPrimitiveCount, UINT uStartIndexLocation, int iBaseVertexLocation)
{
	syncronize();
	//@TODO: move to syncronize
	m_pDeviceContext->IASetPrimitiveTopology(m_drawPT);

	assert(m_pCurIndexBuffer && "Draw indexed without index buffer bound!");

	m_pDeviceContext->DrawIndexedInstanced(getIDXcount(uPrimitiveCount), uInstanceCount, uStartIndexLocation, iBaseVertexLocation, 0);

	_updateStats(uPrimitiveCount);
}
void CGXContext::drawPrimitiveInstanced(UINT uInstanceCount, UINT uStartVertex, UINT uPrimitiveCount)
{
	syncronize();
	//@TODO: move to syncronize
	m_pDeviceContext->IASetPrimitiveTopology(m_drawPT);

	m_pDeviceContext->DrawInstanced(getIDXcount(uPrimitiveCount), uInstanceCount, uStartVertex, 0);

	_updateStats(uPrimitiveCount);
}

void CGXContext::syncronize(UINT flags)
{
	if(m_sync_state.bRenderBuffer)
	{
		if(m_pCurRenderBuffer)
		{
			CGXRenderBuffer *pRB = (CGXRenderBuffer*)m_pCurRenderBuffer;
			CGXVertexDeclaration *pVD = (CGXVertexDeclaration*)pRB->m_pVertexDeclaration;
			assert(pVD->m_u8StreamCount == pRB->m_uStreamCount);

			m_pDeviceContext->IASetInputLayout(pVD->m_pDeclaration);
			UINT stride, offset = 0;
			for(UINT i = 0; i < pRB->m_uStreamCount; ++i)
			{
				stride = pVD->m_u8StreamStride[i];
				m_pDeviceContext->IASetVertexBuffers(i, 1, &((CGXVertexBuffer*)pRB->m_ppVertexBuffers[i])->m_pBuffer, &stride, &offset);
			}
		}
		else
		{
			ID3D11Buffer *pBuffer = NULL;
			UINT tmp = 0;
			m_pDeviceContext->IASetVertexBuffers(0, 1, &pBuffer, &tmp, &tmp);
			m_pDeviceContext->IASetInputLayout(NULL);
		}

		m_sync_state.bRenderBuffer = FALSE;
	}
	if(m_sync_state.bIndexBuffer)
	{
		if(m_pCurIndexBuffer)
		{
			CGXIndexBuffer *pIB = (CGXIndexBuffer*)m_pCurIndexBuffer;
			m_pDeviceContext->IASetIndexBuffer(pIB->m_pBuffer, pIB->m_format, 0);
		}
		else
		{
			m_pDeviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R16_UINT, 0);
		}
		m_sync_state.bIndexBuffer = FALSE;
	}
	if(m_sync_state.bRasterizerState)
	{
		CGXRasterizerState *pRS = (CGXRasterizerState*)m_pRasterizerState;
		if(!pRS)
		{
			pRS = (CGXRasterizerState*)m_pDefaultRasterizerState;
		}
		m_pDeviceContext->RSSetState(pRS->m_pStateBlock);
		m_isScissorsEnable = pRS->m_isScissorsEnabled;
		m_sync_state.bRasterizerState = FALSE;
	}
	for(UINT i = 0; i < GX_MAX_SAMPLERS; ++i)
	{
		if(m_sync_state.bSamplerState[i])
		{
			CGXSamplerState *pSS = (CGXSamplerState*)m_pSamplerState[i];
			if(!pSS)
			{
				pSS = (CGXSamplerState*)m_pDefaultSamplerState;
			}
			m_pDeviceContext->PSSetSamplers(i, 1, &pSS->m_pStateBlock);
			m_sync_state.bSamplerState[i] = FALSE;
		}
	}

	if(m_sync_state.bDepthStencilState)
	{
		CGXDepthStencilState *pDS = (CGXDepthStencilState*)m_pDepthStencilState;
		if(!pDS)
		{
			pDS = (CGXDepthStencilState*)m_pDefaultDepthStencilState;
		}
		m_pDeviceContext->OMSetDepthStencilState(pDS->m_pStateBlock, m_uStencilRef);

		m_sync_state.bDepthStencilState = FALSE;
	}

	if(m_sync_state.bBlendState)
	{
		CGXBlendState *pBS = (CGXBlendState*)m_pBlendState;
		if(!pBS)
		{
			pBS = (CGXBlendState*)m_pDefaultBlendState;
		}
		m_pDeviceContext->OMSetBlendState(pBS->m_pStateBlock, (float*)&GX_COLOR_COLOR_TO_F4(m_blendFactor), 0xffffffff);
		m_sync_state.bBlendState = FALSE;
	}

	// Unbound textures first!
	for(UINT i = 0; i < GX_MAX_TEXTURES; ++i)
	{
		if(m_sync_state.bTexture[i])
		{
			if(!m_pTextures[i])
			{
				ID3D11ShaderResourceView *pNull = NULL;
				m_pDeviceContext->PSSetShaderResources(i, 1, &pNull);
				m_sync_state.bTexture[i] = FALSE;
			}
		}
		if(m_sync_state.bTextureVS[i])
		{
			if(!m_pTexturesVS[i])
			{
				ID3D11ShaderResourceView *pNull = NULL;
				m_pDeviceContext->VSSetShaderResources(i, 1, &pNull);
				m_sync_state.bTextureVS[i] = FALSE;
			}
		}
		if(m_sync_state.bTextureCS[i])
		{
			if(!m_pTexturesCS[i])
			{
				ID3D11ShaderResourceView *pNull = NULL;
				m_pDeviceContext->CSSetShaderResources(i, 1, &pNull);
				m_sync_state.bTextureCS[i] = FALSE;
			}
		}
	}

	if(m_sync_state.bRenderTarget)
	{
		UINT uMaxIdx = 0;
		for(UINT i = 0; i < GX_MAX_COLORTARGETS; ++i)
		{

			//m_pDeviceContext->OMSetRenderTargets(
			if(m_pColorTarget[i])
			{
				uMaxIdx = i;
				m_pDXColorTarget[i] = ((CGXSurface*)m_pColorTarget[i])->m_pRTV;
			}
			else
			{
				m_pDXColorTarget[i] = NULL;
			}
		}
		m_pDeviceContext->OMSetRenderTargets(uMaxIdx + 1, m_pDXColorTarget, m_pDepthStencilSurface ? ((CGXDepthStencilSurface*)m_pDepthStencilSurface)->m_pSurface : NULL);

		D3D11_VIEWPORT vp;
		vp.Width = (float)m_pColorTarget[0]->getWidth();
		vp.Height = (float)m_pColorTarget[0]->getHeight();
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0.5f;
		vp.TopLeftY = 0.5f;
		m_pDeviceContext->RSSetViewports(1, &vp);

		m_sync_state.bRenderTarget = FALSE;
	}

	for(UINT i = 0; i < GX_MAX_UAV_TEXTURES; ++i)
	{
		if(m_sync_state.bUAVsCS[i])
		{
			UINT uUAVInitialCounts = 0;
			if(m_pUAVsCS[i])
			{
				ID3D11UnorderedAccessView *pTex = NULL;
				switch(m_pUAVsCS[i]->getType())
				{
				case GXTEXTURE_TYPE_2D:
					pTex = ((CGXTexture2D*)m_pUAVsCS[i])->m_pUAV;
					break;
				case GXTEXTURE_TYPE_3D:
					pTex = ((CGXTexture3D*)m_pUAVsCS[i])->m_pUAV;
					break;
				case GXTEXTURE_TYPE_CUBE:
					pTex = ((CGXTextureCube*)m_pUAVsCS[i])->m_pUAV;
					break;
				}
				m_pDeviceContext->CSSetUnorderedAccessViews(i, 1, &pTex, &uUAVInitialCounts);
			}
			else
			{
				ID3D11UnorderedAccessView *pNull = NULL;
				m_pDeviceContext->CSSetUnorderedAccessViews(i, 1, &pNull, &uUAVInitialCounts);
			}
			m_sync_state.bUAVsCS[i] = FALSE;
		}
	}
	
	
	if(!m_pShader)
	{
		CGXDevice::debugMessage(GX_LOG_ERROR, "Shader is not set!");
	}
	else
	{
		CGXShader *pShader = (CGXShader*)m_pShader;
		if(m_sync_state.bShader)
		{
			if(pShader->m_pVShader)
			{
				m_pDeviceContext->VSSetShader(pShader->m_pVShader->m_pShader, NULL, 0);
			}
			else
			{
				m_pDeviceContext->VSSetShader(NULL, NULL, 0);
			}
			if(pShader->m_pPShader)
			{
				m_pDeviceContext->PSSetShader(pShader->m_pPShader->m_pShader, NULL, 0);
			}
			else
			{
				m_pDeviceContext->PSSetShader(NULL, NULL, 0);
			}

			if(pShader->m_pGShader)
			{
				m_pDeviceContext->GSSetShader(pShader->m_pGShader->m_pShader, NULL, 0);
			}
			else
			{
				m_pDeviceContext->GSSetShader(NULL, NULL, 0);
			}

			if(pShader->m_pCShader)
			{
				m_pDeviceContext->CSSetShader(pShader->m_pCShader->m_pShader, NULL, 0);
			}
			else
			{
				m_pDeviceContext->CSSetShader(NULL, NULL, 0);
			}
			m_sync_state.bShader = FALSE;
			++m_frameStats.uShaderSwitches;
		}
	}

	for(UINT i = 0; i < GX_MAX_TEXTURES; ++i)
	{
		if(m_sync_state.bTexture[i])
		{
			if(m_pTextures[i])
			{
				ID3D11ShaderResourceView *pTex = NULL;
				switch(m_pTextures[i]->getType())
				{
				case GXTEXTURE_TYPE_2D:
					pTex = ((CGXTexture2D*)m_pTextures[i])->getDXTexture();
					break;
				case GXTEXTURE_TYPE_3D:
					pTex = ((CGXTexture3D*)m_pTextures[i])->getDXTexture();
					break;
				case GXTEXTURE_TYPE_CUBE:
					pTex = ((CGXTextureCube*)m_pTextures[i])->getDXTexture();
					break;
				}
				m_pDeviceContext->PSSetShaderResources(i, 1, &pTex);
			}

			m_sync_state.bTexture[i] = FALSE;
		}
		if(m_sync_state.bTextureVS[i])
		{
			if(m_pTexturesVS[i])
			{
				ID3D11ShaderResourceView *pTex = NULL;
				switch(m_pTexturesVS[i]->getType())
				{
				case GXTEXTURE_TYPE_2D:
					pTex = ((CGXTexture2D*)m_pTexturesVS[i])->getDXTexture();
					break;
				case GXTEXTURE_TYPE_3D:
					pTex = ((CGXTexture3D*)m_pTexturesVS[i])->getDXTexture();
					break;
				case GXTEXTURE_TYPE_CUBE:
					pTex = ((CGXTextureCube*)m_pTexturesVS[i])->getDXTexture();
					break;
				}
				m_pDeviceContext->VSSetShaderResources(i, 1, &pTex);
			}

			m_sync_state.bTextureVS[i] = FALSE;
		}
		if(m_sync_state.bTextureCS[i])
		{
			if(m_pTexturesCS[i])
			{
				ID3D11ShaderResourceView *pTex = NULL;
				switch(m_pTexturesCS[i]->getType())
				{
				case GXTEXTURE_TYPE_2D:
					pTex = ((CGXTexture2D*)m_pTexturesCS[i])->getDXTexture();
					break;
				case GXTEXTURE_TYPE_3D:
					pTex = ((CGXTexture3D*)m_pTexturesCS[i])->getDXTexture();
					break;
				case GXTEXTURE_TYPE_CUBE:
					pTex = ((CGXTextureCube*)m_pTexturesCS[i])->getDXTexture();
					break;
				}
				m_pDeviceContext->CSSetShaderResources(i, 1, &pTex);
			}

			m_sync_state.bTextureCS[i] = FALSE;
		}
	}


	if(m_isScissorsEnable && m_sync_state.bScissorsRect)
	{
		m_pDeviceContext->RSSetScissorRects(1, &m_rcScissors);
	}
}

void CGXContext::setPrimitiveTopology(GXPRIMITIVETOPOLOGY pt)
{
	m_gxPT = pt;
	switch(pt)
	{
	case GXPT_POINTLIST:
		m_drawPT = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
		break;
	case GXPT_LINELIST:
		m_drawPT = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
		break;
	case GXPT_LINESTRIP:
		m_drawPT = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
		break;
	case GXPT_TRIANGLELIST:
		m_drawPT = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		break;
	case GXPT_TRIANGLESTRIP:
		m_drawPT = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		break;
	default:
		assert(!"Unknown primitive topology!");
	}
}

void CGXContext::setVSConstant(IGXConstantBuffer *pBuffer, UINT uSlot)
{
	//@TODO: defer this

	assert(uSlot < GX_MAX_SHADER_CONST);

	ID3D11Buffer *pBuf = NULL;
	if(pBuffer)
	{
		pBuf = ((CGXConstantBuffer*)pBuffer)->m_pBuffer;
		pBuffer->AddRef();
	}
	m_pDeviceContext->VSSetConstantBuffers(uSlot, 1, &pBuf);

	mem_release(m_pVSConstant[uSlot]);
	m_pVSConstant[uSlot] = pBuffer;
}

void CGXContext::setPSConstant(IGXConstantBuffer *pBuffer, UINT uSlot)
{
	//@TODO: defer this

	assert(uSlot < GX_MAX_SHADER_CONST);

	ID3D11Buffer *pBuf = NULL;
	if(pBuffer)
	{
		pBuf = ((CGXConstantBuffer*)pBuffer)->m_pBuffer;
		pBuffer->AddRef();
	}
	m_pDeviceContext->PSSetConstantBuffers(uSlot, 1, &pBuf);

	mem_release(m_pPSConstant[uSlot]);
	m_pPSConstant[uSlot] = pBuffer;
}

void CGXContext::setGSConstant(IGXConstantBuffer *pBuffer, UINT uSlot)
{
	//@TODO: defer this

	assert(uSlot < GX_MAX_SHADER_CONST);

	ID3D11Buffer *pBuf = NULL;
	if(pBuffer)
	{
		pBuf = ((CGXConstantBuffer*)pBuffer)->m_pBuffer;
		pBuffer->AddRef();
	}
	m_pDeviceContext->GSSetConstantBuffers(uSlot, 1, &pBuf);

	mem_release(m_pGSConstant[uSlot]);
	m_pGSConstant[uSlot] = pBuffer;
}

void CGXContext::setCSConstant(IGXConstantBuffer *pBuffer, UINT uSlot)
{
	//@TODO: defer this

	assert(uSlot < GX_MAX_SHADER_CONST);

	ID3D11Buffer *pBuf = NULL;
	if(pBuffer)
	{
		pBuf = ((CGXConstantBuffer*)pBuffer)->m_pBuffer;
		pBuffer->AddRef();
	}
	m_pDeviceContext->CSSetConstantBuffers(uSlot, 1, &pBuf);

	mem_release(m_pCSConstant[uSlot]);
	m_pCSConstant[uSlot] = pBuffer;
}

void CGXContext::setShader(IGXShaderSet *pSH)
{
	if(m_pShader == pSH)
	{
		return;
	}
	mem_release(m_pShader);
	m_pShader = pSH;
	if(pSH)
	{
		pSH->AddRef();
	}
	m_sync_state.bShader = TRUE;
}
IGXShaderSet* CGXContext::getShader()
{
	if(m_pShader)
	{
		m_pShader->AddRef();
	}
	return(m_pShader);
}

void CGXContext::setRenderBuffer(IGXRenderBuffer *pBuff)
{
	if(m_pCurRenderBuffer == pBuff)
	{
		return;
	}
	mem_release(m_pCurRenderBuffer);
	m_pCurRenderBuffer = pBuff;
	if(pBuff)
	{
		pBuff->AddRef();
	}
	m_sync_state.bRenderBuffer = TRUE;
}

void CGXContext::setSamplerState(IGXSamplerState *pState, UINT uSlot)
{
	assert(uSlot < GX_MAX_SAMPLERS);
	if(uSlot >= GX_MAX_SAMPLERS)
	{
		CGXDevice::debugMessage(GX_LOG_ERROR, "Unable to set sampler state: uSlot >= GX_MAX_SAMPLERS!");
		return;
	}
	if(m_pSamplerState[uSlot] == pState)
	{
		return;
	}
	mem_release(m_pSamplerState[uSlot]);
	m_pSamplerState[uSlot] = pState;
	if(pState)
	{
		pState->AddRef();
	}
	m_sync_state.bSamplerState[uSlot] = TRUE;
}
IGXSamplerState* CGXContext::getSamplerState(UINT uSlot)
{
	assert(uSlot < GX_MAX_SAMPLERS);
	if(m_pSamplerState[uSlot])
	{
		m_pSamplerState[uSlot]->AddRef();
	}
	return(m_pSamplerState[uSlot]);
}

void CGXContext::setRasterizerState(IGXRasterizerState *pState)
{
	if(m_pRasterizerState == pState)
	{
		return;
	}
	mem_release(m_pRasterizerState);
	m_pRasterizerState = pState;
	if(pState)
	{
		pState->AddRef();
	}
	m_sync_state.bRasterizerState = TRUE;
}
IGXRasterizerState* CGXContext::getRasterizerState()
{
	if(m_pRasterizerState)
	{
		m_pRasterizerState->AddRef();
	}
	return(m_pRasterizerState);
}
void CGXContext::setScissorRect(int iTop, int iRight, int iBottom, int iLeft)
{
	m_rcScissors = {iLeft, iTop, iRight, iBottom};
	m_sync_state.bScissorsRect = TRUE;
}

void CGXContext::setDepthStencilState(IGXDepthStencilState *pState)
{
	if(m_pDepthStencilState == pState)
	{
		return;
	}
	mem_release(m_pDepthStencilState);
	m_pDepthStencilState = pState;
	if(pState)
	{
		pState->AddRef();
	}
	m_sync_state.bDepthStencilState = TRUE;
}
IGXDepthStencilState* CGXContext::getDepthStencilState()
{
	if(m_pDepthStencilState)
	{
		m_pDepthStencilState->AddRef();
	}
	return(m_pDepthStencilState);
}
void CGXContext::setStencilRef(UINT uVal)
{
	m_uStencilRef = uVal;
	m_sync_state.bDepthStencilState = TRUE;
}

void CGXContext::setBlendState(IGXBlendState *pState)
{
	if(m_pBlendState == pState)
	{
		return;
	}
	mem_release(m_pBlendState);
	m_pBlendState = pState;
	if(pState)
	{
		pState->AddRef();
	}
	m_sync_state.bBlendState = TRUE;
}
IGXBlendState* CGXContext::getBlendState()
{
	if(m_pBlendState)
	{
		m_pBlendState->AddRef();
	}
	return(m_pBlendState);
}
void CGXContext::setBlendFactor(GXCOLOR val)
{
	m_blendFactor = val;
	m_sync_state.bBlendState = TRUE;
}

void CGXContext::setDepthStencilSurface(IGXDepthStencilSurface *pSurface)
{
	if(m_pDepthStencilSurface == pSurface)
	{
		return;
	}
	mem_release(m_pDepthStencilSurface);
	if(!pSurface)
	{
		pSurface = m_pDefaultDepthStencilSurface;
	}

	m_pDepthStencilSurface = pSurface;
	pSurface->AddRef();

	m_sync_state.bRenderTarget = TRUE;
}
void CGXContext::unsetDepthStencilSurface()
{
	if(!m_pDepthStencilSurface)
	{
		return;
	}
	mem_release(m_pDepthStencilSurface);

	m_pDepthStencilSurface = NULL;
	m_sync_state.bRenderTarget = TRUE;
}
IGXDepthStencilSurface* CGXContext::getDepthStencilSurface()
{
	if(m_pDepthStencilSurface)
	{
		m_pDepthStencilSurface->AddRef();
	}
	return(m_pDepthStencilSurface);
}

void CGXContext::downsampleColorTarget(IGXSurface *pSource, IGXSurface *pTarget)
{
	assert(pSource);
	assert(pTarget);
	CGXSurface *pSrcSurface = (CGXSurface*)pSource;
	CGXSurface *pTgtSurface = (CGXSurface*)pTarget;

	//syncronize(GX_SYNCFLAG_NO_SHADER);
	m_pDeviceContext->ResolveSubresource(pTgtSurface->m_pSurface, 0, pSrcSurface->m_pSurface, 0, pSrcSurface->m_descTex2D.Format);
}
void CGXContext::setColorTarget(IGXSurface *pSurf, UINT idx)
{
	assert(idx < GX_MAX_COLORTARGETS);
	if(m_pColorTarget[idx] == pSurf)
	{
		return;
	}
	mem_release(m_pColorTarget[idx]);
	
	if(!pSurf)
	{
		if(idx == 0)
		{
			pSurf = m_pGXDevice->m_pDefaultSwapChain->getColorTarget();
		}
	}
	else
	{
		pSurf->AddRef();
	}
	m_pColorTarget[idx] = pSurf;
	
	m_sync_state.bRenderTarget = TRUE;

#if 0
	bool bFound = false;
	for(UINT i = 0; i < GX_MAX_TEXTURES && !bFound; ++i)
	{
		if(m_pTextures[i])
		{
			switch(m_pTextures[i]->getType())
			{
			case GXTEXTURE_TYPE_2D:
			{
				CGXTexture2D *pTex = (CGXTexture2D*)m_pTextures[i];
				if(pTex->m_descTex2D.BindFlags & D3D11_BIND_RENDER_TARGET)
				{
					IGXSurface *pTexSurf = pTex->getMipmap(0);
					if(pTexSurf == pSurf)
					{
						m_pTextures[i] = NULL;
						bFound = true;
						break;
					}
					mem_release(pTexSurf);
				}
				break;
			}
			case GXTEXTURE_TYPE_3D:
			{
				CGXTexture3D *pTex = (CGXTexture3D*)m_pTextures[i];
				if(pTex->m_descTex3D.BindFlags & D3D11_BIND_RENDER_TARGET)
				{
					IGXSurface *pTexSurf = pTex->asRenderTarget();
					if(pTexSurf == pSurf)
					{
						m_pTextures[i] = NULL;
						bFound = true;
						break;
					}
					mem_release(pTexSurf);
				}
				break;
			}
			case GXTEXTURE_TYPE_CUBE:
			{
				CGXTextureCube *pTex = (CGXTextureCube*)m_pTextures[i];
				if(pTex->m_descTex2D.BindFlags & D3D11_BIND_RENDER_TARGET)
				{
					IGXSurface *pTexSurf = pTex->asRenderTarget();
					if(pTexSurf == pSurf)
					{
						m_pTextures[i] = NULL;
						bFound = true;
						break;
					}
					mem_release(pTexSurf);
				}
				break;
			}
			}
		}
	}
#endif
}

IGXSurface* CGXContext::getColorTarget(UINT idx)
{
	assert(idx < GX_MAX_COLORTARGETS);
	if(m_pColorTarget[idx])
	{
		m_pColorTarget[idx]->AddRef();
	}
	return(m_pColorTarget[idx]);
}

void CGXContext::setPSTexture(IGXBaseTexture *pTexture, UINT uStage)
{
	assert(uStage < GX_MAX_TEXTURES);
#if 1
	if(!pTexture && m_pTextures[uStage] == pTexture)
	{
		return;
	}
#endif
	mem_release(m_pTextures[uStage]);
	m_pTextures[uStage] = pTexture;
	if(pTexture)
	{
		pTexture->AddRef();
	}
	m_sync_state.bTexture[uStage] = TRUE;
}
IGXBaseTexture* CGXContext::getPSTexture(UINT uStage)
{
	assert(uStage < GX_MAX_TEXTURES);
	if(m_pTextures[uStage])
	{
		m_pTextures[uStage]->AddRef();
	}
	return(m_pTextures[uStage]);
}

void CGXContext::setVSTexture(IGXBaseTexture *pTexture, UINT uStage)
{
	assert(uStage < GX_MAX_TEXTURES);
#if 0
	if(m_pTexturesVS[uStage] == pTexture)
	{
		return;
	}
#endif
	mem_release(m_pTexturesVS[uStage]);
	m_pTexturesVS[uStage] = pTexture;
	if(pTexture)
	{
		pTexture->AddRef();
	}
	m_sync_state.bTextureVS[uStage] = TRUE;
}
IGXBaseTexture* CGXContext::getVSTexture(UINT uStage)
{
	assert(uStage < GX_MAX_TEXTURES);
	if(m_pTexturesVS[uStage])
	{
		m_pTexturesVS[uStage]->AddRef();
	}
	return(m_pTexturesVS[uStage]);
}

void CGXContext::setCSTexture(IGXBaseTexture *pTexture, UINT uStage)
{
	assert(uStage < GX_MAX_TEXTURES);
	mem_release(m_pTexturesCS[uStage]);
	m_pTexturesCS[uStage] = pTexture;
	if(pTexture)
	{
		pTexture->AddRef();
	}
	m_sync_state.bTextureCS[uStage] = TRUE;
}
IGXBaseTexture* CGXContext::getCSTexture(UINT uStage)
{
	assert(uStage < GX_MAX_TEXTURES);
	if(m_pTexturesCS[uStage])
	{
		m_pTexturesCS[uStage]->AddRef();
	}
	return(m_pTexturesCS[uStage]);
}

void CGXContext::setCSUnorderedAccessView(IGXBaseTexture *pTexture, UINT uStage)
{
	assert(uStage < GX_MAX_UAV_TEXTURES);
	mem_release(m_pUAVsCS[uStage]);
	m_pUAVsCS[uStage] = pTexture;
	if(pTexture)
	{
		pTexture->AddRef();
	}
	m_sync_state.bUAVsCS[uStage] = TRUE;
}
IGXBaseTexture* CGXContext::getCSUnorderedAccessView(UINT uStage)
{
	assert(uStage < GX_MAX_UAV_TEXTURES);
	if(m_pUAVsCS[uStage])
	{
		m_pUAVsCS[uStage]->AddRef();
	}
	return(m_pUAVsCS[uStage]);
}


ID3D11DeviceContext* CGXContext::getDXDeviceContext()
{
	return(m_pDeviceContext);
}

void CGXContext::_updateStats(UINT uPrimCount)
{
	++m_frameStats.uDIPcount;
	switch(m_drawPT)
	{
	case D3D_PRIMITIVE_TOPOLOGY_POINTLIST:
		m_frameStats.uPointCount += uPrimCount;
		break;
	case D3D_PRIMITIVE_TOPOLOGY_LINELIST:
	case D3D_PRIMITIVE_TOPOLOGY_LINESTRIP:
		m_frameStats.uLineCount += uPrimCount;
		break;
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		m_frameStats.uPolyCount += uPrimCount;
		break;
	}
}

UINT CGXContext::getIDXcount(UINT ptCount)
{
	switch(m_drawPT)
	{
	case D3D_PRIMITIVE_TOPOLOGY_POINTLIST:
		return(ptCount);
	case D3D_PRIMITIVE_TOPOLOGY_LINELIST:
		return(ptCount * 2);
	case D3D_PRIMITIVE_TOPOLOGY_LINESTRIP:
		return(ptCount + 1);
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		return(ptCount * 3);
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		return(ptCount + 2);
	}
	return(0);
}
