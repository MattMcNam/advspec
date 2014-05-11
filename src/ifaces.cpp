/**
 *  ifaces.cpp
 *  AdvSpec project
 *  
 *  Copyright (c) 2014 Matthew McNamara
 *  BSD 2-Clause License
 *  http://opensource.org/licenses/BSD-2-Clause
 *
 */

#include "ifaces.h"

#ifdef _POSIX
#define GetFuncAddress(pAddress, szFunction) dlsym(pAddress, szFunction)
#define GetHandleOfModule(szModuleName) dlopen(szModuleName".so", RTLD_NOLOAD)
#else
#define GetFuncAddress(pAddress, szFunction) ::GetProcAddress((HMODULE)pAddress, szFunction)
#define GetHandleOfModule(szModuleName) GetModuleHandleA(szModuleName".dll")
#endif

#define CheckPointerAndWarn(pPointer, className) \
	if (pPointer == nullptr) { \
		Warning("[AS] %s is NULL!\n", #className); \
		return false; \
	}

SrcIfaces ifaces;

bool SrcIfaces::Init() {
	void* hmClient = GetHandleOfModule("client");
	CreateInterfaceFn pfnClient = (CreateInterfaceFn) GetFuncAddress(hmClient, "CreateInterface");
	this->pClientDll = (IBaseClientDLL*) pfnClient("VClient017", NULL);
	this->pClientEntityList = (IClientEntityList*) pfnClient("VClientEntityList003", NULL);

	void* hmEngine = GetHandleOfModule("engine");
	CreateInterfaceFn pfnEngine = (CreateInterfaceFn) GetFuncAddress(hmEngine, "CreateInterface");
	this->pEngineClient = (IVEngineClient*) pfnEngine("VEngineClient013", NULL);

	CheckPointerAndWarn(this->pClientDll, IBaseClientDLL);
	CheckPointerAndWarn(this->pClientEntityList, IClientEntityList);
	CheckPointerAndWarn(this->pEngineClient, IVEngineClient);
	
	return true;
}

IBaseClientDLL* SrcIfaces::GetBaseClientDLL() {
	return this->pClientDll;
}

IClientEntityList* SrcIfaces::GetClientEntityList() {
	return this->pClientEntityList;
}

IVEngineClient* SrcIfaces::GetEngineClient() {
	return this->pEngineClient;
}
