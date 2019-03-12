#ifndef _IDSGshaderOgl_H_
#define _IDSGshaderOgl_H_

#include "GXContext.h"

class CGXVertexShader: public IGXVertexShader
{
	friend class CGXContext;

	CGXVertexShader(CGXContext * pRender):m_pRender(pRender)
	{
	}
	~CGXVertexShader();

	CGXContext * m_pRender;
	ID3D11VertexShader *m_pShader = NULL;
	ID3DBlob *m_pShaderBlob = NULL;

public:
	void Release();

	void setConstantF(UINT uStartRegister, const float *pConstantData, UINT uVector4fCount);
	void setConstantI(UINT uStartRegister, const int *pConstantData, UINT uVector4iCount);
	UINT getConstantCount();
	UINT getConstantLocation(const char *szConstName);
	UINT getConstantSizeV4(const char *szConstName);

	void getData(void *pData, UINT *pSize);
};

class CGXGeometryShader: public IGXGeometryShader
{
	friend class CGXContext;

	CGXGeometryShader(CGXContext * pRender):m_pRender(pRender)
	{}
	~CGXGeometryShader();

	CGXContext * m_pRender;
	ID3D11GeometryShader *m_pShader = NULL;
	ID3DBlob *m_pShaderBlob = NULL;

public:
	void Release();
	void getData(void *pData, UINT *pSize);
};

class CGXPixelShader: public IGXPixelShader
{
	friend class CGXContext;

	CGXPixelShader(CGXContext * pRender):m_pRender(pRender)
	{
	}
	~CGXPixelShader();

	CGXContext * m_pRender;
	ID3D11PixelShader *m_pShader = NULL;
	ID3DBlob *m_pShaderBlob = NULL;

public:
	void Release();

	void setConstantF(UINT uStartRegister, const float *pConstantData, UINT uVector4fCount);
	void setConstantI(UINT uStartRegister, const int *pConstantData, UINT uVector4iCount);
	UINT getConstantCount();
	UINT getConstantLocation(const char *szConstName);
	UINT getConstantSizeV4(const char *szConstName);
	void getData(void *pData, UINT *pSize);
};


class CGXShader: public IGXShader
{
	friend class CGXContext;

	CGXShader(CGXContext * pRender, IGXVertexShader *pVS = NULL, IGXPixelShader *pPS = NULL, IGXGeometryShader *pGS = NULL):
		m_pRender(pRender),
		m_pVShader(pVS),
		m_pPShader(pPS),
		m_pGShader(pGS)
	{
		if(pVS)
		{
			pVS->AddRef();
		}
		if(pPS)
		{
			pPS->AddRef();
		}
	}

	CGXContext * m_pRender;
	IGXVertexShader *m_pVShader;
	IGXPixelShader *m_pPShader;
	IGXGeometryShader *m_pGShader;
public:
	void Release()
	{
		--m_uRefCount;
		if(!m_uRefCount)
		{
			mem_release(m_pVShader);
			mem_release(m_pPShader);
			mem_release(m_pGShader);
			m_pRender->destroyShader(this);
		}
	}

	IGXPixelShader *getPixelShader()
	{
		if(m_pPShader)
		{
			m_pPShader->AddRef();
		}
		return(m_pPShader);
	}
	IGXGeometryShader *getGeometryShader()
	{
		if(m_pGShader)
		{
			m_pGShader->AddRef();
		}
		return(m_pGShader);
	}
	IGXVertexShader *getVertexShader()
	{
		if(m_pVShader)
		{
			m_pVShader->AddRef();
		}
		return(m_pVShader);
	}

	void setPixelShader(IGXPixelShader *pShader)
	{
		mem_release(m_pPShader);
		m_pPShader = pShader;
		if(pShader)
		{
			pShader->AddRef();
		}
	}
	void setGeometryShader(IGXGeometryShader *pShader)
	{
		mem_release(m_pGShader);
		m_pGShader = pShader;
		if(pShader)
		{
			pShader->AddRef();
		}
	}
	void setVertexShader(IGXVertexShader *pShader)
	{
		mem_release(m_pVShader);
		m_pVShader = pShader;
		if(pShader)
		{
			pShader->AddRef();
		}
	}
};

#endif
