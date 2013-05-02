
#include "stdafx.h"
#include "plugin.h"
#include "util.h"
#include "ifaces.h"

#include "outlines.h"

extern "C" char __ImageBase;

void* CreateInterface( const char* name, int* code )
{
	// Force ourselves to remain loaded
	//static bool skip = false;
	//if ( !skip )
	//{
	//	skip = true;
	//	char path[MAX_PATH];
	//	::GetModuleFileNameA( (HMODULE) &__ImageBase, path, sizeof(path) );
	//	::LoadLibraryA( path );
	//}

	// Check which interface the game is requesting from us
	void* i = NULL;
	if ( !strcmp( name, "ISERVERPLUGINCALLBACKS003" ) )
		i = &CAdvSpec::g;
	return i;
}

inline ConVar* UnlockVar( const char* name )
{
	ConVar* var = Ifaces.pCvar->FindVar( name );
	if ( var )
	{
		// WARNING!
		// Servers can query the values of your cvars (if you don't hide them)
		// This allows them to detect if you've changed cvars you normally shouldn't be able to
		// Ex. you can remove the cheat flag from r_drawothermodels and have a primitive wallhack
		//     but you're gonna get banned pretty quickly when a server has a competent anti-cheat
		var->bHasMax = false;
		var->bHasMin = false;
		var->nFlags &= ~FCVAR_CHEAT;
	}
	return var;
}

inline void CleanCommand( const char* name )
{
	if ( ConCommand* cmd = Ifaces.pCvar->FindCommand( name ) )
	{
		cmd->nFlags &= ~FCVAR_CHEAT;
	}
}

void PluginInit() {
	Msg( "Loading %s...\n", SHORT_DESC );

	if ( !COutlines::g.Init() )
	{
		Msg( "Outlines::Init() Failed!\n" );
	}
}

#ifdef USE_FANCY_CPP_FEATURES
void DelayPluginLoad() {
	while (GetHandleOfModule( "bin/ServerBrowser" ) == NULL ) {
		std::chrono::milliseconds sleeptime(500);
		std::this_thread::sleep_for(sleeptime);
	}

	PluginInit();
}
#endif

CAdvSpec CAdvSpec::g;

bool CAdvSpec::Load( CreateInterfaceFn pfnAppSystem, CreateInterfaceFn pfnServer )
{
	// Start by initing the ifaces
	if ( !Ifaces.Init(pfnAppSystem) )
		return false;

#ifdef USE_FANCY_CPP_FEATURES
	std::thread delay_thread(DelayPluginLoad);
	delay_thread.detach();
#else
	PluginInit();
#endif

	return true;
}
void CAdvSpec::Unload( void )
{
	COutlines::g.Close();
}

const char* CAdvSpec::GetPluginDescription( void )
{
	return "Advanced Spectator Plugin b2.5 (freak u valve)";
}

// Blahblahblah not interested

void CAdvSpec::Pause( void ) { }
void CAdvSpec::UnPause( void ) { }
void CAdvSpec::LevelInit( char const *pMapName ) { }
void CAdvSpec::ServerActivate( void *pEdictList, int edictCount, int clientMax ) { }
void CAdvSpec::GameFrame( bool simulating ) { }
void CAdvSpec::LevelShutdown( void ) { }
void CAdvSpec::ClientActive( void *pEntity ) { }
void CAdvSpec::ClientDisconnect( void *pEntity ) { }
void CAdvSpec::ClientPutInServer( void *pEntity, char const *playername ) { }
void CAdvSpec::SetCommandClient( int index ) { }
void CAdvSpec::ClientSettingsChanged( void *pEdict ) { }
int  CAdvSpec::ClientConnect( bool *bAllowConnect, void *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen ) { return 0; }
int  CAdvSpec::ClientCommand( void *pEntity, const void *args ) { return 0; }
int  CAdvSpec::NetworkIDValidated( const char *pszUserName, const char *pszNetworkID ) { return 0; }
void CAdvSpec::OnQueryCvarValueFinished( int iCookie, void *pPlayerEntity, int eStatus, const char *pCvarName, const char *pCvarValue ) { }
void CAdvSpec::OnEdictAllocated( void *edict ) { }
void CAdvSpec::OnEdictFreed( const void *edict ) { }
