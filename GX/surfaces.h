
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

/*! @name Идентификаторы буферов
!{*/

//! идентификатор буфера цвета
#define GXCLEAR_COLOR 0x00000001

//! идентификатор буфера глубины
#define GXCLEAR_DEPTH 0x00000002

//! идентификатор stencil буфера
#define GXCLEAR_STENCIL 0x00000004

//!}

//##########################################################################

/*
 @todo Посмотреть закоменченное и разобраться с этим
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