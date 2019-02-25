#include "GXVertexBuffer.h"

void CGXVertexBuffer::Release()
{
	--m_uRefCount;
	if(!m_uRefCount)
	{
		m_pRender->destroyVertexBuffer(this);
	}
}

bool CGXVertexBuffer::lock(void **ppData, GXBUFFERLOCK mode)
{
	assert(mode == GXBL_WRITE);
	assert(m_isLockable || m_wasReset);

	if(!(m_isLockable || m_wasReset) || mode != GXBL_WRITE)
	{
		return(false);
	}

	if(!FAILED(m_pBuffer->Lock(0, m_uSize, ppData, mode == GXBL_WRITE ? m_uLockFlagsWrite : D3DLOCK_READONLY)))
	{
		m_wasReset = false;
		return(true);
	}
	return(false);
}

void CGXVertexBuffer::unlock()
{
	m_pBuffer->Unlock();
}

CGXVertexBuffer::~CGXVertexBuffer()
{
	mem_release(m_pBuffer);
}

bool CGXVertexBuffer::wasReset()
{
	return(m_wasReset);
}
