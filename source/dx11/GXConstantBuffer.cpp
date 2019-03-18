#include "GXConstantBuffer.h"

void CGXConstantBuffer::Release()
{
	--m_uRefCount;
	if(!m_uRefCount)
	{
		m_pRender->addBytesShaderConst(-m_uSize, true);
		delete this;
	}
}

CGXConstantBuffer::CGXConstantBuffer(CGXContext * pRender, UINT uSize):
	m_pRender(pRender)
{
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = uSize;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	m_uSize = uSize;
	
	DX_CALL(m_pRender->getDXDevice()->CreateBuffer(&bufferDesc, NULL, &m_pBuffer));
	m_pRender->addBytesShaderConst(m_uSize, true);
}
CGXConstantBuffer::~CGXConstantBuffer()
{
	mem_release(m_pBuffer);
}

UINT CGXConstantBuffer::getSize()
{
	return(m_uSize);
}

void CGXConstantBuffer::update(const void *pData)
{
	m_pRender->getDXDeviceContext()->UpdateSubresource(m_pBuffer, 0, NULL, pData, 0, 0);
	m_pRender->addBytesShaderConst(m_uSize);
}
