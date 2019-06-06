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
	DX_CALL(m_pDevice->BeginStateBlock());
	m_pDevice->SetRenderState(D3DRS_ZENABLE, m_desc.bDepthEnable ? D3DZB_TRUE : D3DZB_FALSE);
	if(m_desc.bDepthEnable)
	{
		m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, m_desc.bEnableDepthWrite);
		m_pDevice->SetRenderState(D3DRS_ZFUNC, m_desc.depthFunc);
	}
	m_pDevice->SetRenderState(D3DRS_STENCILENABLE, m_desc.bStencilEnable);
	if(m_desc.bStencilEnable)
	{
		m_pDevice->SetRenderState(D3DRS_STENCILMASK, (DWORD)m_desc.u8StencilReadMask);
		m_pDevice->SetRenderState(D3DRS_STENCILWRITEMASK, (DWORD)m_desc.u8StencilWriteMask);
		m_pDevice->SetRenderState(D3DRS_STENCILFAIL, m_desc.stencilFailOp);
		m_pDevice->SetRenderState(D3DRS_STENCILZFAIL, m_desc.stencilDepthFailOp);
		m_pDevice->SetRenderState(D3DRS_STENCILPASS, m_desc.stencilPassOp);
		m_pDevice->SetRenderState(D3DRS_STENCILFUNC, m_desc.stencilFunc);
		if(m_desc.stencilBackFunc != GXCMP_ALWAYS || m_desc.stencilBackFailOp != GXSTENCIL_OP_KEEP || m_desc.stencilBackDepthFailOp != GXSTENCIL_OP_KEEP || m_desc.stencilBackPassOp != GXSTENCIL_OP_KEEP)
		{
			m_pDevice->SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, TRUE);
			m_pDevice->SetRenderState(D3DRS_CCW_STENCILFAIL, m_desc.stencilBackFailOp);
			m_pDevice->SetRenderState(D3DRS_CCW_STENCILZFAIL, m_desc.stencilBackDepthFailOp);
			m_pDevice->SetRenderState(D3DRS_CCW_STENCILPASS, m_desc.stencilBackPassOp);
			m_pDevice->SetRenderState(D3DRS_CCW_STENCILFUNC, m_desc.stencilBackFunc);
		}
		else
		{
			m_pDevice->SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, FALSE);
		}
	}

	DX_CALL(m_pDevice->EndStateBlock(&m_pStateBlock));
}
