
/*****************************************************
Copyright © DogmaNet Team, 2019
Site: dogmanet.ru
See the license in LICENSE
*****************************************************/

#ifndef __BUFFERS_H
#define __BUFFERS_H

#include <common/types.h>
#include "ibase.h"

//##########################################################################

//! максимальное количество потоков вершинных буферов
#define GX_MAX_VSTREAM 16

//##########################################################################

//! deprecated
enum GXBUFFERLOCK
{
	GXBL_READ,
	GXBL_WRITE
};

//##########################################################################

//! Тип использования буфера
enum GXBUFFER_USAGE
{
	//! данные никогда не обновлятся, нет доступа с CPU
	GXBUFFER_USAGE_STATIC = 0x01,

	//! данные будут обновляться, но не каждый кадр, есть доступ с CPU на запись
	GXBUFFER_USAGE_DYNAMIC = 0x02,

	//! данные будут обновляться каждый кадр, есть доступ с CPU на запись
	GXBUFFER_USAGE_STREAM = 0x04,
};


/*! Типы данных для декларации вершин
 @note В видеопамяти регистры четырехкомпонентные, значит GXDECLTYPE_FLOAT1 в видеопамяти будет занимать 4 float.
  GXDECLTYPE_FLOAT1 (2, 3), введены для того чтобы меньше данных гонять по шине,
  то есть с CPU в GPU будет отправлен 1 float при этом GPU допишет в регистр скрытые данные.
  Аналогичное правило действует и для других типов данных.
 @note Данные имеющие нормализацию являются float в пределах отведенной памяти. 
  Например GXDECLTYPE_UBYTE4N принимают значения [0.0/255.0, 255.0/255.0]. 
  Однако, на стороне CPU данные представлены указанными в значении типами.
 @note При пересылке со стороны CPU в GPU данные должны иметь следующие размеры (на компоненту):
  - float - 4 байта 
  - float16 - 2 байта
  - ubyte - 1 байт
  - short - 2 байта
*/
enum GXDECLTYPE
{
	//! 1 float, итоговое значение float4(x, 0.0, 0.0, 1.0)
	GXDECLTYPE_FLOAT1 = 0,

	//! 2 float, итоговое значение float4(x, y, 0.0, 1.0)
	GXDECLTYPE_FLOAT2 = 1,

	//! 3 float, итоговое значение float4(x, y, z, 1.0)
	GXDECLTYPE_FLOAT3 = 2,

	//! 4 float
	GXDECLTYPE_FLOAT4 = 3,

	//! 4 unsigned byte
	GXDECLTYPE_UBYTE4 = 5,

	//! 2 signed short, итоговое значение short4(x, y, 0, 1)
	GXDECLTYPE_SHORT2 = 6,

	//! 4 signed short
	GXDECLTYPE_SHORT4 = 7,


	//! 4 unsigned byte, итоговое значение ubyte4(x/255.0, y/255.0, z/255.0, w/255.0)
	GXDECLTYPE_UBYTE4N = 8,

	//! 2 signed short, итоговое значение short4(x/32767.0, y/32767.0, 0.0, 1.0)
	GXDECLTYPE_SHORT2N = 9,

	//! 4 signed short, итоговое значение short4(x/32767.0, y/32767.0, z/32767.0, w/32767.0)
	GXDECLTYPE_SHORT4N = 10,

	//! 2 unsigned short, итоговое значение ushort4(x/65535.0, y/65535.0, 0.0, 1.0)
	GXDECLTYPE_USHORT2N = 11,

	//! 4 unsigned short, итоговое значение ushort4(x/65535.0, y/65535.0, z/65535.0, w/65535.0)
	GXDECLTYPE_USHORT4N = 12,

	//! 2 16-bit float, итоговое значение half4(x, y, 0.0, 1.0)
	GXDECLTYPE_FLOAT16_2 = 15,

	//! 4 16-bit float
	GXDECLTYPE_FLOAT16_4 = 16,


	//! значение для завершающего элемента массива декларации вершин
	GXDECLTYPE_UNUSED = 17,
};

//! Спецификатор использования данных в вершинном буфере
enum GXDECLSPEC
{
	//! данные используются для вершин
	GXDECLSPEC_PER_VERTEX_DATA = 0,

	//! данные используются для hardware instancing
	GXDECLSPEC_PER_INSTANCE_DATA
};

/*! Семантика данных в вершинном буфере
 @todo Переименовать, надо добавить упоминание vertex
*/
enum GXDECLUSAGE
{
	//! позиция (float3, float4)
	GXDECLUSAGE_POSITION = 0,

	//! основные текстурные координаты (float2 - float4)
	GXDECLUSAGE_TEXCOORD,

	//! нормаль (float3)
	GXDECLUSAGE_NORMAL,

	//! веса смешивания (костей) (float4, на одну вершину до 4 костей, [0.0, 1.0])
	GXDECLUSAGE_BLENDWEIGHT,

	//! веса индексов (костей) (ubyte4)
	GXDECLUSAGE_BLENDINDICES,

	//! цвет (float4, без HDR [0.0, 1.0])
	GXDECLUSAGE_COLOR,

	//! касательная (float3)
	GXDECLUSAGE_TANGENT,

	//! бинормаль (float3)
	GXDECLUSAGE_BINORMAL,

	//! 
	GXDECLUSAGE_TESSFACTOR,

	//! дополнительные текстурные координаты (float2 - float4)
	GXDECLUSAGE_TEXCOORD1,
	GXDECLUSAGE_TEXCOORD2,
	GXDECLUSAGE_TEXCOORD3,
	GXDECLUSAGE_TEXCOORD4,
	GXDECLUSAGE_TEXCOORD5,
	GXDECLUSAGE_TEXCOORD6,
	GXDECLUSAGE_TEXCOORD7,
};

//! @todo Выяснить: надо ли?
#define GX_DECL_LIST { \
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

//! Характеристика элемента вершинном буфера
struct GXVertexElement
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

/*! финальная строка декларации вершин
 @note Каждая декларация должна заканчиваться данной макрофункцией
*/
#define GX_DECL_END() {0xFF,0,GXDECLTYPE_UNUSED,(GXDECLUSAGE)0, GXDECLSPEC_PER_VERTEX_DATA}

//##########################################################################

/*! тип примитивов
 @note LIST - список, на каждый примитив по необходимому количесту индексов, для линий 2, для треугольников 3
 @note STRIP - полоса, на первый примитив нужное количество индексов, на остальные по 1 индексу, а остальные из предыдущего примитива
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

/*! интерфейс базового буфера
 @note Для доступа к данным буфера необходима блокировка (lock), после окончания необходимо разблокировать (unlock).
  При блокировке невозможно использовать данные буфера для рендера
*/
class IGXBaseBuffer: public IGXBaseInterface
{
public:

	/*! заблокировать буфер для доступа к данным
	 @param ppData указатель на массив вершин
	*/
	virtual bool lock(void **ppData, GXBUFFERLOCK mode) = 0;

	//! разблокировать
	virtual void unlock() = 0;

	//! @deprecated
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

#endif