/*
 *  vfuncs.cpp
 *  WebSpec project
 *  
 *  Copyright (c) 2013 Matthew McNamara
 *  BSD 2-Clause License
 *  http://opensource.org/licenses/BSD-2-Clause
 *
 */

#include "vfuncs.h"

void CBaseCombatCharacter_UpdateGlowEffect(C_BaseCombatCharacter *pThisPtr) {
	//Magic from SourceMod
	void **this_ptr = *(void ***)&pThisPtr;
	void **vtable = *(void ***)pThisPtr;
	void *func = vtable[Index_UpdateGlowEffect];

	union {
		void (WSEmptyClass::*mfpnew)();
#if !defined(_LINUX) && !defined(_OSX)
		void *addr;
	} u;
	u.addr = func;
#else
		struct {
			void *addr;
			intptr_t adjustor;
		} s;
	} u;
	u.s.addr = func;
	u.s.adjustor = 0;
#endif

	(void) (reinterpret_cast<WSEmptyClass*>(this_ptr)->*u.mfpnew)();
}

DWORD *HookVFunc(DWORD *vtable, int index, DWORD *newFunction)
{
	#ifndef _POSIX
	DWORD dwOldProt;
	#endif
	DWORD *oldFunc;
	
	#ifdef _POSIX
	mprotect(&vtable[index], 4, PROT_READ|PROT_WRITE|PROT_EXEC);
	#else
	VirtualProtect(&vtable[index], 4, PAGE_EXECUTE_READWRITE, &dwOldProt);
	#endif
	oldFunc=(DWORD*)vtable[index];
	vtable[index]=(DWORD)newFunction;
	#ifndef _POSIX //TODO: Restore mem protection on posix
	VirtualProtect(&vtable[index], 4, dwOldProt, &dwOldProt);
	#endif
	
	return oldFunc;
}