#include "GXShader.h"

void CGXVertexShader::Release()
{
	--m_uRefCount;
	if(!m_uRefCount)
	{
		delete this;
	}
}

CGXVertexShader::~CGXVertexShader()
{
	mem_release(m_pShaderBlob);
	mem_release(m_pShader);
}

void CGXVertexShader::setConstantF(UINT uStartRegister, const float *pConstantData, UINT uVector4fCount)
{
	assert(!"Not implemented!");
}

void CGXVertexShader::setConstantI(UINT uStartRegister, const int *pConstantData, UINT uVector4iCount)
{
	assert(!"Not implemented!");
}

UINT CGXVertexShader::getConstantCount()
{
	assert(!"Not implemented!");
	return(0);
}

UINT CGXVertexShader::getConstantLocation(const char *szConstName)
{
	assert(!"Not implemented!");
	return(GX_SHADER_CONSTANT_FAIL);
}

UINT CGXVertexShader::getConstantSizeV4(const char *szConstName)
{
	assert(!"Not implemented!");
	return(GX_SHADER_CONSTANT_FAIL);
}

void CGXVertexShader::getData(void *_pData, UINT *pSize)
{
	byte *pData = (byte*)_pData;
	if(pData)
	{
		*pSize = (UINT)m_pShaderBlob->GetBufferSize();
		memcpy(_pData, m_pShaderBlob->GetBufferPointer(), *pSize);
	}
	else
	{
		*pSize = (UINT)m_pShaderBlob->GetBufferSize();
	}
}

//##########################################################################

void CGXPixelShader::Release()
{
	--m_uRefCount;
	if(!m_uRefCount)
	{
		delete this;
	}
}

CGXPixelShader::~CGXPixelShader()
{
	mem_release(m_pShaderBlob);
	mem_release(m_pShader);
}

void CGXPixelShader::setConstantF(UINT uStartRegister, const float *pConstantData, UINT uVector4fCount)
{
	assert(!"Not implemented!");
}

void CGXPixelShader::setConstantI(UINT uStartRegister, const int *pConstantData, UINT uVector4iCount)
{
	assert(!"Not implemented!");
}

UINT CGXPixelShader::getConstantCount()
{
	assert(!"Not implemented!");
	return(0);
}

UINT CGXPixelShader::getConstantLocation(const char *szConstName)
{
	assert(!"Not implemented!");
	return(GX_SHADER_CONSTANT_FAIL);
}

UINT CGXPixelShader::getConstantSizeV4(const char *szConstName)
{
	assert(!"Not implemented!");
	return(GX_SHADER_CONSTANT_FAIL);
}

void CGXPixelShader::getData(void *_pData, UINT *pSize)
{
	byte *pData = (byte*)_pData;
	if(pData)
	{
		*pSize = (UINT)m_pShaderBlob->GetBufferSize();
		memcpy(_pData, m_pShaderBlob->GetBufferPointer(), *pSize);
	}
	else
	{
		*pSize = (UINT)m_pShaderBlob->GetBufferSize();
	}
}

//##########################################################################

void CGXGeometryShader::Release()
{
	--m_uRefCount;
	if(!m_uRefCount)
	{
		delete this;
	}
}

CGXGeometryShader::~CGXGeometryShader()
{
	mem_release(m_pShaderBlob);
	mem_release(m_pShader);
}

void CGXGeometryShader::getData(void *_pData, UINT *pSize)
{
	byte *pData = (byte*)_pData;
	if(pData)
	{
		*pSize = (UINT)m_pShaderBlob->GetBufferSize();
		memcpy(_pData, m_pShaderBlob->GetBufferPointer(), *pSize);
	}
	else
	{
		*pSize = (UINT)m_pShaderBlob->GetBufferSize();
	}
}

//##########################################################################

void CGXComputeShader::Release()
{
	--m_uRefCount;
	if(!m_uRefCount)
	{
		delete this;
	}
}

CGXComputeShader::~CGXComputeShader()
{
	mem_release(m_pShaderBlob);
	mem_release(m_pShader);
}

void CGXComputeShader::getData(void *_pData, UINT *pSize)
{
	byte *pData = (byte*)_pData;
	if(pData)
	{
		*pSize = (UINT)m_pShaderBlob->GetBufferSize();
		memcpy(_pData, m_pShaderBlob->GetBufferPointer(), *pSize);
	}
	else
	{
		*pSize = (UINT)m_pShaderBlob->GetBufferSize();
	}
}
