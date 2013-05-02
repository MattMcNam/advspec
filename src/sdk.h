#pragma once

#include "sdk/icommandline.h"


class ICvar;
class IBaseClientDLL;
class IClientNetworkable;



typedef void* (* CreateInterfaceFn)( const char* name, int* code );


struct RecvTable;
struct RecvProp;
struct CRecvDecoder;
struct ClientClass;
struct DVariant;

struct player_info_t;

class IClientEntity;



//---- tier0.dll imports ----
#ifndef _LINUX
extern "C" __declspec(dllimport)
	void __cdecl Msg( const char* fmt, ... );
extern "C" __declspec(dllimport)
	ICommandLine*  __cdecl CommandLine_Tier0();
#else
extern "C" void Msg( const char* fmt, ... );
extern "C" ICommandLine *CommandLine_Tier0();
#endif

//---- Interfaces ----

class IAppSystem
{
public:
	// Here's where the app systems get to learn about each other 
	virtual bool Connect( CreateInterfaceFn factory ) = 0;
	virtual void Disconnect() = 0;

	// Here's where systems can access other interfaces implemented by this object
	// Returns NULL if it doesn't implement the requested interface
	virtual void *QueryInterface( const char *pInterfaceName ) = 0;

	// Init, shutdown
	virtual int Init() = 0;
	virtual void Shutdown() = 0;
};

class ConCommandBase;
class ConVar;
class ConCommand;
class IConVar;
struct CCmdArgs;

#include "sdk/icliententitylist.h"
#include "sdk/recvstuff.h"
#include "sdk/convar.h"
#include "sdk/icvar.h"
#include "sdk/isurface.h"