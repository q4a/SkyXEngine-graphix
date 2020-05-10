
/*****************************************************
Copyright © DogmaNet Team, 2019
Site: dogmanet.ru
See the license in LICENSE
*****************************************************/

#ifndef __IGXCONTEXT_H
#define __IGXCONTEXT_H

#include <common/types.h>
#include "color.h"
#include "ibase.h"
#include "textures.h"
#include "surfaces.h"
#include "states.h"
#include "shaders.h"
#include "adapter_stats.h"
#include "buffers.h"
#include "IContextState.h"

class IGXContext: public IGXBaseInterface
{
public:
	//! проверяет, является ли этот контекст прямым контекстом отрисовки
	virtual bool isDirect() = 0;

	//! вызывается до рендера, только для непрямого контекста
	virtual void beginIndirect() = 0;

	//! весь рендер прошел, только для непрямого контекста
	virtual void endIndirect() = 0;

	//! запускает выполнение команд, накопленных в отложенном контексте
	virtual void executeIndirectContext(IGXContext *pContext) = 0;

	//! начало кадра, вызывается до рендера
	virtual bool beginFrame() = 0;

	//! конец кадра, весь рендер прошел
	virtual void endFrame() = 0;

	//! возвращает статистику текущего кадра
	virtual const GXFrameStats* getFrameStats() = 0;

	//! возвращает объект состояния контекста, все установленные стейты, шейдеры, текстуры и прочее
	virtual IGXContextState* getCurrentState() = 0;

	//! загружает состояние из объекта
	virtual void setFullState(IGXContextState *pState) = 0;

	//##########################################################################

	/*! очистка данных рендера
	@param what - что надо очистить, из дефайно GXCLEAR_
	@param color - цвет для #GXCLEAR_COLOR
	@param fDepth - глубина [0.0, 1.0] для #GXCLEAR_DEPTH
	@param uStencil - значение для #GXCLEAR_STENCIL
	*/
	virtual void clear(UINT what, const float4 &vColor = 0.0f, float fDepth = 1.0f, UINT uStencil = 0) = 0;

	//! установка индексного буфера
	virtual void setIndexBuffer(IGXIndexBuffer *pBuff) = 0;

	//! установка рендер буфера
	virtual void setRenderBuffer(IGXRenderBuffer *pBuff) = 0;

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

	//########################################################################

	//! установить буфер константы pBuffer вершинному шейдеру в слот uSlot
	virtual void setVSConstant(IGXConstantBuffer *pBuffer, UINT uSlot = 0) = 0;
	virtual void setPSConstant(IGXConstantBuffer *pBuffer, UINT uSlot = 0) = 0;
	virtual void setGSConstant(IGXConstantBuffer *pBuffer, UINT uSlot = 0) = 0;
	virtual void setCSConstant(IGXConstantBuffer *pBuffer, UINT uSlot = 0) = 0;

	//! устанавливает набор шейдеров
	virtual void setShader(IGXShaderSet *pSH) = 0;

	//! возвращает текущий установленный набор шейдеров, если не установлен тогда NULL
	virtual IGXShaderSet* getShader() = 0;

	//########################################################################

	//! установить поверхность глубины и трафарета
	virtual void setDepthStencilSurface(IGXDepthStencilSurface *pSurface) = 0;

	/*! убрать текущую поверхность глубины и трафарета
	@note Отсутствие поверхности глубины и трафарета полностью отключает стадию конвейера ответственную за глубину и трафарет
	*/
	virtual void unsetDepthStencilSurface() = 0;

	//! возвращает текущую установленную поверхность глубины и трафарета
	virtual IGXDepthStencilSurface* getDepthStencilSurface() = 0;


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

	/*! установка текстуры для пиксельного шейдера
	@note В пиксельном шейдере можно только читать текстуру
	@todo Переименовать в setTexturePS
	*/
	virtual void setPSTexture(IGXBaseTexture *pTexture, UINT uStage = 0) = 0;
	virtual IGXBaseTexture* getPSTexture(UINT uStage = 0) = 0;

	virtual void setVSTexture(IGXBaseTexture *pTexture, UINT uStage = 0) = 0;
	virtual IGXBaseTexture* getVSTexture(UINT uStage = 0) = 0;

	virtual void setCSTexture(IGXBaseTexture *pTexture, UINT uStage = 0) = 0;
	virtual IGXBaseTexture* getCSTexture(UINT uStage = 0) = 0;

	//! установить текстуру с произвольным доступом для вычислительного шейдера
	virtual void setCSUnorderedAccessView(IGXBaseTexture *pUAV, UINT uStage = 0) = 0;
	virtual IGXBaseTexture* getCSUnorderedAccessView(UINT uStage = 0) = 0;

	//########################################################################

	//! установка состояния смешивания
	virtual void setBlendState(IGXBlendState *pState) = 0;

	//! возвращает текущее установленное состояние смешивания
	virtual IGXBlendState* getBlendState() = 0;

	//! установить фактор смешивания, если в #GXBlendDesc установлен GXBLEND_BLEND_FACTOR
	virtual void setBlendFactor(GXCOLOR val) = 0;



	//! установить состояние глубины и трафарета
	virtual void setDepthStencilState(IGXDepthStencilState *pState) = 0;

	//! возвращает текущее установленное состояние глубины и трафарета
	virtual IGXDepthStencilState* getDepthStencilState() = 0;

	//! установка значения для внутренних операций указанных в состоянии (#GXDepthStencilDesc) глубины и трафарета
	virtual void setStencilRef(UINT uVal) = 0;



	//! установить состояние растеризатора
	virtual void setRasterizerState(IGXRasterizerState *pState) = 0;

	//! возвращает текущее установленное состояние растеризатора
	virtual IGXRasterizerState* getRasterizerState() = 0;

	/*! установить фиксированную облать для рендера
	@note Параметры задают координаты 2D прямоугольнка для рендера, для всех цепочек вывода
	*/
	virtual void setScissorRect(int iTop, int iRight, int iBottom, int iLeft) = 0;



	//! установка выборки текстуры для слота uSlot, нумерация с 0
	virtual void setSamplerState(IGXSamplerState *pState, UINT uSlot) = 0;

	//! возвращает текущее установленное состояние выботки текстуры из слота uSlot
	virtual IGXSamplerState* getSamplerState(UINT uSlot) = 0;

};

#endif
