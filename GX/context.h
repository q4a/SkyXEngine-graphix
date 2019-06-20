
/*****************************************************
Copyright © DogmaNet Team, 2019
Site: dogmanet.ru
See the license in LICENSE
*****************************************************/

#ifndef __CONTEXT_H
#define __CONTEXT_H

#include "color.h"
#include "ibase.h"
#include "textures.h"
#include "surfaces.h"
#include "states.h"
#include "shaders.h"
#include "adapter_stats.h"
#include "buffers.h"

//##########################################################################

//! интерфейс графического контекста
class IGXContext
{
protected:
	virtual ~IGXContext()
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

	//! изменение размеров области рендера
	virtual void resize(int iWidth, int iHeight, bool isWindowed) = 0;

	//##########################################################################

	//! создать цепочку вывода #IGXSwapChain приязанную к окну wnd
	virtual IGXSwapChain* createSwapChain(UINT uWidth, UINT uHeight, SXWINDOW wnd) = 0;

	//! обменять front и back буферы, когда необходимо показать на экране то что нарисовано
	virtual void swapBuffers() = 0;

	//! начало кадра, вызывается до рендера
	virtual bool beginFrame() = 0;

	//! конец кадра, весь рендер прошел
	virtual void endFrame() = 0;

	//! можно ли начать кадр, вызывается до #beginFrame
	virtual bool canBeginFrame() = 0;

	//##########################################################################

	//! deprecated, могли ли быть утеряны данные в буферах и текстурах, для которых это допустимо?
	virtual bool wasReset() = 0;

	/*! очистка данных рендера
	 @param what - что надо очистить, из дефайно GXCLEAR_
	 @param color - цветя для #GXCLEAR_COLOR
	 @param fDepth - глубина [0.0, 1.0] для #GXCLEAR_DEPTH
	 @param uStencil - значение для #GXCLEAR_STENCIL
	*/
	virtual void clear(UINT what, GXCOLOR color = 0, float fDepth = 1.0f, UINT uStencil = 0) = 0;

	/*! создание вершинного буфера
	 @param size - размер буфера в байтах, размер_структуры * количество_вершин
	 @param usage - тип использования из #GX_BUFFER_USAGE
	 @param pInitData - данные для заполнения, если буфер статичный (usage == GX_BUFFER_USAGE_STATIC) тогда pInitData единственный способ заполнить буфер
	*/
	virtual IGXVertexBuffer* createVertexBuffer(size_t size, GX_BUFFER_USAGE usage, void *pInitData = NULL) = 0;

	/*! создание индексного буфера
	 @param size - размер буфера в байтах, размер_структуры * количество_индексов
	 @param usage - тип использования из #GX_BUFFER_USAGE
	 @param it - тип индексов из #GXINDEXTYPE
	 @param pInitData - данные для заполнения, если буфер статичный (usage == GX_BUFFER_USAGE_STATIC) тогда pInitData единственный способ заполнить буфер
	*/
	virtual IGXIndexBuffer* createIndexBuffer(size_t size, GX_BUFFER_USAGE usage, GXINDEXTYPE it, void *pInitData = NULL) = 0;

	//! установка индексного буфера
	virtual void setIndexBuffer(IGXIndexBuffer *pBuff) = 0;

	//! установка рендер буфера
	virtual void setRenderBuffer(IGXRenderBuffer *pBuff) = 0;

	//! создание декларации вершин из массива pDecl
	virtual IGXVertexDeclaration* createVertexDeclaration(const GXVertexElement *pDecl) = 0;

	//##########################################################################

	//! установка топологии примитивов
	virtual void setPrimitiveTopology(GXPRIMITIVETOPOLOGY pt) = 0;

	/*! отрисовка индексированных примитивов
	 @param uVertexCount количество вершин, которые будут задействованы в отрисовке
	 @param uPrimitiveCount количество рисуемых примитивов
	 @param uStartIndexLocation стартовый индекс
	 @param iVertexBias смещение текущего значения индекса. Например, в индексном буфере лежит число 17, а iVertexBias равен -2
	  тогда будет взят вертекс за номером 15
	*/
	virtual void drawIndexed(UINT uVertexCount, UINT uPrimitiveCount, UINT uStartIndexLocation = 0, int iVertexBias = 0) = 0;

	/*! отрисовка индексированных примтивов с использованием hardware instancing
	 @note Параметры uVertexCount, uPrimitiveCount, uStartIndexLocation, iVertexBias указываются для ортисовки одной инстанции
	 @param uInstanceCount количество инстансов
	 @param uVertexCount количество вершин, которые будут задействованы в отрисовке
	 @param uPrimitiveCount количество рисуемых примитивов
	 @param uStartIndexLocation стартовый индекс
	 @param iVertexBias смещение текущего значения индекса
	*/
	virtual void drawIndexedInstanced(UINT uInstanceCount, UINT uVertexCount, UINT uPrimitiveCount, UINT uStartIndexLocation = 0, int iVertexBias = 0) = 0;

	/*! отрисовка примитивов
	 @param uStartVertex номер стартовой вершины
	 @param uPrimitiveCount количество рисуемых примитивов
	*/
	virtual void drawPrimitive(UINT uStartVertex, UINT uPrimitiveCount) = 0;

	//########################################################################

	// https://github.com/LukasBanana/XShaderCompiler/releases
	// https://github.com/Thekla/hlslparser/tree/master/src

	/*! выполнение compute shaders
	 @note Шейдеры могут выполняться в многопоточном режиме.
	  Аргументы данной функции задают трехмерную сетку групп потоков, которая является лишь абстрацией, и создана для связи кода с координатами этой сетки.
	  Внутри каждой группы количество потоков задается при помощи вектора numthreads внутри шейдера
	 @note Трехмерность групп и потоков связана с тем чтобы дать зависимость расчетов внутри шейдера от координат,
	  которые присваиваются каждой группе и каждому потоку.
	  Если зависимость не нужна, тогда в X координате можно указывать общее количество групп и потоков.
	  Например:
	   - computeDispatch(3, 4, 5) запустит 3x4x5 = 60 групп потоков
	   - numthreads(5,6,3) запустит 5x6x3 = 90 потоков в каждой группе,
	  итого 60x90 = 5400 потоков, при этом внутри шейдера можно будет получить доступ к координатам привязанным как к группе так и к потоку.
	  Если в предыдущем случае не нужна зависимость от координат групп и потоков, тогда:
	   - computeDispatch(60, 1, 1)
	   - numthreads(90, 1, 1)
	*/
	virtual void computeDispatch(UINT uThreadGroupCountX, UINT uThreadGroupCountY, UINT uThreadGroupCountZ) = 0;


	/*! создать вершинный шейдер загрузив код из файла szFile
	 @param szFile путь до файла
	 @param pDefs массив макроопределений передаваймых в шейдер
	*/
	virtual IGXVertexShader* createVertexShader(const char * szFile, GXMacro *pDefs = NULL) = 0;

	//! создать вершинный шейдер из строки с кодом szCode
	virtual IGXVertexShader* createVertexShaderFromString(const char * szCode, GXMacro *pDefs = NULL) = 0;

	//! создать вершинный шейдер из бинарных данных pData
	virtual IGXVertexShader* createVertexShaderFromBin(void *pData, UINT uSize) = 0;

	//! установить буфер константы pBuffer вершинному шейдеру в слот uSlot
	virtual void setVertexShaderConstant(IGXConstantBuffer *pBuffer, UINT uSlot = 0) = 0;


	virtual IGXPixelShader* createPixelShader(const char * szFile, GXMacro *pDefs = NULL) = 0;
	virtual IGXPixelShader* createPixelShaderFromString(const char * szCode, GXMacro *pDefs = NULL) = 0;
	virtual IGXPixelShader* createPixelShaderFromBin(void *pData, UINT uSize) = 0;
	virtual void setPixelShaderConstant(IGXConstantBuffer *pBuffer, UINT uSlot = 0) = 0;


	virtual IGXGeometryShader* createGeometryShader(const char * szFile, GXMacro *pDefs = NULL) = 0;
	virtual IGXGeometryShader* createGeometryShaderFromString(const char * szCode, GXMacro *pDefs = NULL) = 0;
	virtual IGXGeometryShader* createGeometryShaderFromBin(void *pData, UINT uSize) = 0;
	virtual void setGeometryShaderConstant(IGXConstantBuffer *pBuffer, UINT uSlot = 0) = 0;


	virtual IGXComputeShader* createComputeShader(const char * szFile, GXMacro *pDefs = NULL) = 0;
	virtual IGXComputeShader* createComputeShaderFromString(const char * szCode, GXMacro *pDefs = NULL) = 0;
	virtual IGXComputeShader* createComputeShaderFromBin(void *pData, UINT uSize) = 0;
	virtual void setComputeShaderConstant(IGXConstantBuffer *pBuffer, UINT uSlot = 0) = 0;


	//! создание набора шейдеров
	virtual IGXShaderSet* createShader(IGXVertexShader *pVS = NULL, IGXPixelShader *pPS = NULL, IGXGeometryShader *pGS = NULL, IGXComputeShader *pCS = NULL) = 0;

	//! устанавливает набор шейдеров
	virtual void setShader(IGXShaderSet *pSH) = 0;

	//! возвращает текущий установленный набор шейдеров, если не установлен тогда 0
	virtual IGXShaderSet* getShader() = 0;

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

	//! установить поверхность глубины и трафарета
	virtual void setDepthStencilSurface(IGXDepthStencilSurface *pSurface) = 0;

	/*! убрать текущую поверхность глубины и трафарета
	 @note Отсутствие поверхности глубины и трафарета полностью отключает стадию конвейера ответственную за глубину и трафарет
	*/
	virtual void setDepthStencilSurfaceNULL() = 0;

	//! возвращает текущую установленную поверхность глубины и трафарета
	virtual IGXDepthStencilSurface *getDepthStencilSurface() = 0;


	/*! создание поверхности цвета
	 @param uWidth ширина в пикселях
	 @param uHeight высота в пикселях
	 @param format формат буфера
	 @param multisampleType тип мультисемплинга
	 @param bAutoResize будет ли изменяться размер поверхности относительно основной цепочки вывода (создается при вызове #IGXContext::initContext), в случае ресайза
	*/
	virtual IGXSurface* createColorTarget(UINT uWidth, UINT uHeight, GXFORMAT format, GXMULTISAMPLE_TYPE multisampleType, bool bAutoResize = false) = 0;

	/*! даунсемплирование pSource в pTarget
	 @param pSource исходная поверхность с обязательным MSAA
	 @param pTarget приемная поверхность, обязана быть такого же размера и формата как pSource, без MSAA
	 @note Cмысл таков что поверхность с MSAA используется только тогда, когда нужен MSAA,
	после того как поверхность с MSAA больше не нужена ее даунсемплируют и дальше работают с даунсемплированной поверхностью
	*/
	virtual void downsampleColorTarget(IGXSurface *pSource, IGXSurface *pTarget) = 0;

	/*! установка цветового буфера
	 @param pSurf поверхность цвета
	 @param idx индекс места куда будет установлен буфер в MRT, максимальное количество #GX_MAX_COLORTARGETS
	*/
	virtual void setColorTarget(IGXSurface *pSurf, UINT idx = 0) = 0;

	//! возвращает текущую цветовую поверхность по индексу idx в MRT
	virtual IGXSurface* getColorTarget(UINT idx = 0) = 0;

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
	 @param szFileName путь до файла
	 @param uTexUsageFlags флаги использования, дефайны GX_TEXFLAG_
	 @param bAllowNonPowerOf2 разрешить текстуре иметь размеры не кратные степени 2,
	  если false тогда размеры будут подогнаны до ближайших подходящих, и этот подгон может быть не пропорциональным
	*/
	virtual IGXTexture2D* createTexture2DFromFile(const char *szFileName, UINT uTexUsageFlags, bool bAllowNonPowerOf2 = false) = 0;
	virtual IGXTextureCube* createTextureCubeFromFile(const char *szFileName, UINT uTexUsageFlags, bool bAllowNonPowerOf2 = false) = 0;

	/*! установка текстуры для пиксельного шейдера
	 @note В пиксельном шейдере можно только читать текстуру
	 @todo Переименовать в setTexturePS
	*/
	virtual void setTexture(IGXBaseTexture *pTexture, UINT uStage = 0) = 0;
	virtual IGXBaseTexture* getTexture(UINT uStage = 0) = 0;

	virtual void setTextureVS(IGXBaseTexture *pTexture, UINT uStage = 0) = 0;
	virtual IGXBaseTexture* getTextureVS(UINT uStage = 0) = 0;

	virtual void setTextureCS(IGXBaseTexture *pTexture, UINT uStage = 0) = 0;
	virtual IGXBaseTexture* getTextureCS(UINT uStage = 0) = 0;

	//! установить текстуру с произвольным доступом для вычислительного шейдера
	virtual void setUnorderedAccessViewCS(IGXBaseTexture *pUAV, UINT uStage = 0) = 0;
	virtual IGXBaseTexture* getUnorderedAccessViewCS(UINT uStage = 0) = 0;

	//! возвращает тип текстуры из файла
	virtual GXTEXTURE_TYPE getTextureTypeFromFile(const char *szFile) = 0;

	/*
	 @todo сделать метод для сохранения surface
	*/
	virtual bool saveTextureToFile(const char *szTarget, IGXBaseTexture *pTexture) = 0;

	//########################################################################

	//! создание состояния смешивания
	virtual IGXBlendState* createBlendState(GXBlendDesc *pBlendDesc) = 0;

	//! установка состояния смешивания
	virtual void setBlendState(IGXBlendState *pState) = 0;

	//! возвращает текущее установленное состояние смешивания
	virtual IGXBlendState* getBlendState() = 0;

	//! установить фактор смешивания, если в #GXBlendDesc установлен GXBLEND_BLEND_FACTOR
	virtual void setBlendFactor(GXCOLOR val) = 0;



	//! создание состояния глубины и трафарета
	virtual IGXDepthStencilState* createDepthStencilState(GXDepthStencilDesc *pDSDesc) = 0;

	//! установить состояние глубины и трафарета
	virtual void setDepthStencilState(IGXDepthStencilState *pState) = 0;

	//! возвращает текущее установленное состояние глубины и трафарета
	virtual IGXDepthStencilState* getDepthStencilState() = 0;

	//! установка значения для внутренних операций указанных в состоянии (#GXDepthStencilDesc) глубины и трафарета
	virtual void setStencilRef(UINT uVal) = 0;



	//! создание состояния растеризатора
	virtual IGXRasterizerState *createRasterizerState(GXRasterizerDesc *pRSDesc) = 0;

	//! установить состояние растеризатора
	virtual void setRasterizerState(IGXRasterizerState *pState) = 0;

	//! возвращает текущее установленное состояние растеризатора
	virtual IGXRasterizerState* getRasterizerState() = 0;

	/*! установить фиксированную облать для рендера
	 @note Параметры задают координаты 2D прямоугольнка для рендера, для всех цепочек вывода
	*/
	virtual void setScissorRect(int iTop, int iRight, int iBottom, int iLeft) = 0;
	


	//! создание состония выборки из текстуры
	virtual IGXSamplerState* createSamplerState(GXSamplerDesc *pSamplerDesc) = 0;

	//! установка выборки текстуры для слота uSlot, нумерация с 0
	virtual void setSamplerState(IGXSamplerState *pState, UINT uSlot) = 0;

	//! возвращает текущее установленное состояние выботки текстуры из слота uSlot
	virtual IGXSamplerState* getSamplerState(UINT uSlot) = 0;

	//########################################################################

	//! возвращает статистику текущего кадра
	virtual const GXFrameStats* getFrameStats() = 0;

	//! возвращает статистику занятой памяти адаптером
	virtual const GXAdapterMemoryStats* getMemoryStats() = 0;

	//! возвращает описание адаптера
	virtual const GXAdapterDesc* getAdapterDesc() = 0;


	// http://www.gamedev.ru/terms/StencilBuffer
	// virtual void setStencilTest(bool isEnable) = 0;

	//https://docs.microsoft.com/ru-ru/windows/desktop/direct3d10/d3d10-graphics-programming-guide-api-features-deprecated
};

#endif