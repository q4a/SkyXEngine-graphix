#include "GXSamplerState.h"

void CGXSamplerState::onDevRst()
{
	auto m_pDevice = m_pRender->getDXDevice();

	D3D11_SAMPLER_DESC sampDesc;
	
	sampDesc.AddressU = (D3D11_TEXTURE_ADDRESS_MODE)(m_desc.addressU + 1);
	sampDesc.AddressV = (D3D11_TEXTURE_ADDRESS_MODE)(m_desc.addressV + 1);
	sampDesc.AddressW = (D3D11_TEXTURE_ADDRESS_MODE)(m_desc.addressW + 1);
	sampDesc.MipLODBias = m_desc.fMipLODBias;
	sampDesc.MaxAnisotropy = m_desc.uMaxAnisotropy;
	sampDesc.ComparisonFunc = (D3D11_COMPARISON_FUNC)m_desc.comparisonFunc;
	sampDesc.BorderColor[0] = m_desc.f4BorderColor.x;
	sampDesc.BorderColor[1] = m_desc.f4BorderColor.y;
	sampDesc.BorderColor[2] = m_desc.f4BorderColor.z;
	sampDesc.BorderColor[3] = m_desc.f4BorderColor.w;
	sampDesc.MinLOD = m_desc.fMinLOD;
	sampDesc.MaxLOD = m_desc.fMaxLOD;

	switch(m_desc.filter)
	{
	case GXFILTER_MIN_MAG_MIP_POINT:
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		break;
	case GXFILTER_MIN_MAG_POINT_MIP_LINEAR:
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
		break;
	case GXFILTER_MIN_POINT_MAG_LINEAR_MIP_POINT:
		sampDesc.Filter = D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
		break;
	case GXFILTER_MIN_POINT_MAG_MIP_LINEAR:
		sampDesc.Filter = D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
		break;
	case GXFILTER_MIN_LINEAR_MAG_MIP_POINT:
		sampDesc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		break;
	case GXFILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
		sampDesc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		break;
	case GXFILTER_MIN_MAG_LINEAR_MIP_POINT:
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		break;
	case GXFILTER_MIN_MAG_MIP_LINEAR:
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		break;
	case GXFILTER_ANISOTROPIC:
		sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		break;


	case GXFILTER_COMPARISON_MIN_MAG_MIP_POINT:
		sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
		break;
	case GXFILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR:
		sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
		break;
	case GXFILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:
		sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
		break;
	case GXFILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR:
		sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
		break;
	case GXFILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT:
		sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
		break;
	case GXFILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
		sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		break;
	case GXFILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT:
		sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		break;
	case GXFILTER_COMPARISON_MIN_MAG_MIP_LINEAR:
		sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		break;
	case GXFILTER_COMPARISON_ANISOTROPIC:
		sampDesc.Filter = D3D11_FILTER_COMPARISON_ANISOTROPIC;
		break;
	}

	DX_CALL(m_pDevice->CreateSamplerState(&sampDesc, &m_pStateBlock));
}
