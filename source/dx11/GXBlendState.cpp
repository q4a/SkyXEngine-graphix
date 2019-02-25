#include "GXBlendState.h"

void CGXBlendState::Release()
{
	--m_uRefCount;
	if(!m_uRefCount)
	{
		m_pRender->destroyBlendState(this);
	}
}

CGXBlendState::~CGXBlendState()
{
	--m_uRefCount;
	if(!m_uRefCount)
	{
		mem_release(m_pStateBlock);
	}
}

void CGXBlendState::onDevLost()
{
	mem_release(m_pStateBlock);
}
void CGXBlendState::onDevRst()
{
	auto m_pDevice = m_pRender->getDXDevice();

	D3D11_BLEND_DESC blendDesc;
	blendDesc.AlphaToCoverageEnable = m_desc.bAlphaToCoverageEnable;
	blendDesc.IndependentBlendEnable = m_desc.bIndependentBlendEnabled;
	for(UINT i = 0; i < blendDesc.IndependentBlendEnable ? 8 : 1; ++i)
	{
		if(blendDesc.RenderTarget[i].BlendEnable = m_desc.renderTarget[i].bBlendEnable)
		{
			blendDesc.RenderTarget[i].SrcBlend = (D3D11_BLEND)m_desc.renderTarget[i].srcBlend;
			blendDesc.RenderTarget[i].DestBlend = (D3D11_BLEND)m_desc.renderTarget[i].destBlend;
			blendDesc.RenderTarget[i].BlendOp = (D3D11_BLEND_OP)m_desc.renderTarget[i].blendOp;

			blendDesc.RenderTarget[i].SrcBlendAlpha = (D3D11_BLEND)m_desc.renderTarget[i].srcBlendAlpha;
			blendDesc.RenderTarget[i].DestBlendAlpha = (D3D11_BLEND)m_desc.renderTarget[i].destBlendAlpha;
			blendDesc.RenderTarget[i].BlendOpAlpha = (D3D11_BLEND_OP)m_desc.renderTarget[i].blendOpAlpha;
		}
		blendDesc.RenderTarget[i].RenderTargetWriteMask = m_desc.renderTarget[i].u8RenderTargetWriteMask;
	}

	m_pDevice->CreateBlendState(&blendDesc, &m_pStateBlock);
}
