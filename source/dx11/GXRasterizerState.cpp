#include "GXRasterizerState.h"

CGXRasterizerState::~CGXRasterizerState()
{
	mem_release(m_pStateBlock);
}

void CGXRasterizerState::onDevLost()
{
	mem_release(m_pStateBlock);
}
void CGXRasterizerState::onDevRst()
{
	auto m_pDevice = m_pRender->getDXDevice();

	D3D11_RASTERIZER_DESC rsDesc;
	rsDesc.FrontCounterClockwise = FALSE;
	rsDesc.DepthBias = m_desc.iDepthBias;
	rsDesc.DepthBiasClamp = m_desc.fDepthBiasClamp;
	rsDesc.SlopeScaledDepthBias = m_desc.fSlopeScaledDepthBias;
	rsDesc.DepthClipEnable = m_desc.useDepthClip;
	rsDesc.ScissorEnable = m_desc.useScissorTest;
	rsDesc.MultisampleEnable = m_desc.useMultisample;
	rsDesc.AntialiasedLineEnable = m_desc.useAntialiasedLine;

	switch(m_desc.fillMode)
	{
	case GXFILL_SOLID:
		rsDesc.FillMode = D3D11_FILL_SOLID;
		break;
	case GXFILL_WIREFRAME:
		rsDesc.FillMode = D3D11_FILL_WIREFRAME;
		break;
	}

	switch(m_desc.cullMode)
	{
	case GXCULL_NONE:
		rsDesc.CullMode = D3D11_CULL_NONE;
		break;
	case GXCULL_FRONT:
		rsDesc.CullMode = D3D11_CULL_FRONT;
		break;
	case GXCULL_BACK:
		rsDesc.CullMode = D3D11_CULL_BACK;
		break;
	}
	DX_CALL(m_pDevice->CreateRasterizerState(&rsDesc, &m_pStateBlock));
}

