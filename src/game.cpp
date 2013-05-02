#include "stdafx.h"

#include "game.h"
#include "util.h"
#include "ifaces.h"
#include "sdk/globalvars.h"
#include "sdk/const.h"


#define NETVARX( OFF, NAME, ... ) static int OFF = 0; if ( !OFF ) { static const char* const vars[] = { __VA_ARGS__, NULL }; if ( !NetworkedVar( NAME, &OFF, vars ) ) __debugbreak(); }

int GetObserverTarget( IClientEntity* pl )
{
	NETVARX( off, "CBasePlayer", "m_hObserverTarget" );

	int h = *(int*)( ((char*)pl) + off );
	if ( h!=-1 )
	{
		h = h&0xFFF;
	}
	return h;
}

int& PlayerResource::iPlayerClass( int pl )
{
	NETVARX( off, "CPlayerResource", "m_iPlayerClass" );
	int* a = (int*)( (char*)this + off );
	return a[pl];
}

bool TFPlayerResource::bConnected( int pl )
{
	if ( pl>0 && pl<=Ifaces.pGlobals->maxClients )
	{
		NETVARX( off, "CTFPlayerResource", "m_bConnected" );
		bool* b = (bool*)( (char*)this + off );
		return b[pl];
	}
	return false;
}
int& TFPlayerResource::iTeam( int pl )
{
	NETVARX( off, "CTFPlayerResource", "m_iTeam" );
	int* a = (int*)( (char*)this + off );
	return a[pl];
}
bool& TFPlayerResource::bAlive( int pl )
{
	NETVARX( off, "CTFPlayerResource", "m_bAlive" );
	bool* b = (bool*)( (char*)this + off );
	return b[pl];
}
int& TFPlayerResource::iHealth( int pl )
{
	NETVARX( off, "CTFPlayerResource", "m_iHealth" );
	int* a = (int*)( (char*)this + off );
	return a[pl];
}

#undef NETVARX
