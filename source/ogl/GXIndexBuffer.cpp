#include "GXIndexBuffer.h"

void CGXIndexBuffer::Release()
{
	m_pRender->destroyIndexBuffer(this);
}

bool CGXIndexBuffer::lock(void** ppData, GXBUFFERLOCK mode)
{
	return(false);
}

void CGXIndexBuffer::unlock()
{
}

bool CGXIndexBuffer::wasReset()
{
	return(m_wasReset);
}
