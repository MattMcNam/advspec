/**
 *  ifaces.h
 *  AdvSpec project
 *  
 *  Copyright (c) 2014 Matthew McNamara
 *  BSD 2-Clause License
 *  http://opensource.org/licenses/BSD-2-Clause
 *
 */

#pragma once

#if defined(WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#define CLIENT_DLL

#include "tier1/tier1.h"
#include "cdll_int.h"
#include "icliententitylist.h"

class SrcIfaces {
public:
	bool Init();

	IBaseClientDLL* GetBaseClientDLL();
	IClientEntityList* GetClientEntityList();

	IVEngineClient* GetEngineClient();
private:
	IBaseClientDLL *pClientDll;
	IClientEntityList *pClientEntityList;

	IVEngineClient *pEngineClient;
};

extern SrcIfaces ifaces;
