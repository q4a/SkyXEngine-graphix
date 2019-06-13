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

	dsDesc.DepthEnable = m_desc.useDepthTest;
	dsDesc.DepthWriteMask = m_desc.useDepthWrite ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = (D3D11_COMPARISON_FUNC)m_desc.cmpFuncDepth;
	dsDesc.StencilEnable = m_desc.useStencilTest;
	dsDesc.StencilReadMask = m_desc.u8StencilReadMask;
	dsDesc.StencilWriteMask = m_desc.u8StencilWriteMask;
	dsDesc.FrontFace.StencilFunc = (D3D11_COMPARISON_FUNC)m_desc.stencilTestFront.cmpFuncStencil;
	dsDesc.FrontFace.StencilPassOp = (D3D11_STENCIL_OP)m_desc.stencilTestFront.stencilOpPass;
	dsDesc.FrontFace.StencilFailOp = (D3D11_STENCIL_OP)m_desc.stencilTestFront.stencilOpFail;
	dsDesc.FrontFace.StencilDepthFailOp = (D3D11_STENCIL_OP)m_desc.stencilTestFront.stencilOpDepthFail;

	dsDesc.BackFace.StencilFunc = (D3D11_COMPARISON_FUNC)m_desc.stencilTestBack.cmpFuncStencil;
	dsDesc.BackFace.StencilPassOp = (D3D11_STENCIL_OP)m_desc.stencilTestBack.stencilOpPass;
	dsDesc.BackFace.StencilFailOp = (D3D11_STENCIL_OP)m_desc.stencilTestBack.stencilOpFail;
	dsDesc.BackFace.StencilDepthFailOp = (D3D11_STENCIL_OP)m_desc.stencilTestBack.stencilOpDepthFail;

	DX_CALL(m_pDevice->CreateDepthStencilState(&dsDesc, &m_pStateBlock));
}
