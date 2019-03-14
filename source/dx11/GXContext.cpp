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

#include <cstdio>

//флаги компиляции шейдеров


#ifdef _DEBUG
/* D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY */
#	define SHADER_FLAGS (D3DCOMPILE_DEBUG | D3DCOMPILE_AVOID_FLOW_CONTROL | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_PARTIAL_PRECISION | D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY)
//#	define SHADER_FLAGS (D3DXSHADER_DEBUG | D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY | D3DXSHADER_AVOID_FLOW_CONTROL | D3DXSHADER_SKIPOPTIMIZATION)
#else
//#	define SHADER_FLAGS (D3DXSHADER_OPTIMIZATION_LEVEL3 | D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY | D3DXSHADER_PARTIALPRECISION | D3DXSHADER_PREFER_FLOW_CONTROL)
#	define SHADER_FLAGS (D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_PARTIAL_PRECISION | D3DCOMPILE_PREFER_FLOW_CONTROL | D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY) 
#endif

CGXContext::CGXContext():
	m_pCurIndexBuffer(NULL),
	m_drawPT(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	m_pCurRenderBuffer(NULL)
{
	memset(&m_sync_state, 0, sizeof(m_sync_state));
	//memset(&m_dwCurrentSamplerStates, 0, sizeof(m_dwCurrentSamplerStates));
	memset(&m_pSamplerState, 0, sizeof(m_pSamplerState));
	memset(&m_pColorTarget, 0, sizeof(m_pColorTarget));
	memset(&m_pDXColorTarget, 0, sizeof(m_pDXColorTarget));
	memset(&m_pTextures, 0, sizeof(m_pTextures));
}

void CGXContext::Release()
{
	delete this;
}

bool CGXContext::beginFrame()
{
	memset(&m_frameStats, 0, sizeof(m_frameStats));

	return(canBeginFrame());
}
void CGXContext::endFrame()
{
	if(m_bDeviceWasReset)
	{
		m_bDeviceWasReset = false;
	}
}
bool CGXContext::canBeginFrame()
{
	return(true);
}

bool CGXContext::wasReset()
{
	return(m_bDeviceWasReset);
}

void CGXContext::onLostDevice()
{
	for(UINT i = 0, l = m_aResettableDSSurfaces.size(); i < l; ++i)
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

void CGXContext::onResetDevice()
{
	for(UINT i = 0, l = m_aResettableDSSurfaces.size(); i < l; ++i)
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
	}
	/*for(UINT i = 0, l = m_aResettableSwapChains.size(); i < l; ++i)
	{
		((CGXSwapChain*)m_aResettableSwapChains[i])->onDevRst(m_uWindowWidth, m_uWindowHeight);
	}*/

	setColorTarget(NULL);

	memset(&m_sync_state, 1, sizeof(m_sync_state));

	m_bDeviceWasReset = true;
}

void CGXContext::resize(int iWidth, int iHeight, bool isWindowed)
{
	if(iWidth < 1)
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

	((CGXSwapChain*)m_pDefaultSwapChain)->resize(iWidth, iHeight, isWindowed);
}

BOOL CGXContext::initContext(SXWINDOW wnd, int iWidth, int iHeight, bool isWindowed)
{
	m_hWnd = (HWND)wnd;

	UINT creationFlags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#if defined(_DEBUG)
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	if(FAILED(DX_CALL(D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &m_pDevice, nullptr, &m_pDeviceContext))))
	{
		return(FALSE);
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
	mem_release(pDXGIAdapter);

	m_uWindowWidth = iWidth;
	m_uWindowHeight = iHeight;
	m_isWindowed = isWindowed;

	m_pDefaultDepthStencilSurface = createDepthStencilSurface(m_uWindowWidth, m_uWindowHeight, GXFMT_D24S8, GXMULTISAMPLE_NONE, true);

	m_pDefaultSwapChain = createSwapChain(m_uWindowWidth, m_uWindowHeight, wnd, isWindowed);
	setColorTarget(NULL);

	GXSAMPLER_DESC samplerDesc;
	memset(&samplerDesc, 0, sizeof(samplerDesc));
	samplerDesc.uMaxAnisotropy = 1;
	m_pDefaultSamplerState = createSamplerState(&samplerDesc);

	GXRASTERIZER_DESC rasterizerDesc;
	memset(&rasterizerDesc, 0, sizeof(rasterizerDesc));
	rasterizerDesc.fillMode = GXFILL_SOLID;
	rasterizerDesc.cullMode = GXCULL_BACK;
	rasterizerDesc.bDepthClipEnable = TRUE;
	m_pDefaultRasterizerState = createRasterizerState(&rasterizerDesc);

	GXDEPTH_STENCIL_DESC depthStencilDesc;
	memset(&depthStencilDesc, 0, sizeof(depthStencilDesc));
	depthStencilDesc.bDepthEnable = TRUE;
	depthStencilDesc.bEnableDepthWrite = TRUE;
	depthStencilDesc.depthFunc = GXCOMPARISON_LESS_EQUAL;
	depthStencilDesc.bStencilEnable = FALSE;

	m_pDefaultDepthStencilState = createDepthStencilState(&depthStencilDesc);

	GXBLEND_DESC blendDesc;
	memset(&blendDesc, 0, sizeof(blendDesc));
	blendDesc.renderTarget[0].u8RenderTargetWriteMask = GXCOLOR_WRITE_ENABLE_ALL;
	
	m_pDefaultBlendState = createBlendState(&blendDesc);

	setDepthStencilSurface(m_pDefaultDepthStencilSurface);
	IGXSurface *pColorTarget = m_pDefaultSwapChain->getColorTarget();
	setColorTarget(pColorTarget);
	mem_release(pColorTarget);

	return(TRUE);
}

CGXContext::~CGXContext()
{
	mem_release(m_pDefaultDepthStencilSurface);
	mem_release(m_pDefaultRasterizerState);
	mem_release(m_pDefaultSamplerState);
	mem_release(m_pDefaultBlendState);
	mem_release(m_pDefaultDepthStencilState);
	mem_release(m_pDXGIFactory);
	mem_release(m_pDXGIDevice);
	mem_release(m_pDevice);
	mem_release(m_pDeviceContext);
	mem_release(m_pDefaultSwapChain);
}


void CGXContext::swapBuffers()
{
	m_pDefaultSwapChain->swapBuffers();
}

void CGXContext::clear(UINT what, GXCOLOR color, float fDepth, UINT uStencil)
{
	if(what & GXCLEAR_COLOR)
	{
		for(UINT i = 0; i < MAXGXCOLORTARGETS; ++i)
		{
			if(m_pColorTarget[i])
			{
				m_pDeviceContext->ClearRenderTargetView(((CGXSurface*)m_pColorTarget[i])->m_pRTV, (float*)&GXCOLOR_COLORVECTOR_ARGB(color));
			}
		}
	}
	if(what & (GXCLEAR_DEPTH | GXCLEAR_STENCIL))
	{
		m_pDeviceContext->ClearDepthStencilView(((CGXDepthStencilSurface*)m_pDepthStencilSurface)->m_pSurface, ((what & GXCLEAR_DEPTH) ? D3D11_CLEAR_DEPTH : 0) | ((what & GXCLEAR_STENCIL) ? D3D11_CLEAR_STENCIL : 0), fDepth, uStencil);
	}
}


IGXVertexBuffer * CGXContext::createVertexBuffer(size_t size, UINT flags, void * pInitData)
{
	CGXVertexBuffer * pBuff = new CGXVertexBuffer(this);

	D3D11_BUFFER_DESC bd;
	memset(&bd, 0, sizeof(bd));
	if(flags & GX_BUFFER_USAGE_STATIC)
	{
		pBuff->m_isLockable = false;
		bd.Usage = D3D11_USAGE_DEFAULT;
	}
	else
	{
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	bd.ByteWidth = size;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA *pSubresourceData = NULL;
	D3D11_SUBRESOURCE_DATA initData;
	if(pInitData)
	{
		memset(&initData, 0, sizeof(initData));
		initData.pSysMem = pInitData;
		initData.SysMemPitch = bd.ByteWidth;
		pSubresourceData = &initData;

		addBytesVertices(size);
	}

	if(FAILED(DX_CALL(m_pDevice->CreateBuffer(&bd, pSubresourceData, &pBuff->m_pBuffer))))
	{
		mem_delete(pBuff);
		return(NULL);
	}

	pBuff->m_uSize = size;

	return(pBuff);
}

IGXIndexBuffer * CGXContext::createIndexBuffer(size_t size, UINT flags, GXINDEXTYPE it, void * pInitData)
{
	CGXIndexBuffer * pBuff = new CGXIndexBuffer(this);
	
	D3D11_BUFFER_DESC bd;
	memset(&bd, 0, sizeof(bd));
	if(flags & GX_BUFFER_USAGE_STATIC)
	{
		pBuff->m_isLockable = false;
		bd.Usage = D3D11_USAGE_DEFAULT;
	}
	else
	{
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	bd.ByteWidth = size;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA *pSubresourceData = NULL;
	D3D11_SUBRESOURCE_DATA initData;
	if(pInitData)
	{
		memset(&initData, 0, sizeof(initData));
		initData.pSysMem = pInitData;
		initData.SysMemPitch = bd.ByteWidth;
		pSubresourceData = &initData;

		addBytesIndices(size);
	}

	if(FAILED(DX_CALL(m_pDevice->CreateBuffer(&bd, pSubresourceData, &pBuff->m_pBuffer))))
	{
		mem_delete(pBuff);
		return(NULL);
	}

	switch(it)
	{
	case GXIT_USHORT:
		pBuff->m_format = DXGI_FORMAT_R16_UINT;
		break;
	case GXIT_UINT:
		pBuff->m_format = DXGI_FORMAT_R32_UINT;
		break;
	}

	pBuff->m_uSize = size;
		
	return(pBuff);
}

void CGXContext::destroyIndexBuffer(IGXIndexBuffer * pBuff)
{
	if(pBuff)
	{
		if(m_pCurIndexBuffer == pBuff)
		{
			m_pCurIndexBuffer = NULL;
			m_sync_state.bIndexBuffer = TRUE;
		}
	}
	mem_delete(pBuff);
}

void CGXContext::destroyVertexBuffer(IGXVertexBuffer * pBuff)
{
	if(pBuff)
	{
		/*if(m_pCurIndexBuffer == pBuff)
		{
			m_pCurIndexBuffer = NULL;
			m_sync_state.bIndexBuffer = TRUE;
		}*/

	}
	mem_delete(pBuff);
}

IGXVertexDeclaration * CGXContext::createVertexDeclaration(const GXVERTEXELEMENT * pDecl)
{
	CGXVertexDeclaration * vd = new CGXVertexDeclaration(m_pDevice, this, pDecl);

	return(vd);
}
void CGXContext::destroyVertexDeclaration(IGXVertexDeclaration * pDecl)
{
	if(m_pCurVertexDecl == pDecl)
	{
		//m_pGL->glDeleteBuffers(1, &((IDSGvertexDeclarationOgl*)m_pCurVertexDecl)->m_pVAO);
		m_pCurVertexDecl = NULL;
		//m_sync_state.bVertexLayout = TRUE;
	}
	mem_delete(pDecl);
	
}

void CGXContext::setIndexBuffer(IGXIndexBuffer * pBuff)
{
	mem_release(m_pCurIndexBuffer);
	m_pCurIndexBuffer = pBuff;
	if(pBuff)
	{
		pBuff->AddRef();
	}
	m_sync_state.bIndexBuffer = TRUE;
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

		m_sync_state.bRenderBuffer = FALSE;
	}
	if(m_sync_state.bIndexBuffer)
	{
		if(m_pCurIndexBuffer)
		{
			CGXIndexBuffer *pIB = (CGXIndexBuffer*)m_pCurIndexBuffer;
			m_pDeviceContext->IASetIndexBuffer(pIB->m_pBuffer, pIB->m_format, 0);
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
	for(UINT i = 0; i < MAX_GXSAMPLERS; ++i)
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
		m_pDeviceContext->OMSetBlendState(pBS->m_pStateBlock, (float*)&GXCOLOR_COLORVECTOR_ARGB(m_blendFactor), 0xffffffff);
		m_sync_state.bBlendState = FALSE;
	}

	if(m_sync_state.bRenderTarget)
	{
		UINT uMaxIdx = 0;
		for(UINT i = 0; i < MAXGXCOLORTARGETS; ++i)
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
		m_pDeviceContext->OMSetRenderTargets(uMaxIdx + 1, m_pDXColorTarget, ((CGXDepthStencilSurface*)m_pDepthStencilSurface)->m_pSurface);

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

	
	
	
	if(!m_pShader)
	{
		debugMessage(GX_LOG_ERROR, "Shader is not set!");
	}
	else
	{
		CGXShader *pShader = (CGXShader*)m_pShader;
		if(m_sync_state.bShader)
		{
			if(pShader->m_pVShader)
			{
				CGXVertexShader *pVS = (CGXVertexShader*)pShader->m_pVShader;

				m_pDeviceContext->VSSetShader(pVS->m_pShader, NULL, 0);
			}
			else
			{
				m_pDeviceContext->VSSetShader(NULL, NULL, 0);
			}
			if(pShader->m_pPShader)
			{
				CGXPixelShader *pPS = (CGXPixelShader*)pShader->m_pPShader;

				m_pDeviceContext->PSSetShader(pPS->m_pShader, NULL, 0);
			}
			else
			{
				m_pDeviceContext->PSSetShader(NULL, NULL, 0);
			}

			if(pShader->m_pGShader)
			{
				CGXGeometryShader *pGS = (CGXGeometryShader*)pShader->m_pGShader;

				m_pDeviceContext->GSSetShader(pGS->m_pShader, NULL, 0);
			}
			else
			{
				m_pDeviceContext->GSSetShader(NULL, NULL, 0);
			}
			m_sync_state.bShader = FALSE;
		}
	}

	for(UINT i = 0; i < MAXGXTEXTURES; ++i)
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
				case GXTEXTURE_TYPE_CUBE:
					pTex = ((CGXTextureCube*)m_pTextures[i])->getDXTexture();
					break;
				}
				m_pDeviceContext->PSSetShaderResources(i, 1, &pTex);
			}
			else
			{
				ID3D11ShaderResourceView *pNull = NULL;
				m_pDeviceContext->PSSetShaderResources(i, 1, &pNull);
			}

			m_sync_state.bTexture[i] = FALSE;
		}
	}

	if(m_isScissorsEnable && m_sync_state.bScissorsRect)
	{
		m_pDeviceContext->RSSetScissorRects(1, &m_rcScissors);
	}
}

void CGXContext::setPrimitiveTopology(GXPT pt)
{
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
	}
}

UINT CGXContext::getPTcount(UINT idxCount)
{
	switch(m_drawPT)
	{
	case D3D11_PRIMITIVE_TOPOLOGY_POINTLIST:
		return(idxCount);
	case D3D11_PRIMITIVE_TOPOLOGY_LINELIST:
		return(idxCount / 2);
	case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP:
		return(idxCount - 1);
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		return(idxCount / 3);
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		return(idxCount - 2);
	}
	return(0);
}

UINT CGXContext::getIndexSize(DXGI_FORMAT idx)
{
	switch(idx)
	{
	case DXGI_FORMAT_R16_UINT:
		return(2);
	case DXGI_FORMAT_R32_UINT:
		return(4);
	}
	return(1);
}

IGXVertexShader * CGXContext::createVertexShader(const char * szFile, GXMACRO *pDefs)
{
	ID3DBlob *pShaderBlob = NULL;
	ID3DBlob *pErrorBlob = NULL;
	if(FAILED(DX_CALL(D3DX11CompileFromFileA(szFile, (D3D_SHADER_MACRO*)pDefs, NULL, "main", "vs_4_0", SHADER_FLAGS, 0, NULL, &pShaderBlob, &pErrorBlob, NULL))))
	{
		if(pErrorBlob)
		{
			int s = strlen((char*)pErrorBlob->GetBufferPointer());
			char *str = (char*)alloca(s + 33);
			sprintf(str, "Unable to create vertex shader: %s", (char*)pErrorBlob->GetBufferPointer());
			debugMessage(GX_LOG_ERROR, str);
			mem_release(pErrorBlob);
			return(NULL);
		}
	}
	mem_release(pErrorBlob);

	IGXVertexShader *pShader = createVertexShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize());

	mem_release(pShaderBlob);
	
	return(pShader);
}
IGXVertexShader * CGXContext::createVertexShaderFromString(const char * szCode, GXMACRO *pDefs)
{
	ID3DBlob *pShaderBlob = NULL;
	ID3DBlob *pErrorBlob = NULL;
	if(FAILED(DX_CALL(D3DX11CompileFromMemory(szCode, strlen(szCode), "memory.vs", (D3D_SHADER_MACRO*)pDefs, NULL, "main", "vs_4_0", SHADER_FLAGS, 0, NULL, &pShaderBlob, &pErrorBlob, NULL))))
	{
		if(pErrorBlob)
		{
			int s = strlen((char*)pErrorBlob->GetBufferPointer());
			char *str = (char*)alloca(s + 33);
			sprintf(str, "Unable to create vertex shader: %s", (char*)pErrorBlob->GetBufferPointer());
			debugMessage(GX_LOG_ERROR, str);
			mem_release(pErrorBlob);
			return(NULL);
		}
	}
	mem_release(pErrorBlob);

	IGXVertexShader *pShader = createVertexShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize());

	mem_release(pShaderBlob);

	return(pShader);
}
IGXVertexShader * CGXContext::createVertexShader(void *_pData, UINT uSize)
{
	CGXVertexShader *pShader = new CGXVertexShader(this);

	DX_CALL(D3DCreateBlob(uSize, &pShader->m_pShaderBlob));
	memcpy(pShader->m_pShaderBlob->GetBufferPointer(), _pData, uSize);

	if(FAILED(DX_CALL(m_pDevice->CreateVertexShader(pShader->m_pShaderBlob->GetBufferPointer(), uSize, NULL, &pShader->m_pShader))))
	{
		mem_delete(pShader);
		return(NULL);
	}

	return(pShader);
}
void CGXContext::destroyVertexShader(IGXVertexShader * pSH)
{
	mem_delete(pSH);
}
void CGXContext::setVertexShaderConstant(IGXConstantBuffer *pBuffer, UINT uSlot)
{
	//@TODO: defer this
	ID3D11Buffer *pBuf = NULL;
	if(pBuffer)
	{
		pBuf = ((CGXConstantBuffer*)pBuffer)->m_pBuffer;
	}
	m_pDeviceContext->VSSetConstantBuffers(uSlot, 1, &pBuf);
}

IGXPixelShader * CGXContext::createPixelShader(const char * szFile, GXMACRO *pDefs)
{
	ID3DBlob *pShaderBlob = NULL;
	ID3DBlob *pErrorBlob = NULL;
	if(FAILED(DX_CALL(D3DX11CompileFromFileA(szFile, (D3D_SHADER_MACRO*)pDefs, NULL, "main", "ps_4_0", SHADER_FLAGS, 0, NULL, &pShaderBlob, &pErrorBlob, NULL))))
	{
		if(pErrorBlob)
		{
			int s = strlen((char*)pErrorBlob->GetBufferPointer());
			char *str = (char*)alloca(s + 33);
			sprintf(str, "Unable to create pixel shader: %s", (char*)pErrorBlob->GetBufferPointer());
			debugMessage(GX_LOG_ERROR, str);
			mem_release(pErrorBlob);
			return(NULL);
		}
	}
	mem_release(pErrorBlob);

	IGXPixelShader *pShader = createPixelShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize());

	mem_release(pShaderBlob);

	return(pShader);
}
IGXPixelShader * CGXContext::createPixelShader(void *_pData, UINT uSize)
{
	CGXPixelShader *pShader = new CGXPixelShader(this);

	DX_CALL(D3DCreateBlob(uSize, &pShader->m_pShaderBlob));
	memcpy(pShader->m_pShaderBlob->GetBufferPointer(), _pData, uSize);

	if(FAILED(DX_CALL(m_pDevice->CreatePixelShader(pShader->m_pShaderBlob->GetBufferPointer(), uSize, NULL, &pShader->m_pShader))))
	{
		mem_delete(pShader);
		return(NULL);
	}

	return(pShader);
}
IGXPixelShader * CGXContext::createPixelShaderFromString(const char * szCode, GXMACRO *pDefs)
{
	ID3DBlob *pShaderBlob = NULL;
	ID3DBlob *pErrorBlob = NULL;
	if(FAILED(DX_CALL(D3DX11CompileFromMemory(szCode, strlen(szCode), "memory.ps", (D3D_SHADER_MACRO*)pDefs, NULL, "main", "ps_4_0", SHADER_FLAGS, 0, NULL, &pShaderBlob, &pErrorBlob, NULL))))
	{
		if(pErrorBlob)
		{
			int s = strlen((char*)pErrorBlob->GetBufferPointer());
			char *str = (char*)alloca(s + 33);
			sprintf(str, "Unable to create pixel shader: %s", (char*)pErrorBlob->GetBufferPointer());
			debugMessage(GX_LOG_ERROR, str);
			mem_release(pErrorBlob);
			return(NULL);
		}
	}
	mem_release(pErrorBlob);

	IGXPixelShader *pShader = createPixelShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize());

	mem_release(pShaderBlob);

	return(pShader);
}
void CGXContext::destroyPixelShader(IGXPixelShader * pSH)
{
	mem_delete(pSH);
}
void CGXContext::setPixelShaderConstant(IGXConstantBuffer *pBuffer, UINT uSlot)
{
	//@TODO: defer this
	ID3D11Buffer *pBuf = NULL;
	if(pBuffer)
	{
		pBuf = ((CGXConstantBuffer*)pBuffer)->m_pBuffer;
	}
	m_pDeviceContext->PSSetConstantBuffers(uSlot, 1, &pBuf);
}

IGXGeometryShader * CGXContext::createGeometryShader(const char * szFile, GXMACRO *pDefs)
{
	ID3DBlob *pShaderBlob = NULL;
	ID3DBlob *pErrorBlob = NULL;
	if(FAILED(DX_CALL(D3DX11CompileFromFileA(szFile, (D3D_SHADER_MACRO*)pDefs, NULL, "main", "gs_4_0", SHADER_FLAGS, 0, NULL, &pShaderBlob, &pErrorBlob, NULL))))
	{
		if(pErrorBlob)
		{
			int s = strlen((char*)pErrorBlob->GetBufferPointer());
			char *str = (char*)alloca(s + 35);
			sprintf(str, "Unable to create geometry shader!\n%s", (char*)pErrorBlob->GetBufferPointer());
			debugMessage(GX_LOG_ERROR, str);
			mem_release(pErrorBlob);
			return(NULL);
		}
	}
	mem_release(pErrorBlob);

	IGXGeometryShader *pShader = createGeometryShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize());

	mem_release(pShaderBlob);

	return(pShader);
}
IGXGeometryShader * CGXContext::createGeometryShader(void *_pData, UINT uSize)
{
	CGXGeometryShader *pShader = new CGXGeometryShader(this);

	DX_CALL(D3DCreateBlob(uSize, &pShader->m_pShaderBlob));
	memcpy(pShader->m_pShaderBlob->GetBufferPointer(), _pData, uSize);

	if(FAILED(DX_CALL(m_pDevice->CreateGeometryShader(pShader->m_pShaderBlob->GetBufferPointer(), uSize, NULL, &pShader->m_pShader))))
	{
		mem_delete(pShader);
		return(NULL);
	}

	return(pShader);
}
IGXGeometryShader * CGXContext::createGeometryShaderFromString(const char * szCode, GXMACRO *pDefs)
{
	ID3DBlob *pShaderBlob = NULL;
	ID3DBlob *pErrorBlob = NULL;
	if(FAILED(DX_CALL(D3DX11CompileFromMemory(szCode, strlen(szCode), "memory.gs", (D3D_SHADER_MACRO*)pDefs, NULL, "main", "gs_4_0", SHADER_FLAGS, 0, NULL, &pShaderBlob, &pErrorBlob, NULL))))
	{
		if(pErrorBlob)
		{
			int s = strlen((char*)pErrorBlob->GetBufferPointer());
			char *str = (char*)alloca(s + 33);
			sprintf(str, "Unable to create geometry shader: %s", (char*)pErrorBlob->GetBufferPointer());
			debugMessage(GX_LOG_ERROR, str);
			mem_release(pErrorBlob);
			return(NULL);
		}
	}
	mem_release(pErrorBlob);

	IGXGeometryShader *pShader = createGeometryShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize());

	mem_release(pShaderBlob);

	return(pShader);
}
void CGXContext::destroyGeometryShader(IGXGeometryShader * pSH)
{
	mem_delete(pSH);
}
void CGXContext::setGeometryShaderConstant(IGXConstantBuffer *pBuffer, UINT uSlot)
{
	//@TODO: defer this
	ID3D11Buffer *pBuf = NULL;
	if(pBuffer)
	{
		pBuf = ((CGXConstantBuffer*)pBuffer)->m_pBuffer;
	}
	m_pDeviceContext->GSSetConstantBuffers(uSlot, 1, &pBuf);
}

IGXShader *CGXContext::createShader(IGXVertexShader *pVS, IGXPixelShader *pPS, IGXGeometryShader *pGS)
{
	return(new CGXShader(this, pVS, pPS, pGS));
}
void CGXContext::destroyShader(IGXShader *pSH)
{
	if(pSH && pSH == m_pShader)
	{
		m_pShader = NULL;
		m_sync_state.bShader = TRUE;
	}
	mem_delete(pSH);
}
void CGXContext::setShader(IGXShader *pSH)
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
IGXShader *CGXContext::getShader()
{
	if(m_pShader)
	{
		m_pShader->AddRef();
	}
	return(m_pShader);
}

IGXRenderBuffer * CGXContext::createRenderBuffer(UINT countSlots, IGXVertexBuffer ** pBuff, IGXVertexDeclaration * pDecl)
{
	assert(countSlots);
	assert(pBuff);
	assert(pDecl);

	CGXRenderBuffer * pRB = new CGXRenderBuffer(this, countSlots, pBuff, pDecl);
	/*for(UINT i = 0; i < countSlots; ++i)
	{
		((CGXVertexBuffer*)pBuff[i])->m_pBuffer->AddRef();
	}
	((CGXVertexDeclaration*)pDecl)->m_pDeclaration->AddRef();
*/
	return(pRB);
}
void CGXContext::destroyRenderBuffer(IGXRenderBuffer * pBuff)
{
	if(pBuff)
	{
		//CGXRenderBuffer *pBuf = (CGXRenderBuffer*)pBuff;

		/*for(UINT i = 0; i < pBuf->m_uStreamCount; ++i)
		{
			mem_release(((CGXVertexBuffer*)pBuf->m_ppVertexBuffers[i])->m_pBuffer);
		}
		mem_release(((CGXVertexDeclaration*)pBuf->m_pVertexDeclaration)->m_pDeclaration);*/
	}
	mem_delete(pBuff);
}

void CGXContext::setRenderBuffer(IGXRenderBuffer * pBuff)
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

void CGXContext::debugMessage(GX_LOG lvl, const char *msg)
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
}

void CGXContext::logDXcall(const char *szCondeString, HRESULT hr)
{
	static char str[4096];
	
	sprintf_s(str, "GX call failed: %s\n    %s, %s", szCondeString, DXGetErrorString(hr), DXGetErrorDescription(hr));
	debugMessage(GX_LOG_ERROR, str);
}

IGXSamplerState *CGXContext::createSamplerState(GXSAMPLER_DESC *pSamplerDesc)
{
	CGXSamplerState *pSS = new CGXSamplerState(this);

	pSS->m_desc = *pSamplerDesc;
	pSS->onDevRst();

	return(pSS);
}
void CGXContext::destroySamplerState(IGXSamplerState *pState)
{
	if(pState)
	{
		for(UINT i = 0; i < MAX_GXSAMPLERS; ++i)
		{
			if(pState == m_pSamplerState[i])
			{
				m_pSamplerState[i] = NULL;
				m_sync_state.bSamplerState[i] = TRUE;
			}
		}
	}
	mem_delete(pState);
}
void CGXContext::setSamplerState(IGXSamplerState *pState, UINT uSlot)
{
	assert(uSlot < MAX_GXSAMPLERS);
	if(uSlot >= MAX_GXSAMPLERS)
	{
		debugMessage(GX_LOG_ERROR, "Unable to set sampler state: uSlot >= MAX_GXSAMPLERS!");
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
IGXSamplerState *CGXContext::getSamplerState(UINT uSlot)
{
	assert(uSlot < MAX_GXSAMPLERS);
	if(m_pSamplerState[uSlot])
	{
		m_pSamplerState[uSlot]->AddRef();
	}
	return(m_pSamplerState[uSlot]);
}

IGXRasterizerState *CGXContext::createRasterizerState(GXRASTERIZER_DESC *pRSDesc)
{
	CGXRasterizerState *pRS = new CGXRasterizerState(this);

	pRS->m_isScissorsEnabled = pRSDesc->bScissorEnable;
	pRS->m_desc = *pRSDesc;
	pRS->onDevRst();

	return(pRS);
}
void CGXContext::destroyRasterizerState(IGXRasterizerState *pState)
{
	if(pState)
	{
		if(pState == m_pRasterizerState)
		{
			m_pRasterizerState = NULL;
			m_sync_state.bRasterizerState = TRUE;
		}
	}
	mem_delete(pState);
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
IGXRasterizerState *CGXContext::getRasterizerState()
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


IGXDepthStencilState *CGXContext::createDepthStencilState(GXDEPTH_STENCIL_DESC *pDSDesc)
{
	CGXDepthStencilState *pDS = new CGXDepthStencilState(this);
	pDS->m_desc = *pDSDesc;
	pDS->onDevRst();

	return(pDS);
}
void CGXContext::destroyDepthStencilState(IGXDepthStencilState *pState)
{
	mem_delete(pState);
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
IGXDepthStencilState *CGXContext::getDepthStencilState()
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


IGXBlendState *CGXContext::createBlendState(GXBLEND_DESC *pBSDesc)
{
	CGXBlendState *pBS = new CGXBlendState(this);
	
	pBS->m_desc = *pBSDesc;
	pBS->onDevRst();
	return(pBS);
}
void CGXContext::destroyBlendState(IGXBlendState *pState)
{
	mem_delete(pState);
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
IGXBlendState *CGXContext::getBlendState()
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

IGXDepthStencilSurface *CGXContext::createDepthStencilSurface(UINT uWidth, UINT uHeight, GXFORMAT format, GXMULTISAMPLE_TYPE multisampleType, bool bAutoResize)
{
	CGXDepthStencilSurface *pDSSurface = new CGXDepthStencilSurface(this);

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

	return(pDSSurface);
}
void CGXContext::destroyDepthStencilSurface(IGXDepthStencilSurface *pSurface)
{
	if(pSurface)
	{
		if(pSurface == m_pDepthStencilSurface)
		{
			setDepthStencilSurface(NULL);
		}

		for(UINT i = 0, l = m_aResettableDSSurfaces.size(); i < l; ++i)
		{
			if(m_aResettableDSSurfaces[i] == pSurface)
			{
				m_aResettableDSSurfaces.erase(i);
				break;
			}
		}

		mem_delete(pSurface);
	}
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
IGXDepthStencilSurface *CGXContext::getDepthStencilSurface()
{
	if(m_pDepthStencilSurface)
	{
		m_pDepthStencilSurface->AddRef();
	}
	return(m_pDepthStencilSurface);
}


IGXSurface *CGXContext::createColorTarget(UINT uWidth, UINT uHeight, GXFORMAT format, GXMULTISAMPLE_TYPE multisampleType, bool bAutoResize)
{
	CGXSurface *pColorSurface = new CGXSurface(this, uWidth, uHeight, format, NULL);

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
	return(pColorSurface);
}
void CGXContext::destroyColorTarget(IGXSurface *pSurface)
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
	assert(idx < MAXGXCOLORTARGETS);
	if(m_pColorTarget[idx] == pSurf)
	{
		return;
	}
	mem_release(m_pColorTarget[idx]);
	
	if(!pSurf)
	{
		if(idx == 0)
		{
			pSurf = m_pDefaultSwapChain->getColorTarget();
		}
	}
	else
	{
		pSurf->AddRef();
	}
	m_pColorTarget[idx] = pSurf;
	
	m_sync_state.bRenderTarget = TRUE;
}

IGXSurface *CGXContext::getColorTarget(UINT idx)
{
	assert(idx < MAXGXCOLORTARGETS);
	if(m_pColorTarget[idx])
	{
		m_pColorTarget[idx]->AddRef();
	}
	return(m_pColorTarget[idx]);
}

void CGXContext::setTexture(IGXBaseTexture *pTexture, UINT uStage)
{
	assert(uStage < MAXGXTEXTURES);
	if(m_pTextures[uStage] == pTexture)
	{
		return;
	}
	mem_release(m_pTextures[uStage]);
	m_pTextures[uStage] = pTexture;
	if(pTexture)
	{
		pTexture->AddRef();
	}
	m_sync_state.bTexture[uStage] = TRUE;
}
IGXBaseTexture *CGXContext::getTexture(UINT uStage)
{
	assert(uStage < MAXGXTEXTURES);
	if(m_pTextures[uStage])
	{
		m_pTextures[uStage]->AddRef();
	}
	return(m_pTextures[uStage]);
}

IGXTexture2D *CGXContext::createTexture2D(UINT uWidth, UINT uHeight, UINT uMipLevels, UINT uTexUsageFlags, GXFORMAT format, void * pInitData)
{
	CGXTexture2D *pTex = new CGXTexture2D(this);

	pTex->m_format = format;
	pTex->m_uHeight = uHeight;
	pTex->m_uWidth = uWidth;
	pTex->m_bWasReset = true;
	pTex->m_bAutoResize = !!(uTexUsageFlags & GX_TEXUSAGE_AUTORESIZE);

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
	if(uTexUsageFlags & GX_TEXUSAGE_RENDERTARGET)
	{
		pTex->m_descTex2D.BindFlags |= D3D11_BIND_RENDER_TARGET;
	}
	if(uTexUsageFlags & GX_TEXUSAGE_AUTOGENMIPMAPS)
	{
		pTex->m_descTex2D.MipLevels = 0;
	}
	pTex->m_uMipLevels = pTex->m_descTex2D.MipLevels;

	if(uTexUsageFlags & GX_TEXUSAGE_AUTORESIZE)
	{
		pTex->m_isResettable = true;
		m_aResettableTextures2D.push_back(pTex);
	}


	if(!(uTexUsageFlags & GX_TEXUSAGE_RENDERTARGET) && pInitData)
	{
		pTex->m_bWasReset = false;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = pInitData;
		initData.SysMemSlicePitch = NULL;
		initData.SysMemPitch = getTextureMemPitch(uWidth, format);
		addBytesTextures(initData.SysMemPitch * uHeight);

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
	else
	{
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

	return(pTex);
}
IGXTextureCube *CGXContext::createTextureCube(UINT uSize, UINT uMipLevels, UINT uTexUsageFlags, GXFORMAT format, void * pInitData)
{
	CGXTextureCube *pTex = new CGXTextureCube(this);

	pTex->m_format = format;
	pTex->m_uSize = uSize;
	pTex->m_bWasReset = true;
	pTex->m_bAutoResize = !!(uTexUsageFlags & GX_TEXUSAGE_AUTORESIZE); 
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
	if(uTexUsageFlags & GX_TEXUSAGE_RENDERTARGET)
	{
		pTex->m_descTex2D.BindFlags |= D3D11_BIND_RENDER_TARGET;
	}
	if(uTexUsageFlags & GX_TEXUSAGE_AUTOGENMIPMAPS)
	{
		pTex->m_descTex2D.MipLevels = 0;
	}
	pTex->m_uMipLevels = pTex->m_descTex2D.MipLevels;

	if(uTexUsageFlags & GX_TEXUSAGE_AUTORESIZE)
	{
		pTex->m_isResettable = true;
		m_aResettableTexturesCube.push_back(pTex);
	}

	if(pInitData)
	{
		debugMessage(GX_LOG_WARN, "Not implemented: Unable to implace init cube texture");
	}
	DX_CALL(m_pDevice->CreateTexture2D(&pTex->m_descTex2D, NULL, &pTex->m_pTexture));

	memset(&pTex->m_descSRV, 0, sizeof(pTex->m_descSRV));
	pTex->m_descSRV.Format = pTex->m_descTex2D.Format;
	pTex->m_descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	pTex->m_descSRV.TextureCube.MipLevels = pTex->m_descTex2D.MipLevels;

	DX_CALL(m_pDevice->CreateShaderResourceView(pTex->m_pTexture, &pTex->m_descSRV, &pTex->m_pSRV));

	return(pTex);
}
void CGXContext::destroyTexture2D(IGXTexture2D * pTexture)
{
	if(pTexture)
	{
		for(UINT i = 0; i < MAXGXTEXTURES; ++i)
		{
			if(pTexture == m_pTextures[i])
			{
				setTexture(NULL, i);
			}
		}

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
	}
	mem_delete(pTexture);
}
void CGXContext::destroyTextureCube(IGXTextureCube * pTexture)
{
	if(pTexture)
	{
		for(UINT i = 0; i < MAXGXTEXTURES; ++i)
		{
			if(pTexture == m_pTextures[i])
			{
				setTexture(NULL, i);
			}
		}

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
	}
	mem_delete(pTexture);
}

IGXSwapChain *CGXContext::createSwapChain(UINT uWidth, UINT uHeight, SXWINDOW wnd)
{
	return(createSwapChain(uWidth, uHeight, wnd, true));
}
IGXSwapChain *CGXContext::createSwapChain(UINT uWidth, UINT uHeight, SXWINDOW wnd, bool bWindowed)
{
	CGXSwapChain *pSC = new CGXSwapChain(this, uWidth, uHeight, wnd, bWindowed);
	m_aResettableSwapChains.push_back(pSC);
	return(pSC);
}
void CGXContext::destroySwapChain(IGXSwapChain *pSwapChain)
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
	mem_delete(pSwapChain);
}


UINT CGXContext::getTextureDataSize(UINT uPitch, UINT uHeight, GXFORMAT format)
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

UINT  CGXContext::getTextureMemPitch(UINT uWidth, GXFORMAT format)
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

	default:
		bc = false;
	}

	if(bc)
	{
		int numBlocksWide = 0;
		if(uWidth > 0)
			numBlocksWide = max(1, uWidth / 4);
		return(numBlocksWide * bcnumBytesPerBlock);
	}
	else
	{
		UINT bpp = getBitsPerPixel(format);
		return((uWidth * bpp + 7) / 8); // round up to nearest byte
	}
}

UINT CGXContext::getBitsPerPixel(GXFORMAT format)
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

	case GXFMT_R8G8B8:
		return(24);

	case GXFMT_D16:
	case GXFMT_R16F:
	case GXFMT_R5G6B5:
		return(16);

	case GXFMT_DXT1:
		return(4);

	case GXFMT_DXT3:
	case GXFMT_DXT5:
		return(8);
	}
	return(0);
}

IGXTexture2D *CGXContext::createTexture2DFromFile(const char *szFileName, UINT uTexUsageFlags, bool bAllowNonPowerOf2)
{
	CGXTexture2D *pTex = new CGXTexture2D(this);

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

	return(pTex);
}

IGXTextureCube *CGXContext::createTextureCubeFromFile(const char *szFileName, UINT uTexUsageFlags, bool bAllowNonPowerOf2)
{
	CGXTextureCube *pTex = new CGXTextureCube(this);
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

	return(pTex);
}

//const D3DCAPS9 *CGXContext::getDXDeviceCaps()
//{
//	return(&m_dxCaps);
//}

DXGI_FORMAT CGXContext::getDXFormat(GXFORMAT format)
{
	switch(format)
	{
	case GXFMT_R8G8B8:
		assert(!"GXFMT_R8G8B8 is unavailable!");
		return(DXGI_FORMAT_R8G8B8A8_UNORM);
	case GXFMT_A8R8G8B8:
		return(DXGI_FORMAT_B8G8R8A8_UNORM);
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
GXFORMAT CGXContext::getGXFormat(DXGI_FORMAT format)
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

ID3D11Device *CGXContext::getDXDevice()
{
	return(m_pDevice);
}

ID3D11DeviceContext *CGXContext::getDXDeviceContext()
{
	return(m_pDeviceContext);
}

GXTEXTURE_TYPE CGXContext::getTextureTypeFromFile(const char *szFile)
{
	D3DX11_IMAGE_INFO oTexInfo;
	memset(&oTexInfo, 0, sizeof(D3DX11_IMAGE_INFO));

	if(!FAILED(DX_CALL(D3DX11GetImageInfoFromFileA(szFile, NULL, &oTexInfo, NULL))))
	{
		if(oTexInfo.ResourceDimension == D3D11_RESOURCE_DIMENSION_TEXTURE2D)
		{
			if(oTexInfo.ArraySize == 1)
			{
				return(GXTEXTURE_TYPE_2D);
			}
			if(oTexInfo.ArraySize == 6)
			{
				return(GXTEXTURE_TYPE_CUBE);
			}
		}
	}

	return(GXTEXTURE_TYPE_UNKNOWN);
}

bool CGXContext::saveTextureToFile(const char *szTarget, IGXBaseTexture *pTexture)
{
	ID3D11Texture2D *pTex = NULL;
	switch(pTexture->getType())
	{
	case GXTEXTURE_TYPE_2D:
		pTex = ((CGXTexture2D*)pTexture)->m_pTexture;
		break;
	case GXTEXTURE_TYPE_CUBE:
		pTex = ((CGXTextureCube*)pTexture)->m_pTexture;
		break;
	}
	return(!FAILED(D3DX11SaveTextureToFileA(m_pDeviceContext, pTex, D3DX11_IFF_PNG, szTarget)));
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

IDXGIFactory *CGXContext::getDXGIFactory()
{
	return(m_pDXGIFactory);
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

IGXConstantBuffer *CGXContext::createConstantBuffer(UINT uSize)
{
	assert(uSize && (uSize % 16 == 0));

	return(new CGXConstantBuffer(this, uSize));
}
