#include "GXDepthStencilState.h"

void CGXDepthStencilState::Release()
{
	--m_uRefCount;
	if(!m_uRefCount)
	{
		m_pRender->destroyDepthStencilState(this);
	}
}

CGXDepthStencilState::~CGXDepthStencilState()
{
	mem_release(m_pStateBlock);
}

void CGXDepthStencilState::onDevLost()
{
	mem_release(m_pStateBlock);
}
void CGXDepthStencilState::onDevRst()
{
	auto m_pDevice = m_pRender->getDXDevice();

	D3D11_DEPTH_STENCIL_DESC dsDesc;
	memset(&dsDesc, 0, sizeof(dsDesc));

	dsDesc.DepthEnable = m_desc.bDepthEnable;
	dsDesc.DepthWriteMask = m_desc.bEnableDepthWrite ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = (D3D11_COMPARISON_FUNC)m_desc.depthFunc;
	dsDesc.StencilEnable = m_desc.bStencilEnable;
	dsDesc.StencilReadMask = m_desc.u8StencilReadMask;
	dsDesc.StencilWriteMask = m_desc.u8StencilWriteMask;
	dsDesc.FrontFace.StencilFunc = (D3D11_COMPARISON_FUNC)m_desc.stencilFunc;
	dsDesc.FrontFace.StencilPassOp = (D3D11_STENCIL_OP)m_desc.stencilPassOp;
	dsDesc.FrontFace.StencilFailOp = (D3D11_STENCIL_OP)m_desc.stencilFailOp;
	dsDesc.FrontFace.StencilDepthFailOp = (D3D11_STENCIL_OP)m_desc.stencilDepthFailOp;
	dsDesc.BackFace.StencilFunc = (D3D11_COMPARISON_FUNC)m_desc.stencilBackFunc;
	dsDesc.BackFace.StencilPassOp = (D3D11_STENCIL_OP)m_desc.stencilBackPassOp;
	dsDesc.BackFace.StencilFailOp = (D3D11_STENCIL_OP)m_desc.stencilBackFailOp;
	dsDesc.BackFace.StencilDepthFailOp = (D3D11_STENCIL_OP)m_desc.stencilBackDepthFailOp;

	DX_CALL(m_pDevice->CreateDepthStencilState(&dsDesc, &m_pStateBlock));
}
