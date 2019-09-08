#include "GXDepthStencilState.h"

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

	dsDesc.DepthEnable = m_desc.useDepthTest;
	dsDesc.DepthWriteMask = m_desc.useDepthWrite ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = (D3D11_COMPARISON_FUNC)m_desc.cmpFuncDepth;
	dsDesc.StencilEnable = m_desc.useStencilTest;
	dsDesc.StencilReadMask = m_desc.u8StencilReadMask;
	dsDesc.StencilWriteMask = m_desc.u8StencilWriteMask;
	dsDesc.FrontFace.StencilFunc = (D3D11_COMPARISON_FUNC)m_desc.stencilTestFront.cmpFunc;
	dsDesc.FrontFace.StencilPassOp = (D3D11_STENCIL_OP)m_desc.stencilTestFront.opPass;
	dsDesc.FrontFace.StencilFailOp = (D3D11_STENCIL_OP)m_desc.stencilTestFront.opFail;
	dsDesc.FrontFace.StencilDepthFailOp = (D3D11_STENCIL_OP)m_desc.stencilTestFront.opDepthFail;

	dsDesc.BackFace.StencilFunc = (D3D11_COMPARISON_FUNC)m_desc.stencilTestBack.cmpFunc;
	dsDesc.BackFace.StencilPassOp = (D3D11_STENCIL_OP)m_desc.stencilTestBack.opPass;
	dsDesc.BackFace.StencilFailOp = (D3D11_STENCIL_OP)m_desc.stencilTestBack.opFail;
	dsDesc.BackFace.StencilDepthFailOp = (D3D11_STENCIL_OP)m_desc.stencilTestBack.opDepthFail;

	DX_CALL(m_pDevice->CreateDepthStencilState(&dsDesc, &m_pStateBlock));
}
