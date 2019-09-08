
/*****************************************************
Copyright © DogmaNet Team, 2019
Site: dogmanet.ru
See the license in LICENSE
*****************************************************/

#ifndef __IGXDEVICE_H
#define __IGXDEVICE_H

#include "IContext.h"

//##########################################################################

//! интерфейс графического контекста
class IGXDevice
{
protected:
	virtual ~IGXDevice()
	{
	}
public:

	/*! инициализация контекста
	 @note В оконном режиме (windowed) можно указать произвольный размер окна, а в полноэкранном (fullscreen) допустимы только поддерживаемые режимы
	 @note В область рендера будет растянута/сжата по размерам окна
	 @param hWnd - нативный дескриптов окна
	 @param iWidth - ширина области рендера в пикселях
	 @param iHeight - высота области рендера в пикселях
	 @param isWindowed - true-оконный режим, false-полноэкранный
	*/
	virtual BOOL initContext(SXWINDOW hWnd, int iWidth, int iHeight, bool isWindowed) = 0;
	virtual void Release() = 0;
	virtual void setLogger(IGXLogger *pLogger) = 0;

	//! изменение размеров области рендера
	virtual void resize(int iWidth, int iHeight, bool isWindowed) = 0;

	//##########################################################################

	//! возвращает основной контекст рендера
	virtual IGXContext* getDirectContext() = 0;

	//! создает непрямой контекст для работы из другого потока
	virtual IGXContext* createIndirectContext() = 0;

	//##########################################################################

	//! создать цепочку вывода #IGXSwapChain приязанную к окну wnd
	virtual IGXSwapChain* createSwapChain(UINT uWidth, UINT uHeight, SXWINDOW wnd) = 0;

	//! обменять front и back буферы, когда необходимо показать на экране то что нарисовано
	virtual void swapBuffers() = 0;

	//##########################################################################
	
	/*! создание вершинного буфера
	 @param size - размер буфера в байтах, размер_структуры * количество_вершин
	 @param usage - тип использования из #GXBUFFER_USAGE
	 @param pInitData - данные для заполнения, если буфер статичный (usage == GXBUFFER_USAGE_STATIC) тогда pInitData единственный способ заполнить буфер
	*/
	virtual IGXVertexBuffer* createVertexBuffer(size_t size, GXBUFFER_USAGE usage, void *pInitData = NULL) = 0;

	/*! создание индексного буфера
	 @param size - размер буфера в байтах, размер_структуры * количество_индексов
	 @param usage - тип использования из #GXBUFFER_USAGE
	 @param it - тип индексов из #GXINDEXTYPE
	 @param pInitData - данные для заполнения, если буфер статичный (usage == GXBUFFER_USAGE_STATIC) тогда pInitData единственный способ заполнить буфер
	*/
	virtual IGXIndexBuffer* createIndexBuffer(size_t size, GXBUFFER_USAGE usage, GXINDEXTYPE it, void *pInitData = NULL) = 0;

	//! создание декларации вершин из массива pDecl
	virtual IGXVertexDeclaration* createVertexDeclaration(const GXVertexElement *pDecl) = 0;

	//##########################################################################

	// https://github.com/LukasBanana/XShaderCompiler/releases
	// https://github.com/Thekla/hlslparser/tree/master/src

	/*! создать вершинный шейдер загрузив код из файла szFile
	 @param szFile путь до файла
	 @param pDefs массив макроопределений передаваймых в шейдер
	*/
	virtual IGXVertexShader* createVertexShader(const char * szFile, GXMacro *pDefs = NULL) = 0;

	//! создать вершинный шейдер из строки с кодом szCode
	virtual IGXVertexShader* createVertexShaderFromString(const char * szCode, GXMacro *pDefs = NULL) = 0;

	//! создать вершинный шейдер из бинарных данных pData
	virtual IGXVertexShader* createVertexShaderFromBin(void *pData, UINT uSize) = 0;
	

	virtual IGXPixelShader* createPixelShader(const char * szFile, GXMacro *pDefs = NULL) = 0;
	virtual IGXPixelShader* createPixelShaderFromString(const char * szCode, GXMacro *pDefs = NULL) = 0;
	virtual IGXPixelShader* createPixelShaderFromBin(void *pData, UINT uSize) = 0;
	
	virtual IGXGeometryShader* createGeometryShader(const char * szFile, GXMacro *pDefs = NULL) = 0;
	virtual IGXGeometryShader* createGeometryShaderFromString(const char * szCode, GXMacro *pDefs = NULL) = 0;
	virtual IGXGeometryShader* createGeometryShaderFromBin(void *pData, UINT uSize) = 0;

	virtual IGXComputeShader* createComputeShader(const char * szFile, GXMacro *pDefs = NULL) = 0;
	virtual IGXComputeShader* createComputeShaderFromString(const char * szCode, GXMacro *pDefs = NULL) = 0;
	virtual IGXComputeShader* createComputeShaderFromBin(void *pData, UINT uSize) = 0;

	/*
	virtual IGXShaderBlob* createShaderBlob(GXSHADER_TYPE type, const char * szCode) = 0;
	virtual IGXShaderBlob* createShaderBlob(GXSHADER_TYPE type, void *pData, UINT uSize) = 0;

	virtual IGXVertexShader* createVS(IGXShaderBlob *pBlob) = 0;
	virtual IGXPixelShader* createPS(IGXShaderBlob *pBlob) = 0;
	virtual IGXGeometryShader* createGS(IGXShaderBlob *pBlob) = 0;
	virtual IGXComputeShader* createCS(IGXShaderBlob *pBlob) = 0;
	*/

	//! создание набора шейдеров
	virtual IGXShaderSet* createShader(IGXVertexShader *pVS = NULL, IGXPixelShader *pPS = NULL, IGXGeometryShader *pGS = NULL, IGXComputeShader *pCS = NULL) = 0;

	//! создание константы размером uSize байт
	virtual IGXConstantBuffer* createConstantBuffer(UINT uSize) = 0;

	//########################################################################

	/*! создать рендер буфер
	 @param countSlots количество вершинных буферов в ppBuff
	 @param ppBuff указатель на массив вершинных буферов
	 @param pDecl декларация вершин
	*/
	virtual IGXRenderBuffer* createRenderBuffer(UINT countSlots, IGXVertexBuffer **ppBuff, IGXVertexDeclaration *pDecl) = 0;

	/*! создание 2D поверхности глубины и трафарета
	 @param uWidth ширина в пикселях
	 @param uHeight высота в пикселях
	 @param format формат буфера
	 @param multisampleType тип мультисемплинга
	 @param bAutoResize будет ли изменяться размер поверхности относительно основной цепочки вывода (создается при вызове #IGXContext::initContext), в случае ресайза
	*/
	virtual IGXDepthStencilSurface* createDepthStencilSurface(UINT uWidth, UINT uHeight, GXFORMAT format, GXMULTISAMPLE_TYPE multisampleType, bool bAutoResize = false) = 0;

	//! создание кубического буфера глубины и трафарета
	virtual IGXDepthStencilSurface* createDepthStencilSurfaceCube(UINT uSize, GXFORMAT format, GXMULTISAMPLE_TYPE multisampleType, bool bAutoResize = false) = 0;
	

	/*! создание поверхности цвета
	 @param uWidth ширина в пикселях
	 @param uHeight высота в пикселях
	 @param format формат буфера
	 @param multisampleType тип мультисемплинга
	 @param bAutoResize будет ли изменяться размер поверхности относительно основной цепочки вывода (создается при вызове #IGXContext::initContext), в случае ресайза
	*/
	virtual IGXSurface* createColorTarget(UINT uWidth, UINT uHeight, GXFORMAT format, GXMULTISAMPLE_TYPE multisampleType, bool bAutoResize = false) = 0;

	//########################################################################

	//https://docs.microsoft.com/en-us/windows/desktop/direct3d11/typed-unordered-access-view-loads

	/*! создать 2D текстуру
	 @param uWidth ширина в пикселях
	 @param uHeight высота в пикселях
	 @param uMipLevels количество mipmap уровней, минимум 1
	 @param uTexUsageFlags флаги использования, дефайны GX_TEXFLAG_
	 @param format формат текстуры
	 @param pInitData массив данных для заполнения, опционально
	*/
	virtual IGXTexture2D* createTexture2D(UINT uWidth, UINT uHeight, UINT uMipLevels, UINT uTexUsageFlags, GXFORMAT format, void *pInitData = NULL) = 0;
	virtual IGXTexture3D* createTexture3D(UINT uWidth, UINT uHeight, UINT uDepth, UINT uMipLevels, UINT uTexUsageFlags, GXFORMAT format, void *pInitData = NULL) = 0;
	virtual IGXTextureCube* createTextureCube(UINT uSize, UINT uMipLevels, UINT uTexUsageFlags, GXFORMAT format, void *pInitData = NULL) = 0;

	/*! загрузка 2D текстуры из файла
	 @deprecated 
	 @param szFileName путь до файла
	 @param uTexUsageFlags флаги использования, дефайны GX_TEXFLAG_
	 @param bAllowNonPowerOf2 разрешить текстуре иметь размеры не кратные степени 2,
	  если false тогда размеры будут подогнаны до ближайших подходящих, и этот подгон может быть не пропорциональным
	*/
	virtual IGXTexture2D* createTexture2DFromFile(const char *szFileName, UINT uTexUsageFlags, bool bAllowNonPowerOf2 = false) = 0;
	virtual IGXTextureCube* createTextureCubeFromFile(const char *szFileName, UINT uTexUsageFlags, bool bAllowNonPowerOf2 = false) = 0;

	/*
	 @todo сделать метод для сохранения surface
	 @deprecated
	*/
	virtual bool saveTextureToFile(const char *szTarget, IGXBaseTexture *pTexture) = 0;

	//########################################################################

	//! создание состояния смешивания
	virtual IGXBlendState* createBlendState(GXBlendDesc *pBlendDesc) = 0;
	
	//! создание состояния глубины и трафарета
	virtual IGXDepthStencilState* createDepthStencilState(GXDepthStencilDesc *pDSDesc) = 0;

	//! создание состояния растеризатора
	virtual IGXRasterizerState *createRasterizerState(GXRasterizerDesc *pRSDesc) = 0;

	//! создание состония выборки из текстуры
	virtual IGXSamplerState* createSamplerState(GXSamplerDesc *pSamplerDesc) = 0;

	//########################################################################

	//! возвращает статистику занятой памяти адаптером
	virtual const GXAdapterMemoryStats* getMemoryStats() = 0;

	//! возвращает описание адаптера
	virtual const GXAdapterDesc* getAdapterDesc() = 0;


	// http://www.gamedev.ru/terms/StencilBuffer
	// virtual void setStencilTest(bool isEnable) = 0;

	//https://docs.microsoft.com/ru-ru/windows/desktop/direct3d10/d3d10-graphics-programming-guide-api-features-deprecated
};

#endif
