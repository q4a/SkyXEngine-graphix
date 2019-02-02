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

	DX_CALL(m_pDevice->BeginStateBlock());

	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, m_desc.renderTarget[0].bBlendEnable);
	if(m_desc.renderTarget[0].bBlendEnable)
	{
		m_pDevice->SetRenderState(D3DRS_SRCBLEND, m_desc.renderTarget[0].srcBlend);
		m_pDevice->SetRenderState(D3DRS_DESTBLEND, m_desc.renderTarget[0].destBlend);
		m_pDevice->SetRenderState(D3DRS_BLENDOP, m_desc.renderTarget[0].blendOp);

		if(m_desc.renderTarget[0].srcBlend != m_desc.renderTarget[0].srcBlendAlpha ||
			m_desc.renderTarget[0].destBlend != m_desc.renderTarget[0].destBlendAlpha ||
			m_desc.renderTarget[0].blendOp != m_desc.renderTarget[0].blendOpAlpha)
		{
			m_pDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);

			m_pDevice->SetRenderState(D3DRS_SRCBLENDALPHA, m_desc.renderTarget[0].srcBlendAlpha);
			m_pDevice->SetRenderState(D3DRS_DESTBLENDALPHA, m_desc.renderTarget[0].destBlendAlpha);
			m_pDevice->SetRenderState(D3DRS_BLENDOPALPHA, m_desc.renderTarget[0].blendOpAlpha);
		}
		else
		{
			m_pDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, FALSE);
		}
	}
	m_pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, m_desc.renderTarget[0].u8RenderTargetWriteMask);

	DX_CALL(m_pDevice->EndStateBlock(&m_pStateBlock));
}
