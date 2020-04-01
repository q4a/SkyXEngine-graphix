#ifndef _IDSGshaderOgl_H_
#define _IDSGshaderOgl_H_

#include "GXContext.h"

class CGXVertexShader: public IGXVertexShader
{
	friend class CGXContext;
	friend class CGXDevice;

	CGXVertexShader(CGXDevice *pRender):
		m_pRender(pRender)
	{
	}
	~CGXVertexShader();

	CGXDevice *m_pRender;
	GLuint m_pShader;
	ID3DBlob *m_pShaderBlob = NULL;

public:
	void Release() override;

	void getData(void* pData, UINT* pSize) override;
};

class CGXGeometryShader : public IGXGeometryShader
{
	friend class CGXContext;
	friend class CGXDevice;

	CGXGeometryShader(CGXDevice* pRender) :
		m_pRender(pRender)
	{}
	~CGXGeometryShader();

	CGXDevice* m_pRender;
	GLuint m_pShader;
	ID3DBlob* m_pShaderBlob = NULL;

public:
	void Release() override;
	void getData(void* pData, UINT* pSize) override;
};

class CGXPixelShader: public IGXPixelShader
{
	friend class CGXContext;
	friend class CGXDevice;

	CGXPixelShader(CGXDevice *pRender):
		m_pRender(pRender)
	{
	}
	~CGXPixelShader();

	CGXDevice *m_pRender;
	GLuint m_pShader;

public:
	void Release() override;
	void getData(void* pData, UINT* pSize) override;
};

class CGXComputeShader : public IGXComputeShader
{
	friend class CGXContext;
	friend class CGXDevice;

	CGXComputeShader(CGXDevice* pRender) :
		m_pRender(pRender)
	{
	}
	~CGXComputeShader();

	CGXDevice *m_pRender;
	GLuint m_pShader;
	ID3DBlob* m_pShaderBlob = NULL;

public:
	void Release() override;
	void getData(void* pData, UINT* pSize) override;
};


class CGXShader : public IGXShaderSet
{
	friend class CGXContext;
	friend class CGXDevice;

	CGXShader(CGXDevice* pRender, IGXVertexShader* pVS = NULL, IGXPixelShader* pPS = NULL, IGXGeometryShader* pGS = NULL, IGXComputeShader* pCS = NULL) :
		m_pRender(pRender),
		m_pVShader((CGXVertexShader*)pVS),
		m_pPShader((CGXPixelShader*)pPS),
		m_pGShader((CGXGeometryShader*)pGS),
		m_pCShader((CGXComputeShader*)pCS)
	{
		if (pVS)
		{
			pVS->AddRef();
		}
		if (pPS)
		{
			pPS->AddRef();
		}
		if (pGS)
		{
			pGS->AddRef();
		}
		if (pCS)
		{
			pCS->AddRef();
		}
	}
	~CGXShader()
	{
		mem_release(m_pVShader);
		mem_release(m_pPShader);
		mem_release(m_pGShader);
		mem_release(m_pCShader);
	}

	CGXDevice* m_pRender;
	CGXVertexShader* m_pVShader;
	CGXPixelShader* m_pPShader;
	CGXGeometryShader* m_pGShader;
	CGXComputeShader* m_pCShader;
public:
	void Release() override
	{
		--m_uRefCount;
		if (!m_uRefCount)
		{
			delete this;
		}
	}

	IGXPixelShader* getPixelShader()
	{
		if (m_pPShader)
		{
			m_pPShader->AddRef();
		}
		return(m_pPShader);
	}
	IGXGeometryShader* getGeometryShader()
	{
		if (m_pGShader)
		{
			m_pGShader->AddRef();
		}
		return(m_pGShader);
	}
	IGXVertexShader* getVertexShader()
	{
		if (m_pVShader)
		{
			m_pVShader->AddRef();
		}
		return(m_pVShader);
	}
	IGXComputeShader* getComputeShader()
	{
		if (m_pCShader)
		{
			m_pCShader->AddRef();
		}
		return(m_pCShader);
	}
};

#endif
