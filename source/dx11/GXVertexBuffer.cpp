#include "GXVertexBuffer.h"
#include "GXContext.h"

void CGXVertexBuffer::Release()
{
	--m_uRefCount;
	if(!m_uRefCount)
	{
		delete this;
	}
}

bool CGXVertexBuffer::lock(void **ppData, GXBUFFERLOCK mode)
{
	assert(mode == GXBL_WRITE);
	assert(m_isLockable);

	if(!m_isLockable || mode != GXBL_WRITE)
	{
		return(false);
	}

	D3D11_MAPPED_SUBRESOURCE srs;

	if(!FAILED(DX_CALL(m_pRender->getDXDeviceContext()->Map(m_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &srs))))
	{
		m_wasReset = false;
		((CGXContext*)m_pRender->getDirectContext())->addBytesVertices(m_uSize);
		*ppData = srs.pData;
		return(true);
	}
	return(false);
}

void CGXVertexBuffer::unlock()
{
	m_pRender->getDXDeviceContext()->Unmap(m_pBuffer, 0);
}

CGXVertexBuffer::~CGXVertexBuffer()
{
	mem_release(m_pBuffer);

	m_pRender->destroyVertexBuffer(this);
}

bool CGXVertexBuffer::wasReset()
{
	return(m_wasReset);
}
