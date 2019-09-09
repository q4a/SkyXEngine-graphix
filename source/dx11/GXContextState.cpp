#include "GXContextState.h"


CGXContextState::CGXContextState()
{
}

CGXContextState::~CGXContextState()
{
	mem_release(m_pCurIndexBuffer);
	mem_release(m_pCurRenderBuffer);

	for(UINT i = 0; i < GX_MAX_SAMPLERS; ++i)
	{
		mem_release(m_pSamplerState[i]);
	}

	mem_release(m_pRasterizerState);
	mem_release(m_pDepthStencilState);
	mem_release(m_pBlendState);
	mem_release(m_pDepthStencilSurface);

	mem_release(m_pShader);

	for(UINT i = 0; i < GX_MAX_COLORTARGETS; ++i)
	{
		mem_release(m_pColorTarget[i]);
	}

	for(UINT i = 0; i < GX_MAX_TEXTURES; ++i)
	{
		mem_release(m_pTextures[i]);
		mem_release(m_pTexturesVS[i]);
		mem_release(m_pTexturesCS[i]);
	}

	for(UINT i = 0; i < GX_MAX_UAV_TEXTURES; ++i)
	{
		mem_release(m_pUAVsCS[i]);
	}

	for(UINT i = 0; i < GX_MAX_SHADER_CONST; ++i)
	{
		mem_release(m_pVSConstant[i]);
		mem_release(m_pPSConstant[i]);
		mem_release(m_pGSConstant[i]);
		mem_release(m_pCSConstant[i]);
	}
}
