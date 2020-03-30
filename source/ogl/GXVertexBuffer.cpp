#include "GXVertexBuffer.h"

void CGXVertexBuffer::Release()
{
	m_pRender->destroyVertexBuffer(this);
}

bool CGXVertexBuffer::lock(void** ppData, GXBUFFERLOCK mode)
{
	return(false);
}

void CGXVertexBuffer::unlock()
{
}

bool CGXVertexBuffer::wasReset()
{
	return(m_wasReset);
}
