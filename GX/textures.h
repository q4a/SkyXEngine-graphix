
/*****************************************************
Copyright © DogmaNet Team, 2019
Site: dogmanet.ru
See the license in LICENSE
*****************************************************/

#ifndef __TEXTURES_H
#define __TEXTURES_H

#include <common/types.h>
#include <ibase.h>

//! deprecated тип блокировки текстур
enum GXTEXLOCK
{
	GXTL_READ,
	GXTL_WRITE
};

//##########################################################################

/*! @name Свойства текстур
 @note Свойства могут быть комбинируемыми
@{*/

//! по умолчанию
#define GX_TEXFLAG_DEFAULT          0x00000000

//! render target
#define GX_TEXFLAG_RENDERTARGET     0x00000001

//! автоматически генерируемые mip-map уровни
#define GX_TEXFLAG_AUTOGENMIPMAPS   0x00000002

//! автоматическое изменение размера относительно цепочки вывода #IGXSwapChain
#define GX_TEXFLAG_AUTORESIZE       0x00000004

//! @deprecated, разрешено потерять данные, например при потере/восстановлении устройства в dx9
#define GX_TEXFLAG_ALLOWDISCARD     0x00000008

//! произвольный доступ для compute shader
#define GX_TEXFLAG_UNORDERED_ACCESS 0x00000010

//!@}

//##########################################################################

//! тип текстуры
enum GXTEXTURE_TYPE
{
	//! 2D текстура (ширина, высота)
	GXTEXTURE_TYPE_2D,

	//! кубическая текстура с 6 гранями (сторонами), каждая сторона имеет ширину и высоту
	GXTEXTURE_TYPE_CUBE,

	//! 3D текстура (ширина, высота, слои)
	GXTEXTURE_TYPE_3D,

	//! неизвестный тип
	GXTEXTURE_TYPE_UNKNOWN
};

//! идентификаторы граней кубической текстуры
enum GXCUBEMAP_FACES
{
	//! Правая сторона куба, координаты направления float3(1.0, 0.0, 0.0) 
	GXCUBEMAP_FACE_POSITIVE_X = 0,

	//! Левая сторона куба, координаты направления float3(-1.0, 0.0, 0.0) 
	GXCUBEMAP_FACE_NEGATIVE_X = 1,

	//! Верхняя сторона куба, координаты направления float3(0.0, 1.0, 0.0) 
	GXCUBEMAP_FACE_POSITIVE_Y = 2,

	//! Нижняя сторона куба, координаты направления float3(0.0, -1.0, 0.0) 
	GXCUBEMAP_FACE_NEGATIVE_Y = 3,

	//! Передняя сторона куба, координаты направления float3(0.0, 0.0, 1.0) 
	GXCUBEMAP_FACE_POSITIVE_Z = 4,

	//! Задняя сторона куба, координаты направления float3(0.0, 0.0, -1.0)
	GXCUBEMAP_FACE_NEGATIVE_Z = 5,

	GXCUBEMAP_FACE_FORCE_DWORD = 0x7fffffff
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
	 @note #IGXTexture3D и #IGXTextureCube используются как многослойные, #IGXTexture2D как 2D
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
	 @note pData должен быть размером как текстура, частичное заполнение недопустимо
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

#endif