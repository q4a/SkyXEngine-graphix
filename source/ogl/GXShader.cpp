#include "GXShader.h"

void CGXVertexShader::Release()
{
	--m_uRefCount;
	if (!m_uRefCount)
	{
		delete this;
	}
	//m_pRender->destroyVertexShader(this);
}

CGXVertexShader::~CGXVertexShader()
{
	//mem_release(m_pShaderBlob);
	//mem_release(m_pShader);
}

void CGXVertexShader::getData(void* _pData, UINT* pSize)
{
	/*byte* pData = (byte*)_pData;
	if (pData)
	{
		*pSize = (UINT)m_pShaderBlob->GetBufferSize();
		memcpy(_pData, m_pShaderBlob->GetBufferPointer(), *pSize);
	}
	else
	{
		*pSize = (UINT)m_pShaderBlob->GetBufferSize();
	}*/
}

//##########################################################################

void CGXPixelShader::Release()
{
	--m_uRefCount;
	if (!m_uRefCount)
	{
		delete this;
	}
	//m_pRender->destroyPixelShader(this);
}

CGXPixelShader::~CGXPixelShader()
{
	//mem_release(m_pShaderBlob);
	//mem_release(m_pShader);
}

void CGXPixelShader::getData(void* _pData, UINT* pSize)
{
	/*byte* pData = (byte*)_pData;
	if (pData)
	{
		*pSize = (UINT)m_pShaderBlob->GetBufferSize();
		memcpy(_pData, m_pShaderBlob->GetBufferPointer(), *pSize);
	}
	else
	{
		*pSize = (UINT)m_pShaderBlob->GetBufferSize();
	}*/
}

//##########################################################################

void CGXGeometryShader::Release()
{
	--m_uRefCount;
	if (!m_uRefCount)
	{
		delete this;
	}
}

CGXGeometryShader::~CGXGeometryShader()
{
	//mem_release(m_pShaderBlob);
	//mem_release(m_pShader);
}

void CGXGeometryShader::getData(void* _pData, UINT* pSize)
{
	/*byte* pData = (byte*)_pData;
	if (pData)
	{
		*pSize = (UINT)m_pShaderBlob->GetBufferSize();
		memcpy(_pData, m_pShaderBlob->GetBufferPointer(), *pSize);
	}
	else
	{
		*pSize = (UINT)m_pShaderBlob->GetBufferSize();
	}*/
}

//##########################################################################

void CGXComputeShader::Release()
{
	--m_uRefCount;
	if (!m_uRefCount)
	{
		delete this;
	}
}

CGXComputeShader::~CGXComputeShader()
{
	//mem_release(m_pShaderBlob);
	//mem_release(m_pShader);
}

void CGXComputeShader::getData(void* _pData, UINT* pSize)
{
	/*byte* pData = (byte*)_pData;
	if (pData)
	{
		*pSize = (UINT)m_pShaderBlob->GetBufferSize();
		memcpy(_pData, m_pShaderBlob->GetBufferPointer(), *pSize);
	}
	else
	{
		*pSize = (UINT)m_pShaderBlob->GetBufferSize();
	}*/
}
