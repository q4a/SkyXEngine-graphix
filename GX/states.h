
/*****************************************************
Copyright © DogmaNet Team, 2019
Site: dogmanet.ru
See the license in LICENSE
*****************************************************/

#ifndef __GXSTATES_H
#define __GXSTATES_H

#include <common/types.h>
#include "ibase.h"

//##########################################################################

//! максимальнео количество регистров сэмплов (настроек для выборки)
#define GX_MAX_SAMPLERS 16

//! макисмальное количество регистров текстур
#define GX_MAX_TEXTURES 16

//! максимальное количество текстур с произвольным доступом (#GX_TEXUSAGE_UNORDERED_ACCESS)
#define GX_MAX_UAV_TEXTURES 8

//! макссимальное значение анизотропной фильтрации
#define GX_MAX_ANISOTROPY 16

//##########################################################################

/*! факторы смешивания, с указанием значения для каждого канала RGBA
 @note Постфиксы: s - source (источник), d - destination (приеник)
 @note Устанавливаются для источника и для применика
 @note https://dbgdiary.blogspot.com/2014/07/dual-source-blending-in-directx-11.html
  SRC1 это возможность влиять на приемника еще до того как будет произведено смешивание.
  Теоретически это работает только для одного render target.
  Для того чтобы использовать данную возможность, необходимо в пиксельном шейдере записывать 2 выходных цвета, 
  то есть якобы записывать цвета в 2 render target, однако на самом деле работа осуществляется только в одном.
  Второй выходной цвет будет умножен на цвет приемника если было использовано #GXBLEND_SRC1_COLOR
  @todo Полностью разобраться с SRC1
*/
enum GXBLEND
{
	//! (0, 0, 0, 0)
	GXBLEND_ZERO = 1,

	//! (1, 1, 1, 1)
	GXBLEND_ONE = 2,

	//! (Rs, Gs, Bs, As)
	GXBLEND_SRC_COLOR = 3,

	//! (1-Rs, 1-Gs, 1-Bs, 1-As)
	GXBLEND_INV_SRC_COLOR = 4,

	//! (As, As, As, As)
	GXBLEND_SRC_ALPHA = 5,

	//! (1-As, 1-As, 1-As, 1-As)
	GXBLEND_INV_SRC_ALPHA = 6,

	//! (Ad, Ad, Ad, Ad)
	GXBLEND_DEST_ALPHA = 7,

	//! (1-Ad, 1-Ad, 1-Ad, 1-Ad)
	GXBLEND_INV_DEST_ALPHA = 8,

	//! (Rd, Gd, Bd, Ad)
	GXBLEND_DEST_COLOR = 9,

	//! (1-Rd, 1-Gd, 1-Bd, 1-Ad)
	GXBLEND_INV_DEST_COLOR = 10,

	//! saturate(As, As, As, As)
	GXBLEND_SRC_ALPHA_SAT = 11,

	//! смешивание с установленным цветом через #IGXContext::setBlendFactor
	GXBLEND_BLEND_FACTOR = 14,

	//! смешиванием с инвертированным установленным цветом через #IGXContext::setBlendFactor
	GXBLEND_INV_BLEND_FACTOR = 15,

	GXBLEND_SRC1_COLOR = 16,
	GXBLEND_INV_SRC1_COLOR = 17,
	GXBLEND_SRC1_ALPHA = 18,
	GXBLEND_INV_SRC1_ALPHA = 19,

	GXBLEND_FORCE_DWORD = 0x7fffffff
};

//! операции смешивания
enum GXBLEND_OP
{
	//! сумма (аддитивный), a + b
	GXBLEND_OP_ADD = 1,

	//! разница (субстрактивный), a - b
	GXBLEND_OP_SUBTRACT = 2,

	//! инверсионная разница (инверсионно-субстрактивный), 1 - (a - b)
	GXBLEND_OP_REV_SUBTRACT = 3,

	//! с выборкой наименьшего
	GXBLEND_OP_MIN = 4,

	//! с выборкой наибольшего
	GXBLEND_OP_MAX = 5,

	GXBLEND_OP_FORCE_DWORD = 0x7fffffff
};

//! разрешения записи цвета на каждый канал
enum GXCOLOR_WRITE_ENABLE
{
	GXCOLOR_WRITE_ENABLE_RED = 1,
	GXCOLOR_WRITE_ENABLE_GREEN = 2,
	GXCOLOR_WRITE_ENABLE_BLUE = 4,
	GXCOLOR_WRITE_ENABLE_ALPHA = 8,
	GXCOLOR_WRITE_ENABLE_ALL = (((GXCOLOR_WRITE_ENABLE_RED | GXCOLOR_WRITE_ENABLE_GREEN) | GXCOLOR_WRITE_ENABLE_BLUE) | GXCOLOR_WRITE_ENABLE_ALPHA)
};

//! описатель смешивания
struct GXBlendDesc
{
	//! описание смешивания для каждого render target
	struct GXBlendRTdesc
	{
		//! включить/выключить смешивание
		BOOL useBlend = FALSE;

		//! фактор источника цвета
		GXBLEND blendSrcColor = GXBLEND_ONE;

		//! фактор приемника цвета
		GXBLEND blendDestColor = GXBLEND_ZERO;

		//! операция смешивания цвета
		GXBLEND_OP blendOpColor = GXBLEND_OP_ADD;

		//! фактор источника прозрачности
		GXBLEND blendSrcAlpha = GXBLEND_ONE;

		//! фактор приемника прозрачности
		GXBLEND blendDestAlpha = GXBLEND_ZERO;

		//! операция смешивания прозрачности
		GXBLEND_OP blendOpAlpha = GXBLEND_OP_ADD;

		//! какие каналы разрешено записывать
		byte u8RenderTargetWriteMask = GXCOLOR_WRITE_ENABLE_ALL;
	};

	//######################################################################

	/*! сглаживание полупрозрачных поверхностей
	 @note Рабтает при включенном MSAA, иначе результат как здесь https://gamedev.ru/code/forum/?id=179377
	  Еще одна ссылка на эту же тему https://www.gamedev.net/forums/topic/671912-alpha-to-coverage-wo-msaa/
	  Статья где можно посмотреть как MSAA работает без AlphaToCoverage и вместе https://habr.com/ru/post/343876/
	*/
	BOOL useAlphaToCoverage = FALSE;

	//! раздельное смешивание по render target включить/выключить
	BOOL useIndependentBlend = FALSE;

	//! описания смешиваний для каждого render target
	GXBlendRTdesc renderTarget[GX_MAX_COLORTARGETS];
};

//##########################################################################

//! функции сравнений, когда сравнение данных считается успешным
enum GXCOMPARISON_FUNC
{
	//! никогда
	GXCMP_NEVER = 1,

	//! <
	GXCMP_LESS,

	//! ==
	GXCMP_EQUAL,

	//! <=
	GXCMP_LESS_EQUAL,

	//! >
	GXCMP_GREATER,

	//! !=
	GXCMP_NOT_EQUAL,

	//! >=
	GXCMP_GREATER_EQUAL,

	//! всегда
	GXCMP_ALWAYS,

	GXCMP_FORCE_DWORD = 0x7fffffff
};

//! операции буфера трафарета
enum GXSTENCIL_OP
{
	//! игнорировать
	GXSTENCIL_OP_KEEP = 1,

	//! ставить 0
	GXSTENCIL_OP_ZERO,

	//! заменять
	GXSTENCIL_OP_REPLACE,

	//! инкрементировать, при достижении границы значений остается на границе (не переполняется)
	GXSTENCIL_OP_INCR_SAT,

	//! декрементировать, при достижении границы значений остается на границе (не переполняется)
	GXSTENCIL_OP_DECR_SAT,

	//! инвертировать
	GXSTENCIL_OP_INVERT,

	//! инкрементировать, может переполняться
	GXSTENCIL_OP_INCR,

	//! декрементировать, может переполняться
	GXSTENCIL_OP_DECR,

	GXSTENCIL_OP_FORCE_DWORD = 0x7fffffff
};

/*! описатель depth-stencil операций
 @note Stencil тест (тест трафарета) может осуществляться для передних к наблюдателю граней (front) и для задних (back)
 @note Маски чтения/записи помогают считывать/записывать определенные биты stencil буфера,
для передачи значения (сравнения при чтении)/записи есть функция #IGXContext::setStencilRef
*/
struct GXDepthStencilDesc
{
	//! включен ли тест глубины
	BOOL useDepthTest = TRUE;

	//! включена ли запись в буфер глубины
	BOOL useDepthWrite = TRUE;

	//! функция сравнения записываемой глубины с уже записанной
	GXCOMPARISON_FUNC cmpFuncDepth = GXCMP_LESS_EQUAL;


	//! включен ли тест трафарета
	BOOL useStencilTest = FALSE;

	//! маска считывания данных из буфера трафарета
	byte u8StencilReadMask = 0xFF;

	//! маска записи данных в буфер трафарета
	byte u8StencilWriteMask = 0xFF;

	//! общие данные теста трафарета
	struct GXStencilTestData
	{
		//! действие в случае провала stencil теста
		GXSTENCIL_OP opFail = GXSTENCIL_OP_KEEP;

		//! действие в случае провала depth теста
		GXSTENCIL_OP opDepthFail = GXSTENCIL_OP_KEEP;

		//! действие в случае успеха stencil теста
		GXSTENCIL_OP opPass = GXSTENCIL_OP_KEEP;

		//! функция сравнения #u8StencilReadMask с данными записанными в stencil буфер
		GXCOMPARISON_FUNC cmpFunc = GXCMP_ALWAYS;
	};

	//! данные теста трафарета для передних к наблюдателю граней
	GXStencilTestData stencilTestFront;

	//! данные теста трафарета для задних к наблюдателю граней
	GXStencilTestData stencilTestBack;
};

//##########################################################################

//! режим заполнения примитивов
enum GXFILL_MODE
{
	//! рисовать грани
	GXFILL_WIREFRAME,

	//! заполнять примитив полностью
	GXFILL_SOLID
};

//! режим отсечения
enum GXCULL_MODE
{
	//! не отсекать
	GXCULL_NONE,

	//! отсекать примитивы повернутые нормалью к камере (front)
	GXCULL_FRONT,

	//! отсекать примитивы развернутые нормалью от камеры (back)
	GXCULL_BACK
};

/*! описатель растеризатора
 @note Примерная формула смещения буфера глубины (при включенном/заданном смещении):
  Bias = clamp(DepthBias * MinDepth + SlopeScaledDepthBias * MaxDepthSlope, 0, DepthBiasClamp);
  MaxDepthSlope = max (abs(ddx (z)), abs(ddy(z)));
  MinDepth = 1 / pow(BitZbuffer, 2);
  https://docs.microsoft.com/ru-ru/windows/desktop/direct3d11/d3d10-graphics-programming-guide-output-merger-stage-depth-bias
  https://www.gamedev.net/forums/topic/660387-depthbias-how-is-maxdepthslope-computed/
*/
struct GXRasterizerDesc
{
	//! режим заполнения
	GXFILL_MODE fillMode = GXFILL_SOLID;

	//! режим отсечения
	GXCULL_MODE cullMode = GXCULL_BACK;

	//! коэффициент минимально доспустимой глубины
	int iDepthBias = 0;

	//! максимально допустимое смещение глубины, в случае превышения будет обрезано до этого значения
	float fDepthBiasClamp = 0.0f;

	//! прибавляемое значение к общему смещение глубины
	float fSlopeScaledDepthBias = 0.0f;

	//! включено ли отсечение на основании расстояния
	BOOL useDepthClip = 1;

	//! включен ли scissor тест
	BOOL useScissorTest = 0;

	//! включен ли multi sampling
	BOOL useMultisample = 0;

	//! включен ли антиальясинг для рендера линий
	BOOL useAntialiasedLine = 0;
};

//##########################################################################

/*! режим фильрации текстур
 @note Фильтры работают для нескольких режимов представления текстур:
  Mag (magnification) - увеличение
  Min (minification) - уменьшение
  Mipmap - уменьшенные копии исходной текстуры, которые участвуют в текстурировании на основании размера/дальности объекта с текстурой
 @note Точечная фильтрация основывается на информации соседних пикселей, дает худший результат на рендере сцены
  используется для точных расчетов в постпроцессе, когда нужна точность в получении цветовых данных (например из буфера глубины)
  Линейная фильтрация не способная дать хорошее изображение если видимая плоскость с текстурой находится под углом к камере
  сглаживает текстуру при full screen quad рендере, если текстура меньше/больше размеров цепочки вывода
  Анизотропная фильтрация выдает хорошие результаты если плоскость с текстурой находится под углом к камере,
  используется для рендера моделей с текстурами
*/
enum GXFILTER
{
	GXFILTER_MIN_MAG_MIP_POINT,
	GXFILTER_MIN_MAG_POINT_MIP_LINEAR,
	GXFILTER_MIN_POINT_MAG_LINEAR_MIP_POINT,
	GXFILTER_MIN_POINT_MAG_MIP_LINEAR,
	GXFILTER_MIN_LINEAR_MAG_MIP_POINT,
	GXFILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
	GXFILTER_MIN_MAG_LINEAR_MIP_POINT,
	GXFILTER_MIN_MAG_MIP_LINEAR,
	GXFILTER_ANISOTROPIC,

	GXFILTER_COMPARISON_MIN_MAG_MIP_POINT,
	GXFILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR,
	GXFILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT,
	GXFILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR,
	GXFILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT,
	GXFILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
	GXFILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
	GXFILTER_COMPARISON_MIN_MAG_MIP_LINEAR,
	GXFILTER_COMPARISON_ANISOTROPIC,
};

//! режим адресации текстур
enum GXTEXTURE_ADDRESS_MODE
{
	//! повторение текстуры вне диапазона [0.0, 1.0]
	GXTEXTURE_ADDRESS_WRAP,

	//! зеркальное отражение вне диапазона [0.0, 1.0]
	GXTEXTURE_ADDRESS_MIRROR,

	//! обрезка текстуры по краям, вне диапазона [0.0, 1.0] будет присвоен граничный цвет
	GXTEXTURE_ADDRESS_CLAMP,

	//! на координаты текстуры вне диапазона [0.0, 1.0] устанавливается цвет границы заданный через GXSamplerDesc::f4BorderColor
	GXTEXTURE_ADDRESS_BORDER,

	//! однократное отражение вне диапазона [0.0, 1.0], после чего эффект как #GXTEXTURE_ADDRESS_CLAMP
	GXTEXTURE_ADDRESS_MIRROR_ONCE
};

/*! описатель выборки текстур
 @note Номер текущего mipmap уровня для использования считается по формуле:
  fCurrLOD = clamp(0+fMipLODBias, fMinLOD, fMaxLOD);
  где clamp(x, a, b) - если x < a, то возвращает а, если x > b, то возвращает b, иначе возвращает x
 @note Смещение mipmap уровней (fMipLODBias), так же как и их ограничение (fMinLOD, fMaxLOD) имеет тип float, так как допускаются дробые значения,
  итоговый результат вычисления применяемого уровня зависит от установленной фильтрации
*/
struct GXSamplerDesc
{
	//! фильтрация текстур
	GXFILTER filter = GXFILTER_MIN_MAG_MIP_POINT;

	//! адресация для U (x - width) оси текстуры
	GXTEXTURE_ADDRESS_MODE addressU = GXTEXTURE_ADDRESS_WRAP;

	//! адресация для V (y - height) оси текстуры
	GXTEXTURE_ADDRESS_MODE addressV = GXTEXTURE_ADDRESS_WRAP;

	//! адресация для W (w - depth) слоя текстуры
	GXTEXTURE_ADDRESS_MODE addressW = GXTEXTURE_ADDRESS_WRAP;

	//! маскимальное значение анизотропной фильтрации, чем выше, тем лучше качество, максимум #GX_MAX_ANISOTROPY
	UINT uMaxAnisotropy = 1;

	//! функция сравнения текущего значения выборки с предыдущим (уже записанным)
	GXCOMPARISON_FUNC comparisonFunc = GXCMP_NEVER;

	//! цвет рамки в случае если адресация border, [0.0, 1.0]
	float4_t f4BorderColor;

	//! Смещение относительно расчетного mipmap уровня для выборки
	float fMipLODBias = 0.0f;

	//! номер наибольшего мип-уровня, доступного для использования (0 - самый детальный)
	float fMinLOD = 0;

	//! номер наименьшего мип-уровня, доступного для использования
	float fMaxLOD = FLT_MAX;
};

//##########################################################################

//! интерфейс состояния смешивания, #GXBlendDesc
class IGXBlendState: public IGXBaseInterface
{
};

//! интерфейс состояния глуибны и трафарета, #GXDepthStencilDesc
class IGXDepthStencilState: public IGXBaseInterface
{
};

//! интерфейс состояния растеризатора, #GXRasterizerDesc
class IGXRasterizerState: public IGXBaseInterface
{
};

//! интерфейс состояния выборки из текстуры, #GXSamplerDesc
class IGXSamplerState: public IGXBaseInterface
{
};

#endif