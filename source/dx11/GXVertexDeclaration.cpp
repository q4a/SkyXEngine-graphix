#include "GXVertexDeclaration.h"
#include <cstdio>

CGXVertexDeclaration::CGXVertexDeclaration(ID3D11Device *pDevice, CGXDevice *pRender, const GXVertexElement *pDecl):
	m_pRender(pRender)
{
	UINT uDeclCount = 0;
	while(pDecl[uDeclCount++].type != GXDECLTYPE_UNUSED);
	--uDeclCount;

	for(UINT i = 0; i < GX_MAX_VSTREAM; ++i)
	{
		m_u8SpecSpec[i] = (GXDECLSPEC)-1;
	}

	for(UINT i = 0; i < uDeclCount; ++i)
	{
		if(pDecl[i].u8Stream >= GX_MAX_VSTREAM)
		{
			CGXDevice::debugMessage(GX_LOG_ERROR, "Unable to create vertex declaration: Stream >= GX_MAX_VSTREAM!");
			return;
		}
		if(m_u8SpecSpec[pDecl[i].u8Stream] == (GXDECLSPEC)-1)
		{
			m_u8SpecSpec[pDecl[i].u8Stream] = pDecl[i].spec;
		}
		else if(m_u8SpecSpec[pDecl[i].u8Stream] != pDecl[i].spec)
		{
			CGXDevice::debugMessage(GX_LOG_ERROR, "Unable to create vertex declaration: vertex instance specs in a stream must be the same!");
			return;
		}
	}

	D3D11_INPUT_ELEMENT_DESC *pEls = (D3D11_INPUT_ELEMENT_DESC*)alloca(sizeof(D3D11_INPUT_ELEMENT_DESC) * uDeclCount);
	memset(m_u8StreamStride, 0, sizeof(m_u8StreamStride));
	char *szStubShaderCode = (char*)alloca(40 + uDeclCount * 32);
	char *tmp = szStubShaderCode;
	tmp += sprintf(tmp, "struct VS{");
	UINT stride = 0;
	m_u8StreamCount = 0;
	for(UINT i = 0; i < uDeclCount; ++i)
	{
		pEls[i].InputSlot = pDecl[i].u8Stream;
		pEls[i].AlignedByteOffset = pDecl[i].u16Offset;
		pEls[i].SemanticIndex = 0;

		if(pDecl[i].u8Stream + 1 > m_u8StreamCount)
		{
			m_u8StreamCount = pDecl[i].u8Stream + 1;
		}

		switch(pDecl[i].spec)
		{
		case GXDECLSPEC_PER_VERTEX_DATA:
			pEls[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			pEls[i].InstanceDataStepRate = 0;
			break;
		case GXDECLSPEC_PER_INSTANCE_DATA:
			pEls[i].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
			pEls[i].InstanceDataStepRate = 1;
			break;
		}

		const char *szShaderType = NULL;
		UINT uSize = 0;
		switch(pDecl[i].type)
		{
		case GXDECLTYPE_FLOAT1:
			pEls[i].Format = DXGI_FORMAT_R32_FLOAT; uSize = 4; szShaderType = "float"; break;
		case GXDECLTYPE_FLOAT2:
			pEls[i].Format = DXGI_FORMAT_R32G32_FLOAT; uSize = 8; szShaderType = "float2"; break;
		case GXDECLTYPE_FLOAT3:
			pEls[i].Format = DXGI_FORMAT_R32G32B32_FLOAT; uSize = 12; szShaderType = "float3"; break;
		case GXDECLTYPE_FLOAT4:
			pEls[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT; uSize = 16; szShaderType = "float4"; break;

		//case GXDECLTYPE_GXCOLOR:
		//	pEls[i].Format = D3DDECLTYPE_D3DCOLOR; uSize = 4; break;


		case GXDECLTYPE_UBYTE4:
			pEls[i].Format = DXGI_FORMAT_R8G8B8A8_UINT; uSize = 4; szShaderType = "uint4"; break;

		case GXDECLTYPE_SHORT2:
			pEls[i].Format = DXGI_FORMAT_R16G16_SINT; uSize = 4; szShaderType = "int2"; break;
		case GXDECLTYPE_SHORT4:
			pEls[i].Format = DXGI_FORMAT_R16G16B16A16_SINT; uSize = 8; szShaderType = "int4"; break;

		case GXDECLTYPE_UBYTE4N:
			pEls[i].Format = DXGI_FORMAT_R8G8B8A8_UNORM; uSize = 4; szShaderType = "uint4"; break;

		case GXDECLTYPE_SHORT2N:
			pEls[i].Format = DXGI_FORMAT_R16G16_SNORM; uSize = 4; szShaderType = "int2"; break;
		case GXDECLTYPE_SHORT4N:
			pEls[i].Format = DXGI_FORMAT_R16G16B16A16_SNORM; uSize = 8; szShaderType = "int4"; break;

		case GXDECLTYPE_USHORT2N:
			pEls[i].Format = DXGI_FORMAT_R16G16_UNORM; uSize = 4; szShaderType = "uint2"; break;
		case GXDECLTYPE_USHORT4N:
			pEls[i].Format = DXGI_FORMAT_R16G16B16A16_UNORM; uSize = 8; szShaderType = "uint4"; break;

		/*case GXDECLTYPE_UDEC3:
			pEls[i].Format = D3DDECLTYPE_UDEC3; uSize = 32; break;

		case GXDECLTYPE_DEC3N:
			pEls[i].Format = D3DDECLTYPE_DEC3N; uSize = 32; break;*/

		case GXDECLTYPE_FLOAT16_2:
			pEls[i].Format = DXGI_FORMAT_R16G16_FLOAT; uSize = 4; szShaderType = "float2"; break;

		case GXDECLTYPE_FLOAT16_4:
			pEls[i].Format = DXGI_FORMAT_R16G16B16A16_FLOAT; uSize = 8; szShaderType = "float4"; break;
		}

		if(pEls[i].AlignedByteOffset + uSize > m_u8StreamStride[pDecl[i].u8Stream])
		{
			m_u8StreamStride[pDecl[i].u8Stream] = pEls[i].AlignedByteOffset + uSize;
		}

		switch(pDecl[i].usage)
		{
		case GXDECLUSAGE_POSITION:
			pEls[i].SemanticName = "POSITION"; break;
		case GXDECLUSAGE_TEXCOORD:
			pEls[i].SemanticName = "TEXCOORD"; break;
		case GXDECLUSAGE_NORMAL:
			pEls[i].SemanticName = "NORMAL"; break;
		case GXDECLUSAGE_BLENDWEIGHT:
			pEls[i].SemanticName = "BLENDWEIGHT"; break;
		case GXDECLUSAGE_BLENDINDICES:
			pEls[i].SemanticName = "BLENDINDICES"; break;
		case GXDECLUSAGE_COLOR:
			pEls[i].SemanticName = "COLOR"; break;
		case GXDECLUSAGE_TANGENT:
			pEls[i].SemanticName = "TANGENT"; break;
		case GXDECLUSAGE_BINORMAL:
			pEls[i].SemanticName = "BINORMAL"; break;
		case GXDECLUSAGE_TESSFACTOR:
			pEls[i].SemanticName = "TESSFACTOR"; break;
		case GXDECLUSAGE_TEXCOORD1:
			pEls[i].SemanticName = "TEXCOORD";
			pEls[i].SemanticIndex = 1;
			break;
		case GXDECLUSAGE_TEXCOORD2:
			pEls[i].SemanticName = "TEXCOORD";
			pEls[i].SemanticIndex = 2;
			break;
		case GXDECLUSAGE_TEXCOORD3:
			pEls[i].SemanticName = "TEXCOORD";
			pEls[i].SemanticIndex = 3;
			break;
		case GXDECLUSAGE_TEXCOORD4:
			pEls[i].SemanticName = "TEXCOORD";
			pEls[i].SemanticIndex = 4;
			break;
		case GXDECLUSAGE_TEXCOORD5:
			pEls[i].SemanticName = "TEXCOORD";
			pEls[i].SemanticIndex = 5;
			break;
		case GXDECLUSAGE_TEXCOORD6:
			pEls[i].SemanticName = "TEXCOORD";
			pEls[i].SemanticIndex = 6;
			break;
		case GXDECLUSAGE_TEXCOORD7:
			pEls[i].SemanticName = "TEXCOORD";
			pEls[i].SemanticIndex = 7;
			break;
		}

		tmp += sprintf(tmp, "%s a%d:%s%d;", szShaderType, i, pEls[i].SemanticName, pEls[i].SemanticIndex);
	}
	tmp += sprintf(tmp, "};VS main(VS IN){return(IN);}");

	ID3DBlob *pShaderBlob = NULL;
	ID3DBlob *pErrorBlob = NULL;
	if(FAILED(DX_CALL(D3DX11CompileFromMemory(szStubShaderCode, strlen(szStubShaderCode), "memory.vs", NULL, NULL, "main", "vs_4_0", D3DCOMPILE_SKIP_OPTIMIZATION, 0, NULL, &pShaderBlob, &pErrorBlob, NULL))))
	{
		if(pErrorBlob)
		{
			CGXDevice::debugMessage(GX_LOG_ERROR, (char*)pErrorBlob->GetBufferPointer());
		}
	}
	mem_release(pErrorBlob);
	DX_CALL(pDevice->CreateInputLayout(pEls, uDeclCount, pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), &m_pDeclaration));
	mem_release(pShaderBlob);
};

CGXVertexDeclaration::~CGXVertexDeclaration()
{
	mem_release(m_pDeclaration);
}
