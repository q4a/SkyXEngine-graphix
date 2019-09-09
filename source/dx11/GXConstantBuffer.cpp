#include "GXConstantBuffer.h"
#include "GXContext.h"

CGXConstantBuffer::CGXConstantBuffer(CGXDevice *pRender, UINT uSize):
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
	m_pRender->addBytesShaderConst((int)m_uSize);
}
CGXConstantBuffer::~CGXConstantBuffer()
{
	mem_release(m_pBuffer);
	m_pRender->addBytesShaderConst(-(int)m_uSize);
}

UINT CGXConstantBuffer::getSize()
{
	return(m_uSize);
}

void CGXConstantBuffer::update(const void *pData)
{
	CGXContext *pCtx = (CGXContext*)m_pRender->getThreadContext();

	pCtx->getDXContext()->UpdateSubresource(m_pBuffer, 0, NULL, pData, 0, 0);
	pCtx->addBytesShaderConst(m_uSize);
}
