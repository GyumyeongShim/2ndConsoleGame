#pragma once
#pragma warning(disable: 4251)
#pragma warning(disable: 4172)
//#define ENGINE_BUILD_DLL 1 // On/Off

#define DllEXPORT   __declspec( dllexport )
#define DllIMPORT   __declspec( dllimport )

#if ENGINE_BUILD_DLL
#define WANNABE_API DllEXPORT
#else
#define  WANNABE_API DllIMPORT
#endif
