#include "GXContext.h"

#if defined(_LINUX) || defined(_MAC)
#	include <gtk/gtk.h>
#	include <gtk/gtkx.h>
#endif

#include "GXIndexBuffer.h"
#include "GXVertexBuffer.h"
#include "GXVertexDeclaration.h"
#include "GXShader.h"
#include "GXRenderBuffer.h"

CGXContext::CGXContext(CGXContext* pDXContext, CGXDevice* pGXDevice, bool isDirect) :
	m_pDeviceContext(pDXContext),
	m_pGXDevice(pGXDevice),
	m_isDirect(isDirect)
{
	memset(&m_sync_state, 0, sizeof(m_sync_state));
	memset(&m_pSamplerState, 0, sizeof(m_pSamplerState));
	memset(&m_pColorTarget, 0, sizeof(m_pColorTarget));
	memset(&m_pDXColorTarget, 0, sizeof(m_pDXColorTarget));
	memset(&m_pTextures, 0, sizeof(m_pTextures));
	memset(&m_pTexturesVS, 0, sizeof(m_pTexturesVS));
	memset(&m_pTexturesCS, 0, sizeof(m_pTexturesCS));
	memset(&m_pUAVsCS, 0, sizeof(m_pUAVsCS));
	memset(&m_pPSConstant, 0, sizeof(m_pPSConstant));
	memset(&m_pVSConstant, 0, sizeof(m_pVSConstant));
	memset(&m_pCSConstant, 0, sizeof(m_pCSConstant));
	memset(&m_pGSConstant, 0, sizeof(m_pGSConstant));
	memset(&m_frameStats, 0, sizeof(m_frameStats));

	m_pDefaultDepthStencilSurface = m_pGXDevice->m_pDefaultDepthStencilSurface;
	m_pDefaultRasterizerState = m_pGXDevice->m_pDefaultRasterizerState;
	m_pDefaultSamplerState = m_pGXDevice->m_pDefaultSamplerState;
	m_pDefaultBlendState = m_pGXDevice->m_pDefaultBlendState;
	m_pDefaultDepthStencilState = m_pGXDevice->m_pDefaultDepthStencilState;

	m_pDefaultDepthStencilSurface->AddRef();
	m_pDefaultRasterizerState->AddRef();
	m_pDefaultSamplerState->AddRef();
	m_pDefaultBlendState->AddRef();
	m_pDefaultDepthStencilState->AddRef();

	/*OGL
	/*for(int i = 0; i < MAXDSGVSTREAM; ++i)
	{
		m_pCurVertexBuffer[i] = NULL;
	}*/
	//OGL//memset(&m_sync_state, 0, sizeof(m_sync_state));

	//m_pSPP->Define("_OGL");

	//char * declList[] = GXDECLLIST;

	//for(int j = 0, l = sizeof(declList) / sizeof(char*); j < l; ++j)
	//{
	//	m_pSPP->Define((DString(declList[j]) + " " + j).c_str());
	//}
}

CGXContext::~CGXContext()
{
	mem_release(m_pCommandList);
	mem_release(m_pDeviceContext);

	//! @todo: release all referenced resources!

	mem_release(m_pDefaultDepthStencilSurface);
	mem_release(m_pDefaultRasterizerState);
	mem_release(m_pDefaultSamplerState);
	mem_release(m_pDefaultBlendState);
	mem_release(m_pDefaultDepthStencilState);

	for (UINT i = 0; i < GX_MAX_SHADER_CONST; ++i)
	{
		mem_release(m_pVSConstant[i]);
		mem_release(m_pPSConstant[i]);
		mem_release(m_pGSConstant[i]);
		mem_release(m_pCSConstant[i]);
	}
	/*OGL
#if defined(_WINDOWS)
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(m_hRC);
	ReleaseDC((HWND)m_hWnd, m_hDC);
#endif

	mem_delete(m_pGL);*/
}

/*
void CGXContext::swapBuffers()
{
#if defined(_WINDOWS)
	::SwapBuffers(m_hDC);
#else
#	error Implement me
#endif
}
*/

void CGXContext::setClearColor(const float4_t & color)
{
	glClearColor(color.x, color.y, color.z, color.w);
}
void CGXContext::clearTarget()
{
	glClear(GL_COLOR_BUFFER_BIT);
}
void CGXContext::clearDepth(float val)
{
	glClearDepth(val);
	glClear(GL_DEPTH_BUFFER_BIT);
}
void CGXContext::clearStencil(UINT val)
{
	glClearStencil(val);
	glClear(GL_STENCIL_BITS);
}

/*void CGXContext::destroyVertexDeclaration(IGXVertexDeclaration * pDecl)
{
	if(m_pCurVertexDecl == pDecl)
	{
		//m_pGL->glDeleteBuffers(1, &((IDSGvertexDeclarationOgl*)m_pCurVertexDecl)->m_pVAO);
		m_pCurVertexDecl = NULL;
		//m_sync_state.bVertexLayout = TRUE;
	}
	mem_delete(pDecl);
	
}

void CGXContext::IASetInputLayout(IDSGvertexDeclaration * pDecl)
{
	m_pCurVertexDecl = pDecl;
	m_sync_state.bVertexLayout = TRUE;
}*/

void CGXContext::setIndexBuffer(IGXIndexBuffer * pBuff)
{
	m_pCurIndexBuffer = pBuff;
	m_sync_state.bIndexBuffer = TRUE;
}
/*void CGXContext::IASetVertexBuffers(UINT startSlot, UINT countSlots, IDSRvertexBuffer ** pBuff)
{
#if defined(_DEBUG)
	if(startSlot + countSlots >= MAXDSGVSTREAM)
	{
		DS_FATAL((DStringW(L"Unable to bound vertex buffer") + (DWORD)startSlot + " " + (DWORD)countSlots).c_str());
	}
#endif

	for(UINT i = 0; i < countSlots; ++i)
	{
		m_pCurVertexBuffer[i + startSlot] = pBuff[i];

		m_sync_state.bVertexBuffers[i + startSlot] = TRUE;
		m_sync_state.bVertexLayout = TRUE;
	}
}
*/
void CGXContext::drawIndexed(UINT uIndexCount, UINT uStartIndexLocation, int iBaseVertexLocation)
{
	syncronize();

	// https://www.khronos.org/opengl/wiki/Vertex_Rendering

	assert(!"change uIndexCount to uPrimitiveCount!");

	//UINT ptC = GetPTcount(IndexCount);
	UINT ptC = uIndexCount;

	UINT uIDXtype = ((CGXIndexBuffer*)m_pCurIndexBuffer)->m_uSize;

	if(iBaseVertexLocation == 0)
	{
		glDrawElements(m_drawPT, ptC, uIDXtype, (void*)(uStartIndexLocation * getIndexSize(uIDXtype)));
	}
	else
	{
		m_pGL->glDrawElementsBaseVertex(m_drawPT, ptC, uIDXtype, (void*)(uStartIndexLocation * getIndexSize(uIDXtype)), iBaseVertexLocation);
	}
}

void CGXContext::syncronize(UINT flags)
{
#if 0
	if(m_sync_state.bVertexLayout)
	{
		IDSGvertexDeclarationOgl * vd = (IDSGvertexDeclarationOgl*)m_pCurVertexDecl;
		//m_pGL->glBindVertexArray(vd->m_pVAO);


		UINT buff = 0;

		for(UINT i = 0; i < vd->m_uDeclCount; i++)
		{
			UINT cb = ((IDSRvertexBufferOgl*)m_pCurVertexBuffer[vd->m_pDeclItems[i].stream])->pBuffer;
			if(buff != cb || i == 0)
			{
				m_pGL->glBindBuffer(GL_ARRAY_BUFFER, cb);
				buff = cb;
			}
			m_pGL->glVertexAttribPointer(vd->m_pDeclItems[i].index, vd->m_pDeclItems[i].size, vd->m_pDeclItems[i].type​, vd->m_pDeclItems[i].normalized, vd->m_pDeclItems[i].stride​, vd->m_pDeclItems[i].pointer​);
			m_pGL->glEnableVertexAttribArray(vd->m_pDeclItems[i].index);
		}
		m_sync_state.bVertexLayout = FALSE;
	}
#endif
	if(m_sync_state.bRenderBuffer)
	{
		m_pGL->glBindVertexArray(m_pCurRenderBuffer ? ((CGXRenderBuffer*)m_pCurRenderBuffer)->m_uVAO : 0);
		m_sync_state.bRenderBuffer = FALSE;
	}
	if(m_sync_state.bIndexBuffer)
	{
		m_pGL->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pCurIndexBuffer ? ((CGXIndexBuffer*)m_pCurIndexBuffer)->m_pBuffer : 0);
		m_sync_state.bIndexBuffer = FALSE;
		//glDrawElements
	}
	
}

void CGXContext::setPrimitiveTopology(GXPRIMITIVETOPOLOGY pt)
{
	switch(pt)
	{
	case GXPT_POINTLIST:
		m_drawPT = GL_POINTS;
		break;
	case GXPT_LINELIST:
		m_drawPT = GL_LINES;
		break;
	case GXPT_LINESTRIP:
		m_drawPT = GL_LINE_STRIP;
		break;
	case GXPT_TRIANGLELIST:
		m_drawPT = GL_TRIANGLES;
		break;
	case GXPT_TRIANGLESTRIP:
		m_drawPT = GL_TRIANGLE_STRIP;
		break;
	//case GXPT_TRIANGLEFAN:
	//	m_drawPT = GL_TRIANGLE_FAN;
	//	break;
	}
}

UINT CGXContext::getPTcount(UINT idxCount)
{
	switch(m_drawPT)
	{
	case GL_POINTS:
		return(idxCount);
	case GL_LINES:
		return(idxCount / 2);
	case GL_LINE_STRIP:
		return(idxCount - 1);
	case GL_TRIANGLES:
		return(idxCount / 3);
	case GL_TRIANGLE_STRIP:
	case GL_TRIANGLE_FAN:
		return(idxCount - 2);
	}
	return(0);
}

UINT CGXContext::getIndexSize(UINT idx)
{
	switch(idx)
	{
	case GL_UNSIGNED_BYTE:
		return(1);
	case GL_UNSIGNED_SHORT:
		return(2);
	case GL_UNSIGNED_INT:
		return(4);
	}
	return(1);
}

/*IDSGshader * CGXContext::CreateShader(const char * pName, UINT flags)
{
	IDSFILE * pFile = m_pDS->FileOpen((L"/shaders/"+DStringW(DString(pName))+L".dss").c_str(), DSFM_RO | DSFM_TEXT);
	if(!pFile)
	{
		DS_ERROR((L"Unable to load shader \"" + DStringW(DString(pName)) + L"\"").c_str());
		return(NULL);
	}
	DSparser p(pFile);
	pFile->Close();
	pFile = NULL;
	DSparser * sh = p.GetNode("shader");
	if(!sh)
	{
		DS_ERROR((L"Unable to load shader \"" + DStringW(DString(pName)) + L"\"\nThere is no \"shader\" node").c_str());
		return(NULL);
	}
	DSparser * sources = sh->GetNode("source");
	if(!sources)
	{
		DS_ERROR((L"Unable to load shader \"" + DStringW(DString(pName)) + L"\"\nThere is no \"source\" node").c_str());
		return(NULL);
	}
	GLuint program = m_pGL->glCreateProgram();
	GLuint ps = 0;
	GLuint vs = 0;
	if(sources->KeyExists("vs"))
	{
		DString vsh = sources->Get("vs");
		vs = GetShaderPart(GL_VERTEX_SHADER, vsh, flags);
		m_pGL->glAttachShader(program, vs);
	}
	if(sources->KeyExists("ps"))
	{
		DString psh = sources->Get("ps");
		ps = GetShaderPart(GL_FRAGMENT_SHADER, psh, flags);
		m_pGL->glAttachShader(program, ps);
	}

	m_pGL->glLinkProgram(program);
	GLenum ErrorCheckValue = glGetError();
	if(ErrorCheckValue != GL_NO_ERROR)
	{
		DS_ERROR((L"Unable to link shader \"" + DStringW(DString(pName)) + L"\" Err: " + (DWORD)ErrorCheckValue).c_str());
		return(0);
	}
	if(vs)
	{
		m_pGL->glDetachShader(program, vs);
	}
	if(ps)
	{
		m_pGL->glDetachShader(program, ps);
	}

	IDSGshaderOgl * pShader = new IDSGshaderOgl(this);

	pShader->m_pProgram = program;

	return(pShader);
}
void CGXContext::destroyShader(IDSGshader * pSH)
{
	if(pSH)
	{
		m_pGL->glDeleteShader(((IDSGshaderOgl*)pSH)->m_pProgram);
		DSAFE_DELETE(pSH);
	}
}
void CGXContext::setShader(IDSGshader * pSH)
{
	if(pSH)
	{
		m_pGL->glUseProgram(((IDSGshaderOgl*)pSH)->m_pProgram);
	}
	else
	{
		m_pGL->glUseProgram(0);
	}
}

GLuint CGXContext::GetShaderPart(GLenum type, const DString & name, UINT flags, const char ** defs)
{
	DString _name = name;
	switch(type)
	{
	case GL_VERTEX_SHADER:
		_name += ".vsh";
		break;
	case GL_FRAGMENT_SHADER:
		_name += ".psh";
		break;
	}

	DString key = _name + "." + (int)flags;
	if(defs == NULL && m_mShaderCache.KeyExists(key))
	{
		return(m_mShaderCache[key].pShader);
	}

	const char * source;

	if(m_mShaderSources.KeyExists(_name))
	{
		source = m_mShaderSources[_name].source;
	}
	else
	{
		IDSFILE * fp = m_pDS->FileOpen((L"/shaders/" + DStringW(_name)).c_str(), DSFM_BINARY | DSFM_RO);
		if(!fp)
		{
			DS_ERROR((L"Unable to open file /shaders/" + DStringW(_name)).c_str());
			return(0);
		}
		char * _source = new char[sizeof(char) * (fp->Size() + 1)];
		_source[fp->Read((void*)_source, fp->Size())] = 0;
		source = _source;
		m_mShaderSources[_name].source = source;
	}


	const char ** pszDef = defs;
	while(pszDef)
	{
		m_pSPP->Define(*pszDef, 0, TRUE);
	}

	DString srcpp = m_pSPP->Process(source);
	m_pSPP->UndefTemp();

	GLuint pSH = m_pGL->glCreateShader(type);
	
	const char * src = srcpp.c_str();

	m_pGL->glShaderSource(pSH, 1, &src, NULL);
	m_pGL->glCompileShader(pSH);

	GLint isCompiled;
	m_pGL->glGetShaderiv(pSH, GL_COMPILE_STATUS, &isCompiled);

	if(isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		m_pGL->glGetShaderiv(pSH, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		DString errorLog;
		errorLog.Reserve(maxLength);
		m_pGL->glGetShaderInfoLog(pSH, maxLength, &maxLength, (GLchar*)(errorLog.c_str()));

		DS_ERROR(DStringW(errorLog).c_str());

		// Provide the infolog in whatever manor you deem best.
		// Exit with failure.
		m_pGL->glDeleteShader(pSH); // Don't leak the shader.
		return(0);
	}

	if(defs == NULL)
	{
		m_mShaderCache[key].pShader = pSH;
	}

	return(pSH);
}*/

IGXPixelShader * CGXContext::createPixelShader(const char * szFile)
{
	assert(!"Not imlemented");
	return(NULL);
}
IGXPixelShader * CGXContext::createPixelShader(void *pData, UINT uSize)
{
	assert(!"Not imlemented");
	return(NULL);
}
void CGXContext::destroyPixelShader(IGXPixelShader * pSH)
{
	assert(!"Not imlemented");
}
void CGXContext::setVertexShader(IGXVertexShader * pSH)
{
	assert(!"Not imlemented");
}
void CGXContext::setPixelShader(IGXPixelShader * pSH)
{
	assert(!"Not imlemented");
}

void CGXContext::destroyRenderBuffer(IGXRenderBuffer * pBuff)
{
	if(pBuff)
	{
		m_pGL->glDeleteBuffers(1, &((CGXRenderBuffer*)pBuff)->m_uVAO);
	}
	mem_delete(pBuff);
}

void CGXContext::setRenderBuffer(IGXRenderBuffer * pBuff)
{
	m_pCurRenderBuffer = pBuff;
	m_sync_state.bRenderBuffer = TRUE;
}
