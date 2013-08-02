/*
 *  vfuncs.h
 *  WebSpec project
 *  
 *  Copyright (c) 2013 Matthew McNamara
 *  BSD 2-Clause License
 *  http://opensource.org/licenses/BSD-2-Clause
 *
 */

#ifndef VFUNCS_H
#define VFUNCS_H

#include "tier0/platform.h"

class WSEmptyClass {};

class CBaseCombatCharacter;

#if defined(_LINUX) || defined(_OSX)
#define Index_CBaseCombatCharacter_UpdateGlowEffect 281 //Out of date like woah
#else
#define Index_CBaseCombatCharacter_UpdateGlowEffect 221 //219
#endif

extern void CBaseCombatCharacter_UpdateGlowEffect(CBaseCombatCharacter *pThisPtr);

#if !defined( _POSIX )
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <sys/mman.h>
//typedef unsigned long DWORD;
#endif

extern DWORD *HookVFunc(DWORD *vtable, int index, DWORD *newFunction);

#endif
