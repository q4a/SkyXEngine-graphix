#ifndef _GXCONTEXTSTATE_H_
#define _GXCONTEXTSTATE_H_

#include "../../graphix.h"

#include "GXDevice.h"

class CGXContextState: public IGXContextState
{
	friend class CGXContext;

public:
	CGXContextState();
	~CGXContextState();

protected:
	IGXRenderBuffer *m_pCurRenderBuffer = NULL;
	IGXIndexBuffer *m_pCurIndexBuffer = NULL;

	GXPRIMITIVETOPOLOGY m_gxPT = GXPT_TRIANGLELIST;

	IGXSamplerState *m_pSamplerState[GX_MAX_SAMPLERS];

	IGXRasterizerState *m_pRasterizerState = NULL;

	IGXDepthStencilState *m_pDepthStencilState = NULL;
	UINT m_uStencilRef = 0;

	IGXBlendState *m_pBlendState = NULL;
	GXCOLOR m_blendFactor = GX_COLOR_ARGB(255, 255, 255, 255);

	IGXDepthStencilSurface *m_pDepthStencilSurface = NULL;

	IGXSurface *m_pColorTarget[GX_MAX_COLORTARGETS];

	IGXBaseTexture *m_pTextures[GX_MAX_TEXTURES];
	IGXBaseTexture *m_pTexturesVS[GX_MAX_TEXTURES];
	IGXBaseTexture *m_pTexturesCS[GX_MAX_TEXTURES];
	IGXBaseTexture *m_pUAVsCS[GX_MAX_UAV_TEXTURES];

	IGXConstantBuffer *m_pVSConstant[GX_MAX_SHADER_CONST];
	IGXConstantBuffer *m_pPSConstant[GX_MAX_SHADER_CONST];
	IGXConstantBuffer *m_pGSConstant[GX_MAX_SHADER_CONST];
	IGXConstantBuffer *m_pCSConstant[GX_MAX_SHADER_CONST];

	IGXShaderSet *m_pShader = NULL;

	RECT m_rcScissors;
};

#endif
