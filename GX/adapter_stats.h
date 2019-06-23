
/*****************************************************
Copyright © DogmaNet Team, 2019
Site: dogmanet.ru
See the license in LICENSE
*****************************************************/

#ifndef __GXADAPTER_STATS_H
#define __GXADAPTER_STATS_H

#include <common/types.h>

//! статистика текущего кадра
struct GXFrameStats
{
	//! количество вызовов DrawIndexedPrimitive и иных функций отрисовки
	UINT uDIPcount;

	//! количество отрисованных примитивов
	UINT uPolyCount;

	//! количество отрисованных линий
	UINT uLineCount;

	//! количество отрисованных точек
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

/*! статистика занятой памяти адаптера
 @note Занятая память в байтах
*/
struct GXAdapterMemoryStats
{
	/*! память занятая текстурами
	 @fixme: Handle mipmaps properly!
	*/
	size_t sizeTextureBytes;

	//! память занятая render target
	size_t sizeRenderTargetBytes;

	//! память занятая вершинными буферами
	size_t sizeVertexBufferBytes;

	//! память занятая индексными буферами
	size_t sizeIndexBufferBytes;

	//! память занятая шейдерными константами
	size_t sizeShaderConstBytes;
};

//! описание GPU
struct GXAdapterDesc
{
	//! название адаптера
	wchar_t szDescription[128];

	//! размер видеопамяти в байтах
	size_t sizeTotalMemory;
};

#endif