
/*****************************************************
Copyright © DogmaNet Team, 2019
Site: dogmanet.ru
See the license in LICENSE
*****************************************************/

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

#include "color.h"
#include "ibase.h"
#include "textures.h"
#include "surfaces.h"
#include "states.h"
#include "shaders.h"
#include "gpu_stats.h"
#include "buffers.h"
#include "context.h"

#endif
