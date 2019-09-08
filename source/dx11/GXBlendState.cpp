#include "GXBlendState.h"

CGXBlendState::~CGXBlendState()
{
	mem_release(m_pStateBlock);
}

void CGXBlendState::onDevLost()
{
	mem_release(m_pStateBlock);
}
void CGXBlendState::onDevRst()
{
	auto m_pDevice = m_pRender->getDXDevice();

	D3D11_BLEND_DESC blendDesc;
	blendDesc.AlphaToCoverageEnable = m_desc.useAlphaToCoverage;
	blendDesc.IndependentBlendEnable = m_desc.useIndependentBlend;
	for(int i = 0; i < (blendDesc.IndependentBlendEnable ? 8 : 1); ++i)
	{
		if(blendDesc.RenderTarget[i].BlendEnable = m_desc.renderTarget[i].useBlend)
		{
			blendDesc.RenderTarget[i].SrcBlend = (D3D11_BLEND)m_desc.renderTarget[i].blendSrcColor;
			blendDesc.RenderTarget[i].DestBlend = (D3D11_BLEND)m_desc.renderTarget[i].blendDestColor;
			blendDesc.RenderTarget[i].BlendOp = (D3D11_BLEND_OP)m_desc.renderTarget[i].blendOpColor;

			blendDesc.RenderTarget[i].SrcBlendAlpha = (D3D11_BLEND)m_desc.renderTarget[i].blendSrcAlpha;
			blendDesc.RenderTarget[i].DestBlendAlpha = (D3D11_BLEND)m_desc.renderTarget[i].blendDestAlpha;
			blendDesc.RenderTarget[i].BlendOpAlpha = (D3D11_BLEND_OP)m_desc.renderTarget[i].blendOpAlpha;
		}
		blendDesc.RenderTarget[i].RenderTargetWriteMask = m_desc.renderTarget[i].u8RenderTargetWriteMask;
	}

	DX_CALL(m_pDevice->CreateBlendState(&blendDesc, &m_pStateBlock));
}
