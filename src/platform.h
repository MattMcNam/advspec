#pragma once

// Make things work out smoothly between windows & linux


// Assume thingy
#ifdef NDEBUG
#ifndef _LINUX
#define ASSUME(e) __assume(e)
#else
#define ASSUME(e) do { if(!(e)) __builtin_unreachable(); } while(0)
#endif
#else
#define ASSUME(e) assert(e)
#endif // NDEBUG


// Cross platform VMT hooks helpers
// Under windows virtual functions usually use __thiscall (same as __stdcall except this is passed in ecx),
//  we abuse __fastcall (explicitly pass args in ecx and edx) to hook these functions under windows.
// Under linux virtual functions are simple __stdcalls.

#ifdef _LINUX
#define __fastcall
#define __thiscall
#define THIS_PARAM( x ) x
#else
#define THIS_PARAM( x ) x, int edx
#endif


// Because not everyone has a compiler with fancy C++11 features...
// DEFINE ME IN COMPILER PREPROCESSOR DEFINITIONS!!
//
// This WILL make the plugin fail to load when using a .vdf to autoload the plugin on launch,
//  load it manually using plugin_load in your autoexec to get around this.

//#define USE_FANCY_CPP_FEATURES


// Here or in util.h?
#ifdef _LINUX
#define GetFuncAddress(pAddress, szFunction) dlsym(pAddress, szFunction)
#define GetHandleOfModule(szModuleName) dlopen(szModuleName".so", RTLD_NOLOAD)
#else
#define GetFuncAddress(pAddress, szFunction) ::GetProcAddress((HMODULE)pAddress, szFunction)
#define GetHandleOfModule(szModuleName) GetModuleHandleA(szModuleName".dll")
#endif


// Exporting functions

#ifndef _LINUX
#define OPAPI extern "C" __declspec(dllexport)
#else
#define OPAPI extern "C" __attribute__ ((visibility("default")))
#endif