
/*****************************************************
Copyright © DogmaNet Team, 2019
Site: dogmanet.ru
See the license in LICENSE
*****************************************************/

#ifndef __SHADERS_H
#define __SHADERS_H

#include <common/types.h>
#include <ibase.h>

/*
В шейдере 14 слотов, по одному на константу, нумерация с 0, первая объявленная константа имеет слот 0, вторая 1 и т.д.
Каждый слот вмещает 4096 регистров (float4)
*/

//##########################################################################

//! @deprecated
#define GX_SHADER_CONSTANT_FAIL ~0

//##########################################################################

/*! макроопределения передаваемые в шейдер
 @note В шейдере это выглядит так:
  #define szName szDefinition
*/
struct GXMacro
{
	//! название макрооределения
	const char *szName;

	//! значение макроопределения
	const char *szDefinition;

};

/*! финальная строка массива макроопределений
 @note Каждый массив должен заканчиваться данной макрофункцией
*/
#define GX_MACRO_END() {0,0}

//##########################################################################

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

//##########################################################################

//! набор шейдеров для каждого из этапов рендера объекта
class IGXShaderSet: public IGXBaseInterface
{
public:
	virtual IGXPixelShader* getPixelShader() = 0;
	virtual IGXGeometryShader* getGeometryShader() = 0;
	virtual IGXVertexShader* getVertexShader() = 0;
	virtual IGXComputeShader* getComputeShader() = 0;
};

#endif