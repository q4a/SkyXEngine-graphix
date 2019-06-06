
//! \gtodo добавить common в репозиторий

/*! \gtodo пересмотреть методы IGXContext::create ... Shader методы повторяются аж 4 раза, как и комментарии
был вариант создать метод
virtual IGXShaderBase* createShader(GX_SHADER_TYPE type, const char * szFile, GXMACRO *pDefs = NULL) = 0;
который бы внутри создавал конкретный шейдер, а возвращал базовый,
нормальный вариант но ошибку несоответсвия типов не получить на этапе компиляции
*/

/*! \gtodo пересмотреть методы setTexture, так как они устанавливаются для типов шейдеров и повторяются 3 раза
есть вариант передавать аргументом тип шейдера
*/

/*! \gtodo добавить возможность для фильтрации текстур в других шейдерах
в метод setSamplerState отправлять тип шейдера для применения к нему фильтрации
*/

#ifndef __GRAPHIX_H
#define __GRAPHIX_H

#include <common/types.h>
#include <common/Math.h>

//##########################################################################

typedef UINT GXCOLOR;

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                ((UINT)(BYTE)(ch0) | ((UINT)(BYTE)(ch1) << 8) |   \
                ((UINT)(BYTE)(ch2) << 16) | ((UINT)(BYTE)(ch3) << 24 ))
#endif

//! создание ARGB цвета, 8 бит [0, 255] на канал (UINT)
#define GXCOLOR_ARGB(a,r,g,b) \
    ((GXCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

#define GXCOLOR_RGBA(r,g,b,a) GXCOLOR_ARGB(a,r,g,b)

#define GXCOLOR_XRGB(r,g,b)   GXCOLOR_ARGB(0xff,r,g,b)

#define GXCOLOR_XYUV(y,u,v)   GXCOLOR_ARGB(0xff,y,u,v)

#define GXCOLOR_AYUV(a,y,u,v) GXCOLOR_ARGB(a,y,u,v)

// maps floating point channels [0.f, 1.f] to GXCOLOR
#define GXCOLOR_COLORVALUE(r,g,b,a) \
    GXCOLOR_RGBA((UINT)((r)*255.f),(UINT)((g)*255.f),(UINT)((b)*255.f),(UINT)((a)*255.f))

#define GXCOLOR_COLORVALUE_V4(vec) GXCOLOR_COLORVALUE((vec).x, (vec).y, (vec).z, (vec).w)

#define GXCOLOR_COLORVECTOR_ARGB(val) float4_t( \
	(float)(((UINT)(val) >> 16) & 0xFF) / 255.0f, \
	(float)(((UINT)(val) >> 8) & 0xFF) / 255.0f, \
	(float)((UINT)(val) & 0xFF) / 255.0f, \
	(float)(((UINT)(val) >> 24) & 0xFF) / 255.0f \
)

//##########################################################################

//deprecated
enum GXBUFFERLOCK
{
	GXBL_READ, // obsolete
	GXBL_WRITE
};

//! тип блокировки текстур
enum GXTEXLOCK
{
	//! чтение
	GXTL_READ,

	//! запись
	GXTL_WRITE
};

/*! \name Идентификаторы буферов
!{*/

//! идентификатор буфера цвета
#define GXCLEAR_COLOR 0x00000001

//! идентификатор буфера глубины
#define GXCLEAR_DEPTH 0x00000002

//! идентификатор stencil буфера
#define GXCLEAR_STENCIL 0x00000004

//!}


//! тип использования буфера
enum GX_BUFFER_USAGE
{
	//! данные будут очень редко обновляться, нет доступа с CPU
	GX_BUFFER_USAGE_STATIC = 0x01,

	//! данные будут обновляться, но не каждый кадр, есть доступ с CPU на запись
	GX_BUFFER_USAGE_DYNAMIC = 0x02,

	//! данные будут обновляться каждый кадр, есть доступ с CPU на запись
	GX_BUFFER_USAGE_STREAM = 0x04,
};


/*! тип данных при декларации вершинного буфера
\note В видеопамяти регистры по 4 float, значит GXDECLTYPE_FLOAT1 в видеопамяти будет занимать 4 float.
GXDECLTYPE_FLOAT1 (2, 3), введены для того чтобы меньше данных гонять пошине,
то есть с CPU в GPU будет отправлен 1 float при этом GPU допишет в регистр скрытые данные.
Аналогичное правило действует и для других типов данных.
\note Данные имеющие нормализацию являются float в пределах отведенной памяти.
Например GXDECLTYPE_UBYTE4N принимают значения [0.0/255.0, 255.0/255.0]
*/
enum GXDECLTYPE
{
	// 1 float expanded to (value, 0., 0., 1.)
	GXDECLTYPE_FLOAT1 = 0,

	// 2 float expanded to (value, value, 0., 1.)
	GXDECLTYPE_FLOAT2 = 1,

	// 3 float expanded to (value, value, value, 1.)
	GXDECLTYPE_FLOAT3 = 2,

	// 4 float
	GXDECLTYPE_FLOAT4 = 3,

	// 4 unsigned byte
	GXDECLTYPE_UBYTE4 = 5,

	// 2 signed short expanded to (value, value, 0., 1.)
	GXDECLTYPE_SHORT2 = 6,

	// 4 signed short
	GXDECLTYPE_SHORT4 = 7,

	// Each of 4 bytes is normalized by dividing to 255.0
	GXDECLTYPE_UBYTE4N = 8,

	// 2D signed short normalized (v[0]/32767.0,v[1]/32767.0,0,1)
	GXDECLTYPE_SHORT2N = 9,

	// 4D signed short normalized (v[0]/32767.0,v[1]/32767.0,v[2]/32767.0,v[3]/32767.0)
	GXDECLTYPE_SHORT4N = 10,

	// 2D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,0,1)
	GXDECLTYPE_USHORT2N = 11,

	// 4D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,v[2]/65535.0,v[3]/65535.0)
	GXDECLTYPE_USHORT4N = 12,

	// Two 16-bit floating point values, expanded to (value, value, 0, 1)
	GXDECLTYPE_FLOAT16_2 = 15,

	// Four 16-bit floating point values
	GXDECLTYPE_FLOAT16_4 = 16,


	// When the type field in a decl is unused.
	GXDECLTYPE_UNUSED = 17,
};

/*! финальная строка декларации вершин
\note Каждая декларация должна заканчиваться данной макрофункцией
*/
#define GXDECL_END() {0xFF,0,GXDECLTYPE_UNUSED,(GXDECLUSAGE)0, GXDECLSPEC_PER_VERTEX_DATA}

//! Спецификатор использования данных в вершинном буфере
enum GXDECLSPEC
{
	//! данные используются для вершин
	GXDECLSPEC_PER_VERTEX_DATA = 0,

	//! данные используются для инстансинга
	GXDECLSPEC_PER_INSTANCE_DATA
};

//! Семантика данных в вершинном буфере
enum GXDECLUSAGE
{
	GXDECLUSAGE_POSITION = 0,
	GXDECLUSAGE_TEXCOORD,
	GXDECLUSAGE_NORMAL,
	GXDECLUSAGE_BLENDWEIGHT,
	GXDECLUSAGE_BLENDINDICES,
	GXDECLUSAGE_COLOR,
	GXDECLUSAGE_TANGENT,
	GXDECLUSAGE_BINORMAL,
	GXDECLUSAGE_TESSFACTOR,
	GXDECLUSAGE_TEXCOORD1,
	GXDECLUSAGE_TEXCOORD2,
	GXDECLUSAGE_TEXCOORD3,
	GXDECLUSAGE_TEXCOORD4,
	GXDECLUSAGE_TEXCOORD5,
	GXDECLUSAGE_TEXCOORD6,
	GXDECLUSAGE_TEXCOORD7,
};

//! \todo Выяснить: надо ли?
#define GXDECLLIST { \
	"POSITION",      \
	"TEXCOORD",      \
	"NORMAL",        \
	"BLENDWEIGHT",   \
	"BLENDINDICES",  \
	"COLOR",         \
	"TANGENT",       \
	"BINORMAL",      \
	"TESSFACTOR",    \
	"TEXCOORD1",     \
	"TEXCOORD2",     \
	"TEXCOORD3",     \
	"TEXCOORD4",     \
	"TEXCOORD5",     \
	"TEXCOORD6",     \
	"TEXCOORD7"      \
}

//! Характеристика элемента вершинного буфера
struct GXVERTEXELEMENT
{
	//! Индекс потока
	UINT8 u8Stream;

	//! Смещение в блоке данных (в байтах)
	UINT16 u16Offset;

	//! Тип
	GXDECLTYPE type;

	//! Семантика
	GXDECLUSAGE usage;

	//! Спецификатор
	GXDECLSPEC spec;
};

//##########################################################################

//! максимальное количество потоков вершинных буферов
#define GX_MAX_VSTREAM 16

//! количество цветовых render targets которое поддерживается всеми GPU
#define GX_MAX_STABLE_COLORTARGETS 4

/*! максимальное количество цветовых render targets при использовании MRT (multi render targets)
\warning Не все GPU поддерживают 8 RT, но 4 поддерживают
*/
#define GX_MAX_COLORTARGETS 8

//! максимальнео количество регистров сэмплов (настроек для выборки)
#define GX_MAX_SAMPLERS 16

//! макисмальное количество регистров текстур
#define GX_MAX_TEXTURES 16

//! максимальное количество текстур с произвольным доступом
#define GX_MAX_UAV_TEXTURES 8

//##########################################################################

//! расположение текстуры на стороне GPU
#define GX_TEXUSAGE_DEFAULT          0x00000000

//! использовать текстуру как render target
#define GX_TEXUSAGE_RENDERTARGET     0x00000001

//! автоматически генерировать mip-map уровни
#define GX_TEXUSAGE_AUTOGENMIPMAPS   0x00000002

//! автоматическое изменение размера текстуры относительно back buffer
#define GX_TEXUSAGE_AUTORESIZE       0x00000004

//! deprecated, разрешено потерять данные, например при потере/восстановлении устройства в dx9
#define GX_TEXUSAGE_ALLOWDISCARD     0x00000008

//! текстура с произвольным доступом
#define GX_TEXUSAGE_UNORDERED_ACCESS 0x00000010

//! deprecated
#define GX_SHADER_CONSTANT_FAIL ~0

//! макссимальное значение анизотропной фильтрации
#define GX_MAX_ANISOTROPY 16

//##########################################################################

/*! тип примитивов
\note LIST - список, на каждый примитив по необходимому количесту индексов, для линий 2, для треугольников 3
\note STRIP - полоса, на первый примитив нужное количество индексов, на остальные по 1 индексу, а остальные из предыдущего примитива
*/
enum GXPRIMITIVETOPOLOGY
{
	//! неопределенный
	GXPT_UNDEFINED = 0,

	//! список точек
	GXPT_POINTLIST = 1,

	//! список линий
	GXPT_LINELIST = 2,

	//! полосы линий
	GXPT_LINESTRIP = 3,

	//! список треугольников
	GXPT_TRIANGLELIST = 4,

	//! полосы треугольников
	GXPT_TRIANGLESTRIP = 5
};

//! тип индексов в индексном буфере
enum GXINDEXTYPE
{
	GXIT_UINT16,
	GXIT_UINT32
};

//##########################################################################

/*
\todo Посмотреть закоменченное и разобраться с этим
*/
enum GXFORMAT
{
	GXFMT_UNKNOWN = 0,

	GXFMT_R8G8B8 = 20,
	GXFMT_A8R8G8B8 = 21,
	GXFMT_X8R8G8B8 = 22,
	GXFMT_R5G6B5 = 23,
	//GXFMT_X1R5G5B5 = 24,
	//GXFMT_A1R5G5B5 = 25,
	//GXFMT_A4R4G4B4 = 26,
	//GXFMT_R3G3B2 = 27,
	//GXFMT_A8 = 28,
	//GXFMT_A8R3G3B2 = 29,
	//GXFMT_X4R4G4B4 = 30,
	//GXFMT_A2B10G10R10 = 31,
	//GXFMT_A8B8G8R8 = 32,
	//GXFMT_X8B8G8R8 = 33,
	//GXFMT_G16R16 = 34,
	//GXFMT_A2R10G10B10 = 35,
	GXFMT_A16B16G16R16 = 36,

	//GXFMT_A8P8 = 40,
	//GXFMT_P8 = 41,

	//GXFMT_L8 = 50,
	//GXFMT_A8L8 = 51,
	//GXFMT_A4L4 = 52,

	//GXFMT_V8U8 = 60,
	//GXFMT_L6V5U5 = 61,
	//GXFMT_X8L8V8U8 = 62,
	//GXFMT_Q8W8V8U8 = 63,
	//GXFMT_V16U16 = 64,
	//GXFMT_A2W10V10U10 = 67,

	//GXFMT_UYVY = MAKEFOURCC('U', 'Y', 'V', 'Y'),
	//GXFMT_R8G8_B8G8 = MAKEFOURCC('R', 'G', 'B', 'G'),
	//GXFMT_YUY2 = MAKEFOURCC('Y', 'U', 'Y', '2'),
	//GXFMT_G8R8_G8B8 = MAKEFOURCC('G', 'R', 'G', 'B'),
	GXFMT_DXT1 = MAKEFOURCC('D', 'X', 'T', '1'),
	//GXFMT_DXT2 = MAKEFOURCC('D', 'X', 'T', '2'),
	GXFMT_DXT3 = MAKEFOURCC('D', 'X', 'T', '3'),
	//GXFMT_DXT4 = MAKEFOURCC('D', 'X', 'T', '4'),
	GXFMT_DXT5 = MAKEFOURCC('D', 'X', 'T', '5'),
	GXFMT_ATI1N = MAKEFOURCC('A', 'T', 'I', '1'),
	GXFMT_ATI2N = MAKEFOURCC('A', 'T', 'I', '2'),


	//GXFMT_D16_LOCKABLE = 70,
	GXFMT_D32 = 71,
	//GXFMT_D15S1 = 73,
	GXFMT_D24S8 = 75,
	GXFMT_D24X8 = 77, //@DEPRECATED
	//GXFMT_D24X4S4 = 79,
	GXFMT_D16 = 80,

	//GXFMT_D32F_LOCKABLE = 82,
	//GXFMT_D24FS8 = 83,

	//GXFMT_L16 = 81,

	//GXFMT_VERTEXDATA = 100,
	//GXFMT_INDEX16 = 101,
	//GXFMT_INDEX32 = 102,

	//GXFMT_Q16W16V16U16 = 110,

	//gxFMT_MULTI2_ARGB8 = MAKEFOURCC('M', 'E', 'T', '1'),

	// Floating point surface formats

	// s10e5 formats (16-bits per channel)
	GXFMT_R16F = 111,
	GXFMT_G16R16F = 112,
	GXFMT_A16B16G16R16F = 113,

	// IEEE s23e8 formats (32-bits per channel)
	GXFMT_R32F = 114,
	GXFMT_G32R32F = 115,
	GXFMT_A32B32G32R32F = 116,

	//GXFMT_CxV8U8 = 117,

	GXFMT_FORCE_DWORD = 0x7fffffff
};

//! типы Multi-Sample буфера
enum GXMULTISAMPLE_TYPE
{
	GXMULTISAMPLE_NONE = 0,
	GXMULTISAMPLE_NONMASKABLE = 1,
	GXMULTISAMPLE_2_SAMPLES = 2,
	GXMULTISAMPLE_3_SAMPLES = 3,
	GXMULTISAMPLE_4_SAMPLES = 4,
	GXMULTISAMPLE_5_SAMPLES = 5,
	GXMULTISAMPLE_6_SAMPLES = 6,
	GXMULTISAMPLE_7_SAMPLES = 7,
	GXMULTISAMPLE_8_SAMPLES = 8,
	GXMULTISAMPLE_9_SAMPLES = 9,
	GXMULTISAMPLE_10_SAMPLES = 10,
	GXMULTISAMPLE_11_SAMPLES = 11,
	GXMULTISAMPLE_12_SAMPLES = 12,
	GXMULTISAMPLE_13_SAMPLES = 13,
	GXMULTISAMPLE_14_SAMPLES = 14,
	GXMULTISAMPLE_15_SAMPLES = 15,
	GXMULTISAMPLE_16_SAMPLES = 16,

	GXMULTISAMPLE_FORCE_DWORD = 0x7fffffff
};

//! идентификаторы граней кубической текстуры
enum GXCUBEMAP_FACES
{
	GXCUBEMAP_FACE_POSITIVE_X = 0,
	GXCUBEMAP_FACE_NEGATIVE_X = 1,
	GXCUBEMAP_FACE_POSITIVE_Y = 2,
	GXCUBEMAP_FACE_NEGATIVE_Y = 3,
	GXCUBEMAP_FACE_POSITIVE_Z = 4,
	GXCUBEMAP_FACE_NEGATIVE_Z = 5,

	GXCUBEMAP_FACE_FORCE_DWORD = 0x7fffffff
};

//##########################################################################

/*! факторы смешивания, с указанием значения для каждого канала RGBA
\note Постфиксы: s - source (источник), d - destination (приеник)
\note Устанавливаются для источника и для применика
\todo Разобраться с SRC1, объяснить что это
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

	//GXBLEND_SRC1_COLOR = 16,
	//GXBLEND_INV_SRC1_COLOR = 17,
	//GXBLEND_SRC1_ALPHA = 18,
	//GXBLEND_INV_SRC1_ALPHA = 19,

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

/*! описатель смешивания
\todo Разобрать bAlphaToCoverageEnable
*/
struct GXBLEND_DESC
{
	//! описание смешивания для каждого render target
	struct GXBLEND_RT_DESC
	{
		//! включить/выключить смешивание
		BOOL bBlendEnable = FALSE;

		//! фактор источника цвета
		GXBLEND srcBlend = GXBLEND_ONE;

		//! фактор приемника цвета
		GXBLEND destBlend = GXBLEND_ZERO;

		//! операция смешивания цвета
		GXBLEND_OP blendOp = GXBLEND_OP_ADD;

		//! фактор источника прозрачности
		GXBLEND srcBlendAlpha = GXBLEND_ONE;

		//! фактор приемника прозрачности
		GXBLEND destBlendAlpha = GXBLEND_ZERO;

		//! операция смешивания прозрачности
		GXBLEND_OP blendOpAlpha = GXBLEND_OP_ADD;

		//! какие каналы разрешено записывать
		byte u8RenderTargetWriteMask = GXCOLOR_WRITE_ENABLE_ALL;
	};

	//######################################################################

	BOOL bAlphaToCoverageEnable = FALSE;

	//! раздельное смешивание по render target включить/выключить
	BOOL bIndependentBlendEnabled = FALSE;

	//! описания смешиваний для каждого render target
	GXBLEND_RT_DESC renderTarget[GX_MAX_COLORTARGETS];
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

//! операции stencil буфера
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
\note Stencil тест может осуществляться для передних к камере граней (front) и для задних (back)
\note Маски чтения/записи помогают считывать/записывать определенные биты stencil буфера,
для передачи значения (сравнения при чтении)/записи есть функция #IGXContext::setStencilRef
*/
struct GXDEPTH_STENCIL_DESC
{
	//! включен ли тест глубины
	BOOL bDepthEnable = TRUE;

	//! включена ли запись в буфер глубины
	BOOL bEnableDepthWrite = TRUE;

	//! функция сравнения записываемой глубины с уже записанной
	GXCOMPARISON_FUNC depthFunc = GXCMP_LESS_EQUAL;


	//! включен ли stencil тест
	BOOL bStencilEnable = FALSE;

	//! маска считывания данных из stencil буфера
	byte u8StencilReadMask = 0xFF;

	//! маска записи данных в stencil буфер
	byte u8StencilWriteMask = 0xFF;


	//! действие в случае провала stencil теста
	GXSTENCIL_OP stencilFailOp = GXSTENCIL_OP_KEEP;

	//! действие в случае провала depth теста
	GXSTENCIL_OP stencilDepthFailOp = GXSTENCIL_OP_KEEP;

	//! действие в случае успеха stencil теста
	GXSTENCIL_OP stencilPassOp = GXSTENCIL_OP_KEEP;

	//! функция сравнения #u8StencilReadMask с данными записанными в stencil буфер
	GXCOMPARISON_FUNC stencilFunc = GXCMP_ALWAYS;

	GXSTENCIL_OP stencilBackFailOp = GXSTENCIL_OP_KEEP;
	GXSTENCIL_OP stencilBackDepthFailOp = GXSTENCIL_OP_KEEP;
	GXSTENCIL_OP stencilBackPassOp = GXSTENCIL_OP_KEEP;
	GXCOMPARISON_FUNC stencilBackFunc = GXCMP_ALWAYS;
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
\note Примерная формула смещения буфера глубины (при включенном/заданном смещении):
Bias = clamp(DepthBias * MinDepth + SlopeScaledDepthBias * MaxDepthSlope, 0, DepthBiasClamp);
MaxDepthSlope = max (abs(ddx (z)), abs(ddy(z)));
MinDepth = 1 / pow(BitZbuffer, 2);
https://docs.microsoft.com/ru-ru/windows/desktop/direct3d11/d3d10-graphics-programming-guide-output-merger-stage-depth-bias
https://www.gamedev.net/forums/topic/660387-depthbias-how-is-maxdepthslope-computed/

*/
struct GXRASTERIZER_DESC
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
	BOOL bDepthClipEnable = 1;

	//! включен ли scissor тест
	BOOL bScissorEnable = 0;

	//! включен ли multi sampling
	BOOL bMultisampleEnable = 0;

	//! включен ли антиальясинг для рендера линий
	BOOL bAntialiasedLineEnable = 0;
};

//##########################################################################

/*! режим фильрации текстур
\note Фильтры работают для нескольких режимов представления текстур:
Mag (magnification) - увеличение
Min (minification) - уменьшение
Mipmap - уменьшенные копии исходной текстуры, которые участвуют в текстурировании на основании размера/дальности объекта с текстурой
\note Точечная фильтрация основывается на информации соседних пикселей, дает худший результат на рендере сцены
используется для точных расчетов в постпроцессе, когда нужна точность в получении цветовых данных (например из буфера глубины)
Линейная фильтрация не способная дать хорошее изображение если видимая плоскость с текстурой находится под углом к камере
сглаживает текстуру при full screen quad рендере, если текстура меньше/больше размеров back buffer
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
	GXFILTER_ANISOTROPIC
};

//! режим адресации текстур
enum GXTEXTURE_ADDRESS_MODE
{
	//! повторение текстуры вне диапазона [0.0, 1.0]
	GXTEXTURE_ADDRESS_WRAP,

	//! зеркальное отражение вне диапазона [0.0, 1.0]
	GXTEXTURE_ADDRESS_MIRROR,

	//! обрезка текстуры по краям, вне диапазона [0.0, 1.0] будет присвоен граниный цвет
	GXTEXTURE_ADDRESS_CLAMP,

	//! на координаты текстуры вне диапазона [0.0, 1.0] устанавливается цвет границы заданный через GXSAMPLER_DESC::f4BorderColor
	GXTEXTURE_ADDRESS_BORDER,

	//! однократное отражение вне диапазона [0.0, 1.0], после чего эффект как #GXTEXTURE_ADDRESS_CLAMP
	GXTEXTURE_ADDRESS_MIRROR_ONCE
};

//! описатель выборки текстур
struct GXSAMPLER_DESC
{
	//! фильтрация текстур
	GXFILTER filter = GXFILTER_MIN_MAG_MIP_POINT;

	//! адресация для U (x - width) оси текстуры
	GXTEXTURE_ADDRESS_MODE addressU = GXTEXTURE_ADDRESS_WRAP;

	//! адресация для V (y - height) оси текстуры
	GXTEXTURE_ADDRESS_MODE addressV = GXTEXTURE_ADDRESS_WRAP;

	//! адресация для W (w - depth) оси текстуры
	GXTEXTURE_ADDRESS_MODE addressW = GXTEXTURE_ADDRESS_WRAP;

	float fMipLODBias = 0.0f;

	//! маскимальное значение анизотропной фильтрации, чем выше, тем лучше качество, максимум #GX_MAX_ANISOTROPY
	UINT uMaxAnisotropy = 1;

	//! функция сравнения текущего значения выборки с предыдущим (уже записанным)
	GXCOMPARISON_FUNC comparisonFunc = GXCMP_NEVER;

	//! цвет рамки в случае если адресация border, [0.0, 1.0]
	float4_t f4BorderColor;

	//! номер наибольшего мип-уровня, доступного для использования (0 - самый детальный)
	float fMinLOD = 0;

	//! номер наименьшего мип-уровня, доступного для использования
	float fMaxLOD = FLT_MAX;
};

//##########################################################################

//! тип текстуры
enum GXTEXTURE_TYPE
{
	GXTEXTURE_TYPE_2D,
	GXTEXTURE_TYPE_CUBE,
	GXTEXTURE_TYPE_3D,

	GXTEXTURE_TYPE_UNKNOWN
};

//! статистика текущего кадра
struct GX_FRAME_STATS
{
	UINT uDIPcount;
	UINT uPolyCount;
	UINT uLineCount;
	UINT uPointCount;

	//! количество байт, выделенных на текстуры
	UINT uUploadedBuffersTextures;

	//! количество байт выделенных на вершины
	UINT uUploadedBuffersVertexes;

	//! количество байт, выделенных на индексы
	UINT uUploadedBuffersIndices;

	//! количество байт переданных шейдерных констант
	UINT uUploadedBuffersShaderConst;
};

/*! статистика занятой памяти GPU
\todo Вместо UINT юзать size_t
*/
struct GX_GPU_MEMORY_STATS
{
	//@FIXME: Handle mipmaps properly!
	UINT uTextureBytes;
	UINT uRenderTargetBytes;
	UINT uVertexBufferBytes;
	UINT uIndexBufferBytes;
	UINT uShaderConstBytes;
};

//! описание GPU
struct GX_ADAPTER_DESC
{
	//! название GPU
	wchar_t szDescription[128];

	//! размер видеопамяти в байтах
	size_t uTotalGPUMemory;
};

//##########################################################################

//! базовый интерфейс
class IGXBaseInterface
{
protected:
	virtual ~IGXBaseInterface()
	{
	}
	UINT m_uRefCount = 1;
public:
	void AddRef()
	{
		++m_uRefCount;
	}
	virtual void Release() = 0;
};

//##########################################################################

/*! интерфейс базового буфера
\note Для доступа к данным буфера необходима блокировка (lock), после окончания необходимо разблокировать (unlock).
При блокировке невозможно использовать данные буфера для рендера
*/
class IGXBaseBuffer: public IGXBaseInterface
{
public:

	/*! заблокировать буфер для доступа к данным
	\param ppData указатель на массив вершин
	*/
	virtual bool lock(void **ppData, GXBUFFERLOCK mode) = 0;

	//! разблокировать
	virtual void unlock() = 0;

	//! deprecated
	virtual bool wasReset() = 0;
};

//! интерфейс буфера индексов
class IGXIndexBuffer: public IGXBaseBuffer
{
};

//! интерфейс буфера вершин
class IGXVertexBuffer: public IGXBaseBuffer
{
};

//! интерфейс декларации вершин
class IGXVertexDeclaration: public IGXBaseInterface
{
};

//! интерфейс рендер буфера
class IGXRenderBuffer: public IGXBaseInterface
{
};

//##########################################################################

/*! макроопределения передаваемые в шейдер
\note В шейдере это выглядит так:
#define szName szDefinition
*/
struct GXMACRO
{
	//! название макрооределения
	const char *szName;

	//! значение макроопределения
	const char *szDefinition;

};

/*! финальная строка массива макроопределений
\note Каждый массив должен заканчиваться данной макрофункцией
*/
#define GXMACRO_END() {0,0}

/*
В шейдере 14 слотов, по одному на константу, нумерация с 0, первая объявленная константа имеет слот 0, вторая 1 и т.д.
Каждый слот вмещает 4096 регистров (float4)
*/

//! интерфейс константы для шейдера
class IGXConstantBuffer: public IGXBaseInterface
{
public:

	//! обновление данных буфера
	virtual void update(const void *pData) = 0;

	//! возвращает размер буфера в байтах
	virtual UINT getSize() = 0;
};

//! базовый интерфейс шейдера
class IGXShaderBase: public IGXBaseInterface
{
public:
	//записывает бинарный код шейдера в pData, в pSize размер в байтах
	virtual void getData(void *pData, UINT *pSize) = 0;
};

//! интерфейс вершинного шейдера
class IGXVertexShader: public IGXShaderBase
{
public:
	//@DEPRECATED:
	//@{
	virtual void setConstantF(UINT uStartRegister, const float *pConstantData, UINT uVector4fCount) = 0;
	virtual void setConstantI(UINT uStartRegister, const int *pConstantData, UINT uVector4iCount) = 0;
	virtual UINT getConstantCount() = 0;
	virtual UINT getConstantLocation(const char *szConstName) = 0;
	virtual UINT getConstantSizeV4(const char *szConstName) = 0;
	//@}
};

//! интерфейс геометрического шейдера
class IGXGeometryShader: public IGXShaderBase
{
};

//! интерфейс вычислительного шейдера
class IGXComputeShader: public IGXShaderBase
{
};

//! интерфейс пиксельного шейдера
class IGXPixelShader: public IGXShaderBase
{
public:
	//@DEPRECATED:
	//@{
	virtual void setConstantF(UINT uStartRegister, const float *pConstantData, UINT uVector4fCount) = 0;
	virtual void setConstantI(UINT uStartRegister, const int *pConstantData, UINT uVector4iCount) = 0;
	virtual UINT getConstantCount() = 0;
	virtual UINT getConstantLocation(const char *szConstName) = 0;
	virtual UINT getConstantSizeV4(const char *szConstName) = 0;
	//@}
};

/*! набор шейдеров для каждого из этапов рендера объекта
*/
class IGXShaderSet: public IGXBaseInterface
{
public:
	virtual IGXPixelShader* getPixelShader() = 0;
	virtual IGXGeometryShader* getGeometryShader() = 0;
	virtual IGXVertexShader* getVertexShader() = 0;
	virtual IGXComputeShader* getComputeShader() = 0;
};

//##########################################################################

//! поверхность глубины и трафарета
class IGXDepthStencilSurface: public IGXBaseInterface
{
};

//! поверхность
class IGXSurface: public IGXBaseInterface
{
public:

	//! возвращает ширину в пикселях
	virtual UINT getWidth() = 0;

	//! возвращает высоту в пикселях
	virtual UINT getHeight() = 0;

	//! возвращает формат
	virtual GXFORMAT getFormat() = 0;
};

//##########################################################################

//! базовый интерфейс текстуры
class IGXBaseTexture: public IGXBaseInterface
{
public:

	//! возвращает формат
	virtual GXFORMAT getFormat() = 0;

	//! deprecated
	virtual bool wasReset() = 0;

	//! возвращает тип
	virtual GXTEXTURE_TYPE getType() = 0;

	/*! использовать текстуру как render target, используется нулевой лод (максимальный размер)
	\note IGXTexture3D и IGXTextureCube используются как многослойные, IGXTexture2D как 2D
	*/
	virtual IGXSurface* asRenderTarget() = 0;
};

//! базовый интерфейс 2D/3D текстур
class IGXBaseTextureD: public IGXBaseTexture
{
public:

	//! возвращает ширину в пикселях
	virtual UINT getWidth() = 0;

	//! возвращает высоту в пикселях
	virtual UINT getHeight() = 0;

	/*! заполняет текстуру данными из буфера
	\note pData должен быть размером как текстура, частичное заполнение недопустимо
	*/
	virtual void update(void *pData) = 0;
};

//! интерфейс 2D текстуры
class IGXTexture2D: public IGXBaseTextureD
{
public:

	//! возвращает mipmap уровень по номеру, нумерация с нуля, самый большой нулевой, если уровня нет - вернет 0
	virtual IGXSurface* getMipmap(UINT i = 0) = 0;

	//@DEPRECATED: 
	virtual bool lock(void **ppData, GXTEXLOCK mode) = 0;
	//@DEPRECATED: 
	virtual void unlock() = 0;
};

//! интерфейс 3D текстуры
class IGXTexture3D: public IGXBaseTextureD
{
public:

	//! возвращает глубину текстуры
	virtual UINT getDepth() = 0;
};

//! интерфейс кубической текстуры
class IGXTextureCube: public IGXBaseTexture
{
public:

	//! возвращает mipmap уровень uNum, грани face
	virtual IGXSurface* getMipmap(GXCUBEMAP_FACES face, UINT uNum = 0) = 0;

	//! возвращает размер 2D грани
	virtual UINT getSize() = 0;

	//@DEPRECATED: 
	virtual bool lock(void **ppData, GXCUBEMAP_FACES face, GXTEXLOCK mode) = 0;
	//@DEPRECATED: 
	virtual void unlock() = 0;
};

//##########################################################################

//! интерфейс состояния смешивания, #GXBLEND_DESC
class IGXBlendState: public IGXBaseInterface
{
};

//! интерфейс состояния глуибны и трафарета, #GXDEPTH_STENCIL_DESC
class IGXDepthStencilState: public IGXBaseInterface
{
};

//! интерфейс состояния растеризатора, #GXRASTERIZER_DESC
class IGXRasterizerState: public IGXBaseInterface
{
};

//! интерфейс состояния выборки из текстуры, #GXSAMPLER_DESC
class IGXSamplerState: public IGXBaseInterface
{
};

//##########################################################################

/*! цепочка вывода
\note Содержит 2 буфера:
- back - задний, в который производится рендер
- front - передний, который показывается в связанном окне (окно привязывается в методе #IGXContext::createSwapChain)
\note Последовательность:
# получение back буфер методом #IGXSwapChain::getColorTarget
# рендер
# вызов метода #IGXSwapChain::swapBuffers для показа результатов рендера в связанном окне
*/
class IGXSwapChain: public IGXBaseInterface
{
public:

	//! поменять местами back и front буферы
	virtual void swapBuffers() = 0;

	//! возвращает цветовую поверхность текущего back буфера
	virtual IGXSurface* getColorTarget() = 0;
};

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

	//! создать цепочку вывода приязанную к окну wnd
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
	\param what - что надо очистить, из дефайно GXCLEAR_
	\param color - цветя для #GXCLEAR_COLOR
	\param fDepth - глубина [0.0, 1.0] для #GXCLEAR_DEPTH
	\param uStencil - значение для #GXCLEAR_STENCIL
	*/
	virtual void clear(UINT what, GXCOLOR color = 0, float fDepth = 1.0f, UINT uStencil = 0) = 0;

	/*! создание вершинного буфера
	\param size - размер буфера в байтах, размер_структуры * количество_вершин
	\param usage - тип использования из #GX_BUFFER_USAGE
	\param pInitData - данные для заполнения, если буфер статичный (usage == GX_BUFFER_USAGE_STATIC) тогда pInitData единственный способ заполнить буфер
	*/
	virtual IGXVertexBuffer* createVertexBuffer(size_t size, GX_BUFFER_USAGE usage, void *pInitData = NULL) = 0;

	/*! создание индексного буфера
	\param size - размер буфера в байтах, размер_структуры * количество_индексов
	\param usage - тип использования из #GX_BUFFER_USAGE
	\param it - тип индексов из #GXINDEXTYPE
	\param pInitData - данные для заполнения, если буфер статичный (usage == GX_BUFFER_USAGE_STATIC) тогда pInitData единственный способ заполнить буфер
	*/
	virtual IGXIndexBuffer* createIndexBuffer(size_t size, GX_BUFFER_USAGE usage, GXINDEXTYPE it, void *pInitData = NULL) = 0;

	//! установка индексного буфера
	virtual void setIndexBuffer(IGXIndexBuffer *pBuff) = 0;

	//! установка рендер буфера
	virtual void setRenderBuffer(IGXRenderBuffer *pBuff) = 0;

	//! создание декларации вершин из массива pDecl
	virtual IGXVertexDeclaration* createVertexDeclaration(const GXVERTEXELEMENT * pDecl) = 0;

	//##########################################################################

	//! установка топологии примитивов
	virtual void setPrimitiveTopology(GXPRIMITIVETOPOLOGY pt) = 0;

	/*! отрисовка индексированных примитивов
	\todo переименовать iBaseVertexLocation
	\param uVertexCount - количество вершин, которые будут задействованы в отрисовке
	\param uPrimitiveCount - количество рисуемых примитивов
	\param uStartIndexLocation - стартовый индекс
	\param iBaseVertexLocation - номер стартовой вершины
	*/
	virtual void drawIndexed(UINT uVertexCount, UINT uPrimitiveCount, UINT uStartIndexLocation = 0, int iBaseVertexLocation = 0) = 0;

	/*! отрисовка индексированных примтивов с использованием hardware instancing
	\note Параметры uVertexCount, uPrimitiveCount, uStartIndexLocation, iBaseVertexLocation указываются для ортисовки одной инстанции
	\param uInstanceCount - количество инстансов
	\param uVertexCount - количество вершин, которые будут задействованы в отрисовке
	\param uPrimitiveCount - количество рисуемых примитивов
	\param uStartIndexLocation - стартовый индекс
	\param iBaseVertexLocation - номер стартовой вершины
	*/
	virtual void drawIndexedInstanced(UINT uInstanceCount, UINT uVertexCount, UINT uPrimitiveCount, UINT uStartIndexLocation = 0, int iBaseVertexLocation = 0) = 0;

	/*! отрисовка примитивов
	\todo Поменять параметры местами, так как в drawIndexed и drawIndexedInstanced другой порядок
	\param uStartVertex - номер стартовой вершины
	\param uPrimitiveCount - количество рисуемых примитивов
	*/
	virtual void drawPrimitive(UINT uStartVertex, UINT uPrimitiveCount) = 0;

	//########################################################################

	// https://github.com/LukasBanana/XShaderCompiler/releases
	// https://github.com/Thekla/hlslparser/tree/master/src

	/*! выполнение compute shaders
	\note Шейдеры могут выполняться в многопоточном режиме.
	Аргументы данной функции задают трехмерную сетку групп потоков, которая является лишь абстрацией, и создана для связи кода с координатами этой сетки.
	Внутри каждой группы количество потоков задается при помощи вектора numthreads внутри шейдера
	\note Трехмерность групп и потоков связана с тем чтобы дать зависимость расчетов внутри шейдера от координат,
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
	\param szFile путь до файла
	\param pDefs массив макроопределений передаваймых в шейдер
	*/
	virtual IGXVertexShader* createVertexShader(const char * szFile, GXMACRO *pDefs = NULL) = 0;

	//! создать вершинный шейдер из строки с кодом szCode
	virtual IGXVertexShader* createVertexShaderFromString(const char * szCode, GXMACRO *pDefs = NULL) = 0;

	/*! создать вершинный шейдер из бинарных данных pData
	\todo переименовать в createVertexShaderFRomBin
	*/
	virtual IGXVertexShader* createVertexShader(void *pData, UINT uSize) = 0;

	//! установить буфер константы pBuffer вершинному шейдеру в слот uSlot
	virtual void setVertexShaderConstant(IGXConstantBuffer *pBuffer, UINT uSlot = 0) = 0;


	virtual IGXPixelShader* createPixelShader(const char * szFile, GXMACRO *pDefs = NULL) = 0;
	virtual IGXPixelShader* createPixelShaderFromString(const char * szCode, GXMACRO *pDefs = NULL) = 0;
	virtual IGXPixelShader* createPixelShader(void *pData, UINT uSize) = 0;
	virtual void setPixelShaderConstant(IGXConstantBuffer *pBuffer, UINT uSlot = 0) = 0;


	virtual IGXGeometryShader* createGeometryShader(const char * szFile, GXMACRO *pDefs = NULL) = 0;
	virtual IGXGeometryShader* createGeometryShaderFromString(const char * szCode, GXMACRO *pDefs = NULL) = 0;
	virtual IGXGeometryShader* createGeometryShader(void *pData, UINT uSize) = 0;
	virtual void setGeometryShaderConstant(IGXConstantBuffer *pBuffer, UINT uSlot = 0) = 0;


	virtual IGXComputeShader* createComputeShader(const char * szFile, GXMACRO *pDefs = NULL) = 0;
	virtual IGXComputeShader* createComputeShaderFromString(const char * szCode, GXMACRO *pDefs = NULL) = 0;
	virtual IGXComputeShader* createComputeShader(void *pData, UINT uSize) = 0;
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
	\param countSlots количество вершинных буферов в ppBuff
	\param ppBuff указатель на массив вершинных буферов
	\param pDecl декларация вершин
	*/
	virtual IGXRenderBuffer* createRenderBuffer(UINT countSlots, IGXVertexBuffer **ppBuff, IGXVertexDeclaration *pDecl) = 0;

	/*! создание 2D поверхности глубины и трафарета
	\param uWidth ширина в пикселях
	\param uHeight высота в пикселях
	\param format формат буфера
	\param multisampleType тип мультисемплинга
	\param bAutoResize будет ли изменяться размер поверхности относительно основной цепочки вывода, в случае ресайза
	*/
	virtual IGXDepthStencilSurface* createDepthStencilSurface(UINT uWidth, UINT uHeight, GXFORMAT format, GXMULTISAMPLE_TYPE multisampleType, bool bAutoResize = false) = 0;

	//! создание кубического буфера глубины и трафарета
	virtual IGXDepthStencilSurface* createDepthStencilSurfaceCube(UINT uSize, GXFORMAT format, GXMULTISAMPLE_TYPE multisampleType, bool bAutoResize = false) = 0;

	//! установить поверхность глубины и трафарета
	virtual void setDepthStencilSurface(IGXDepthStencilSurface *pSurface) = 0;

	/*! убрать текущую поверхность глубины и трафарета
	\note Отсутствие поверхности глубины и трафарета полностью отключает стадию конвейера ответственную за глубину и трафарет
	*/
	virtual void setDepthStencilSurfaceNULL() = 0;

	//! возвращает текущую установленную поверхность глубины и трафарета
	virtual IGXDepthStencilSurface *getDepthStencilSurface() = 0;


	/*! создание поверхности цвета
	\param uWidth ширина в пикселях
	\param uHeight высота в пикселях
	\param format формат буфера
	\param multisampleType тип мультисемплинга
	\param bAutoResize будет ли изменяться размер поверхности относительно основной цепочки вывода, в случае ресайза
	*/
	virtual IGXSurface* createColorTarget(UINT uWidth, UINT uHeight, GXFORMAT format, GXMULTISAMPLE_TYPE multisampleType, bool bAutoResize = false) = 0;

	/*! даунсемплирование pSource в pTarget
	\param pSource исходная поверхность с обязательным MSAA
	\param pTarget приемная поверхность, обязана быть такого же размера и формата как pSource, без MSAA
	\note Cмысл таков что поверхность с MSAA используется только тогда, когда нужен MSAA,
	после того как поверхность с MSAA больше не нужена ее даунсемплируют и дальше работают с даунсемплированной поверхностью
	*/
	virtual void downsampleColorTarget(IGXSurface *pSource, IGXSurface *pTarget) = 0;

	/*! установка цветового буфера
	\param pSurf поверхность цвета
	\param idx индекс места куда будет установлен буфер в MRT, максимальное количество #GX_MAX_COLORTARGETS
	*/
	virtual void setColorTarget(IGXSurface *pSurf, UINT idx = 0) = 0;

	//! возвращает текущую цветовую поверхность по индексу idx в MRT
	virtual IGXSurface* getColorTarget(UINT idx = 0) = 0;

	//########################################################################

	//https://docs.microsoft.com/en-us/windows/desktop/direct3d11/typed-unordered-access-view-loads

	/*! создать 2D текстуру
	\param uWidth
	\param uHeight
	\param uMipLevels количество mipmap уровней, минимум 1
	\param uTexUsageFlags флаги использования, дефайны GX_TEXUSAGE_
	\param format формат текстуры
	\param pInitData массив данных для заполнения, опционально
	*/
	virtual IGXTexture2D* createTexture2D(UINT uWidth, UINT uHeight, UINT uMipLevels, UINT uTexUsageFlags, GXFORMAT format, void *pInitData = NULL) = 0;
	virtual IGXTexture3D* createTexture3D(UINT uWidth, UINT uHeight, UINT uDepth, UINT uMipLevels, UINT uTexUsageFlags, GXFORMAT format, void *pInitData = NULL) = 0;
	virtual IGXTextureCube* createTextureCube(UINT uSize, UINT uMipLevels, UINT uTexUsageFlags, GXFORMAT format, void *pInitData = NULL) = 0;

	/*! загрузка 2D текстуры из файла
	\param szFileName путь до файла
	\param uTexUsageFlags флаги использования, дефайны GX_TEXUSAGE_
	\param bAllowNonPowerOf2 разрешить текстуре иметь размеры не кратные степени 2,
	если false тогда размеры будут подогнаны до ближайших подходящих, и этот подгон может быть не пропорциональным
	*/
	virtual IGXTexture2D* createTexture2DFromFile(const char *szFileName, UINT uTexUsageFlags, bool bAllowNonPowerOf2 = false) = 0;
	virtual IGXTextureCube* createTextureCubeFromFile(const char *szFileName, UINT uTexUsageFlags, bool bAllowNonPowerOf2 = false) = 0;

	/*! установка текстуры для пиксельного шейдера
	\note В пиксельном шейдере можно только читать текстуру
	\todo Переименовать в setTexturePS
	*/
	virtual void setTexture(IGXBaseTexture *pTexture, UINT uStage = 0) = 0;
	virtual IGXBaseTexture* getTexture(UINT uStage = 0) = 0;

	virtual void setTextureVS(IGXBaseTexture *pTexture, UINT uStage = 0) = 0;
	virtual IGXBaseTexture* getTextureVS(UINT uStage = 0) = 0;

	virtual void setTextureCS(IGXBaseTexture *pTexture, UINT uStage = 0) = 0;
	virtual IGXBaseTexture* getTextureCS(UINT uStage = 0) = 0;

	/*! установить текстуру с произвольным доступом для вычислительного шейдера
	\todo Переименовать, в слове Veiw ошибка
	*/
	virtual void setUnorderedAccessVeiwCS(IGXBaseTexture *pUAV, UINT uStage = 0) = 0;
	virtual IGXBaseTexture* getUnorderedAccessVeiwCS(UINT uStage = 0) = 0;

	//! возвращает тип текстуры из файла
	virtual GXTEXTURE_TYPE getTextureTypeFromFile(const char *szFile) = 0;

	/*
	\todo сделать метод для сохранения surface
	*/
	virtual bool saveTextureToFile(const char *szTarget, IGXBaseTexture *pTexture) = 0;

	//########################################################################

	//! создание состояния смешивания
	virtual IGXBlendState* createBlendState(GXBLEND_DESC *pBlendDesc) = 0;

	//! установка состояния смешивания
	virtual void setBlendState(IGXBlendState *pState) = 0;

	//! возвращает текущее установленное состояние смешивания
	virtual IGXBlendState* getBlendState() = 0;

	//! установить фактор смешивания, если в GXBLEND_DESC установлен GXBLEND_BLEND_FACTOR
	virtual void setBlendFactor(GXCOLOR val) = 0;



	//! создание состояния глубины и трафарета
	virtual IGXDepthStencilState* createDepthStencilState(GXDEPTH_STENCIL_DESC *pDSDesc) = 0;

	//! установить состояние глубины и трафарета
	virtual void setDepthStencilState(IGXDepthStencilState *pState) = 0;

	//! возвращает текущее установленное состояние глубины и трафарета
	virtual IGXDepthStencilState* getDepthStencilState() = 0;

	//! установка значения для внутренних операций указанных в состоянии (#GXDEPTH_STENCIL_DESC) глубины и трафарета
	virtual void setStencilRef(UINT uVal) = 0;



	//! создание состояния растеризатора
	virtual IGXRasterizerState *createRasterizerState(GXRASTERIZER_DESC *pRSDesc) = 0;

	//! установить состояние растеризатора
	virtual void setRasterizerState(IGXRasterizerState *pState) = 0;

	//! возвращает текущее установленное состояние растеризатора
	virtual IGXRasterizerState* getRasterizerState() = 0;

	/*! установить фиксированную облать для рендера
	\note Параметры задают координаты 2D прямоугольнка для рендера относительно back буфера
	*/
	virtual void setScissorRect(int iTop, int iRight, int iBottom, int iLeft) = 0;
	


	//! создание состония выборки из текстуры
	virtual IGXSamplerState* createSamplerState(GXSAMPLER_DESC *pSamplerDesc) = 0;

	//! установка выборки текстуры для слота uSlot, нумерация с 0
	virtual void setSamplerState(IGXSamplerState *pState, UINT uSlot) = 0;

	//! возвращает текущее установленное состояние выботки текстуры из слота uSlot
	virtual IGXSamplerState* getSamplerState(UINT uSlot) = 0;

	//########################################################################

	//! возвращает статистику текущего кадра
	virtual const GX_FRAME_STATS* getFrameStats() = 0;

	//! возвращает статистику занятой памяти GPU
	virtual const GX_GPU_MEMORY_STATS* getMemoryStats() = 0;

	//! возвращает описание GPU
	virtual const GX_ADAPTER_DESC* getAdapterDesc() = 0;


	// http://www.gamedev.ru/terms/StencilBuffer
	// virtual void setStencilTest(bool isEnable) = 0;

	//https://docs.microsoft.com/ru-ru/windows/desktop/direct3d10/d3d10-graphics-programming-guide-api-features-deprecated
};

#endif
