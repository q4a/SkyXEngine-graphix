
/*****************************************************
Copyright © DogmaNet Team, 2019
Site: dogmanet.ru
See the license in LICENSE
*****************************************************/

#ifndef __COLOR_H
#define __COLOR_H

#include <common/types.h>

//! тип 32 битного (4 байта) цвета, на каждую компоненту 8 бит [0, 255]
typedef UINT GXCOLOR;

#ifndef MAKEFOURCC

//! преобразования 4х символов в число
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                ((UINT)(BYTE)(ch0) | ((UINT)(BYTE)(ch1) << 8) |   \
                ((UINT)(BYTE)(ch2) << 16) | ((UINT)(BYTE)(ch3) << 24 ))
#endif

//! создание GXCOLOR ARGB цвета
#define GX_COLOR_ARGB(a,r,g,b) \
    ((GXCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

//! создание GXCOLOR RGBА цвета
#define GX_COLOR_RGBA(r,g,b,a) GX_COLOR_ARGB(a,r,g,b)

//! создание RGB цвета, в A компоненте 255
#define GX_COLOR_XRGB(r,g,b)   GX_COLOR_ARGB(0xff,r,g,b)


//! создание YUV цвета с альфа компонентой
#define GX_COLOR_AYUV(a,y,u,v) GX_COLOR_ARGB(a,y,u,v)

//! создание YUV цвета без альфа компоненты, значение альфы 255
#define GX_COLOR_XYUV(y,u,v)   GX_COLOR_ARGB(0xff,y,u,v)


//! преобразование цвета с float компонентами [0.f, 1.f] в GXCOLOR RGBA
#define GX_COLOR_FLOAT_RGBA(r,g,b,a) \
    GX_COLOR_RGBA((UINT)((r)*255.f),(UINT)((g)*255.f),(UINT)((b)*255.f),(UINT)((a)*255.f))

//! преобразование цвета float4 с компонентами [0.f, 1.f] в GXCOLOR, XYZW -> RGBA
#define GX_COLOR_F4_TO_COLOR(vec) GX_COLOR_FLOAT_RGBA((vec).x, (vec).y, (vec).z, (vec).w)

//! преобразование GXCOLOR в float4_t с компонентами [0.f, 1.f] RGBA -> XYZW
#define GX_COLOR_COLOR_TO_F4(val) float4_t( \
	(float)(((UINT)(val) >> 16) & 0xFF) / 255.0f, \
	(float)(((UINT)(val) >> 8) & 0xFF) / 255.0f, \
	(float)((UINT)(val) & 0xFF) / 255.0f, \
	(float)(((UINT)(val) >> 24) & 0xFF) / 255.0f \
)

#endif