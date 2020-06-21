#include "GXConstantBuffer.h"
#include "GXContext.h"

CGXConstantBuffer::CGXConstantBuffer(CGXDevice *pRender, UINT uSize, bool isPartialUpdateAllowed):
	m_pRender(pRender),
	m_isPartialUpdateAllowed(isPartialUpdateAllowed)
{
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = /*isPartialUpdateAllowed ? D3D11_USAGE_DYNAMIC : */D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = uSize;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = /*isPartialUpdateAllowed ? D3D11_CPU_ACCESS_WRITE : */0;
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

void CGXConstantBuffer::update(const void *pData, size_t sizeUpdate)
{
	CGXContext *pCtx = (CGXContext*)m_pRender->getThreadContext();

	assert(sizeUpdate % 16 == 0);

	// 
	if(false && sizeUpdate && sizeUpdate != m_uSize)
	{
		if(!m_isPartialUpdateAllowed)
		{
			CGXDevice::debugMessage(GX_LOG_ERROR, "Trying to partial update constant buffer that is not allowed to be partial updated!");
			return;
		}
		D3D11_MAPPED_SUBRESOURCE mappedSubResource;
		memset(&mappedSubResource, 0, sizeof(mappedSubResource));

		if(SUCCEEDED(DX_CALL(pCtx->getDXContext()->Map(m_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource))))
		{
			memcpy(mappedSubResource.pData, pData, sizeUpdate);
			pCtx->getDXContext()->Unmap(m_pBuffer, 0);
			pCtx->addBytesShaderConst(sizeUpdate);
		}
	}
	else
	{
		pCtx->getDXContext()->UpdateSubresource(m_pBuffer, 0, NULL, pData, 0, 0);
		pCtx->addBytesShaderConst(m_uSize);
	}
}
