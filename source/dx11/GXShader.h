#ifndef _IDSGshaderOgl_H_
#define _IDSGshaderOgl_H_

#include "GXContext.h"

class CGXVertexShader: public IGXVertexShader
{
	friend class CGXContext;

	CGXVertexShader(CGXContext *pRender):
		m_pRender(pRender)
	{
	}
	~CGXVertexShader();

	CGXContext *m_pRender;
	ID3D11VertexShader *m_pShader = NULL;
	ID3DBlob *m_pShaderBlob = NULL;

public:
	void Release() override;

	void setConstantF(UINT uStartRegister, const float *pConstantData, UINT uVector4fCount) override;
	void setConstantI(UINT uStartRegister, const int *pConstantData, UINT uVector4iCount) override;
	UINT getConstantCount() override;
	UINT getConstantLocation(const char *szConstName) override;
	UINT getConstantSizeV4(const char *szConstName) override;

	void getData(void *pData, UINT *pSize) override;
};

class CGXGeometryShader: public IGXGeometryShader
{
	friend class CGXContext;

	CGXGeometryShader(CGXContext *pRender):
		m_pRender(pRender)
	{}
	~CGXGeometryShader();

	CGXContext *m_pRender;
	ID3D11GeometryShader *m_pShader = NULL;
	ID3DBlob *m_pShaderBlob = NULL;

public:
	void Release() override;
	void getData(void *pData, UINT *pSize) override;
};

class CGXPixelShader: public IGXPixelShader
{
	friend class CGXContext;

	CGXPixelShader(CGXContext *pRender):
		m_pRender(pRender)
	{
	}
	~CGXPixelShader();

	CGXContext *m_pRender;
	ID3D11PixelShader *m_pShader = NULL;
	ID3DBlob *m_pShaderBlob = NULL;

public:
	void Release() override;

	void setConstantF(UINT uStartRegister, const float *pConstantData, UINT uVector4fCount) override;
	void setConstantI(UINT uStartRegister, const int *pConstantData, UINT uVector4iCount) override;
	UINT getConstantCount() override;
	UINT getConstantLocation(const char *szConstName) override;
	UINT getConstantSizeV4(const char *szConstName) override;
	void getData(void *pData, UINT *pSize) override;
};

class CGXComputeShader: public IGXComputeShader
{
	friend class CGXContext;

	CGXComputeShader(CGXContext *pRender):
		m_pRender(pRender)
	{
	}
	~CGXComputeShader();

	CGXContext *m_pRender;
	ID3D11ComputeShader *m_pShader = NULL;
	ID3DBlob *m_pShaderBlob = NULL;

public:
	void Release() override;
	void getData(void *pData, UINT *pSize) override;
};


class CGXShader: public IGXShaderSet
{
	friend class CGXContext;

	CGXShader(CGXContext *pRender, IGXVertexShader *pVS = NULL, IGXPixelShader *pPS = NULL, IGXGeometryShader *pGS = NULL, IGXComputeShader *pCS = NULL):
		m_pRender(pRender),
		m_pVShader((CGXVertexShader*)pVS),
		m_pPShader((CGXPixelShader*)pPS),
		m_pGShader((CGXGeometryShader*)pGS),
		m_pCShader((CGXComputeShader*)pCS)
	{
		if(pVS)
		{
			pVS->AddRef();
		}
		if(pPS)
		{
			pPS->AddRef();
		}
		if(pGS)
		{
			pGS->AddRef();
		}
		if(pCS)
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

	CGXContext *m_pRender;
	CGXVertexShader *m_pVShader;
	CGXPixelShader *m_pPShader;
	CGXGeometryShader *m_pGShader;
	CGXComputeShader *m_pCShader;
public:
	void Release() override
	{
		--m_uRefCount;
		if(!m_uRefCount)
		{
			delete this;
		}
	}

	IGXPixelShader* getPixelShader()
	{
		if(m_pPShader)
		{
			m_pPShader->AddRef();
		}
		return(m_pPShader);
	}
	IGXGeometryShader* getGeometryShader()
	{
		if(m_pGShader)
		{
			m_pGShader->AddRef();
		}
		return(m_pGShader);
	}
	IGXVertexShader* getVertexShader()
	{
		if(m_pVShader)
		{
			m_pVShader->AddRef();
		}
		return(m_pVShader);
	}
	IGXComputeShader* getComputeShader()
	{
		if(m_pCShader)
		{
			m_pCShader->AddRef();
		}
		return(m_pCShader);
	}
};

#endif
