
/*****************************************************
Copyright © DogmaNet Team, 2019
Site: dogmanet.ru
See the license in LICENSE
*****************************************************/

#ifndef __SURFACES_H
#define __SURFACES_H

#include <common/types.h>
#include <ibase.h>

//##########################################################################

//! количество цветовых render targets которое поддерживается всеми GPU
#define GX_MAX_STABLE_COLORTARGETS 4

/*! максимальное количество цветовых render targets при использовании MRT (multi render targets)
 @warning Не все GPU поддерживают 8 RT, но 4 поддерживают
*/
#define GX_MAX_COLORTARGETS 8

//##########################################################################

/*! @name Флаги очистки буферов
!{*/

//! Флаг очистки буфера цвета
#define GX_CLEAR_COLOR 0x00000001

//! Флаг очистки буфера глубины
#define GX_CLEAR_DEPTH 0x00000002

//! Флаг очистки stencil буфера
#define GX_CLEAR_STENCIL 0x00000004

//!}

//##########################################################################

/*
 @todo Подумать над разделением форматов по видам (для текстур, рт, глубины), может быть раскидать по енумам, а может быть просто добавить в имена
*/
enum GXFORMAT
{
	GXFMT_UNKNOWN = 0,

	GXFMT_R8G8B8 = 20,
	GXFMT_A8R8G8B8 = 21,
	GXFMT_X8R8G8B8 = 22,
	GXFMT_R5G6B5 = 23,
	GXFMT_A16B16G16R16 = 36,

	GXFMT_DXT1 = MAKEFOURCC('D', 'X', 'T', '1'),
	GXFMT_DXT3 = MAKEFOURCC('D', 'X', 'T', '3'),
	GXFMT_DXT5 = MAKEFOURCC('D', 'X', 'T', '5'),
	GXFMT_ATI1N = MAKEFOURCC('A', 'T', 'I', '1'),
	GXFMT_ATI2N = MAKEFOURCC('A', 'T', 'I', '2'),

	GXFMT_D32 = 71,
	GXFMT_D24S8 = 75,
	GXFMT_D24X8 = 77, //@DEPRECATED
	GXFMT_D16 = 80,

	// Floating point surface formats

	// s10e5 formats (16-bits per channel)
	GXFMT_R16F = 111,
	GXFMT_G16R16F = 112,
	GXFMT_A16B16G16R16F = 113,

	// IEEE s23e8 formats (32-bits per channel)
	GXFMT_R32F = 114,
	GXFMT_G32R32F = 115,
	GXFMT_A32B32G32R32F = 116,

	GXFMT_FORCE_DWORD = 0x7fffffff
};

//! типы Multi-Sample буфера
enum GXMULTISAMPLE_TYPE
{
	//! без мультисемплинга
	GXMULTISAMPLE_NONE = 0,

	//! немаскируемый мультисемплируемый буфер, только для получения anti-aliasing
	GXMULTISAMPLE_NONMASKABLE = 1,

	//! маскируемый мультисемплируемый буфер с 2 субпикселями на пиксель
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

/*! цепочка вывода
 @note Содержит 2 буфера:
  - back - задний, в который производится рендер
  - front - передний, который показывается в связанном окне (окно привязывается в методе #IGXContext::createSwapChain)
 @note Последовательность:
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

#endif