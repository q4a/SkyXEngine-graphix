#include "GXRasterizerState.h"

void CGXRasterizerState::Release()
{
	--m_uRefCount;
	if(!m_uRefCount)
	{
		m_pRender->destroyRasterizerState(this);
	}
}

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
	
	DX_CALL(m_pDevice->BeginStateBlock());

	switch(m_desc.fillMode)
	{
	case GXFILL_SOLID:
		m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		break;
	case GXFILL_WIREFRAME:
		m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		break;
	}

	switch(m_desc.cullMode)
	{
	case GXCULL_NONE:
		m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		break;
	case GXCULL_FRONT:
		m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
		break;
	case GXCULL_BACK:
		m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		break;
	}

	//m_pDevice->SetRenderState(D3DRS_DEPTHBIAS, pRSDesc->iDepthBias); // ?
	m_pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, m_desc.bScissorEnable);

	DX_CALL(m_pDevice->EndStateBlock(&m_pStateBlock));
}

