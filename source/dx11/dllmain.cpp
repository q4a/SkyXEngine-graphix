// dllmain.cpp : Defines the entry point for the DLL application.

#include "GXContext.h"

#if defined(_WINDOWS)
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#	if defined(GXGAPIDX11_EXPORTS)
#		define GXGAPI __declspec(dllexport)
#	else
#		define GXGAPI __declspec(dllimport)
#	endif
#else
#	define DSGAPI
#endif
#define C extern "C"

GX_ENABLE_HIGH_PERFORMANCE_DUAL_GPU()

#if defined(_WINDOWS)
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch(ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//SetHandlers();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
#endif

C GXGAPI IGXDevice* GetInstance()
{
	return(new CGXDevice());
}

