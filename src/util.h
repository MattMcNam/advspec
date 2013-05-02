#pragma once

#include "sdk.h"
#include "sdk/recvstuff.h"


void* SigScan( void* hmod, const unsigned char* pat, const unsigned char* mask, unsigned int len );

template< unsigned int len >
inline void* SigScan( void* hmod, const char (&pat)[len], const char (&mask)[len] )
{
	return SigScan( hmod, (const unsigned char*)pat, (const unsigned char*)mask, len-1 );
}


RecvProp* NetworkedSearch( RecvTable* table, const char* name );
RecvProp* NetworkedVar( const char* cls, int* offset, const char* const* vars );
#define NETVAR( CLASS, OFFSET, ... ) do { const char* vars[] = { __VA_ARGS__, NULL }; NetworkedVar( CLASS, OFFSET, vars ); } while ( false )
template< typename T > inline T& GetProp( IClientEntity* pEnt, int offset ) { return *reinterpret_cast<T*>( (char*)pEnt + offset ); }

bool GetInformationPointer( void* ptr, char (&buf)[64], unsigned int& off );

int FindPlayerInServer( const char* partial, player_info_t* pinfo );


class NetPropHook
{
public:
	//NetPropHook() : prop(NULL), oldproxy(NULL)
	//{
	//}
	//NetPropHook( RecvProp* prop, RecvVarProxyFn hook )
	//{
	//	Hook( prop, hook );
	//}
	//~NetPropHook()
	//{
	//	if ( prop )
	//		prop->ProxyFn = oldproxy;
	//}

	void Hook( RecvProp* prop, RecvVarProxyFn hook );
	void Unhook();

public:
	RecvProp* prop;
	RecvVarProxyFn oldproxy;
};