/*
 *  advspec.cpp
 *  AdvSpec project
 *  
 *  Copyright (c) 2013 Matthew McNamara
 *  BSD 2-Clause License
 *  http://opensource.org/licenses/BSD-2-Clause
 *
 */

#include "advspec.h"

inline bool DataCompare( const BYTE* pData, const BYTE* bSig, const char* szMask )
{
    for( ; *szMask; ++szMask, ++pData, ++bSig)
    {
        if( *szMask == 'x' && *pData != *bSig)
            return false;
    }
	
    return ( *szMask ) == NULL;
}

// Finds a pattern at the specified address
inline DWORD FindPattern ( DWORD dwAddress, DWORD dwSize, BYTE* pbSig, char* szMask )
{
    for( DWORD i = NULL; i < dwSize; i++ )
    {
        if( DataCompare( (BYTE*) ( dwAddress + i ), pbSig, szMask ) )
            return (DWORD)( dwAddress + i );
    }
    return 0;
}

IGameResources* GetGameResources() {
	//IGameResources* res;
    static DWORD funcadd = NULL;
    if( !funcadd )
        funcadd = FindPattern( (DWORD) GetModuleHandle( _T("client.dll") ), 0x2680C6, (PBYTE) "\xA1\x00\x00\x00\x00\x85\xC0\x74\x06\x05", "x????xxxxx" );
        
    typedef IGameResources* (*GGR_t) (void);
    GGR_t GGR = (GGR_t) funcadd;
    return GGR();
}

static void outline_color( const CCommand& args );
static ConCommand outline_color_command("advspec_outline_color", outline_color, "Sets the RGB color of the glow per team");
static void toggle_outlines();
static ConCommand toggle_outlines_command("advspec_toggle_outline", toggle_outlines, "Toggles glow effect on players");

// Not implemented yet
ConVar pov_outlines_hack("advspec_pov_outlines_hack", "0", 0, "Forces outlines to stay up-to-date in PoV demos by checking every frame, may cause a noticable performance hit!");

// CBaseCombatCharacter::GetGlowEffectColor(float &red, float &green, &float &blue);
void (__fastcall *origGetGlowEffectColor)(void* thisptr, int edx, float*, float*, float*);

void __fastcall hookedGetGlowEffectColor( CBaseCombatCharacter *thisPtr, int edx, float &red, float &green, float &blue ) {
	int team = *MakePtr(int*, thisPtr, WSOffsets::pCTFPlayer__m_iTeamNum);

	if (team == 2) {
		red   = float(redColor->r() / 255.0);
		green = float(redColor->g() / 255.0);
		blue  = float(redColor->b() / 255.0);
	} else if (team == 3) {
		red   = float(bluColor->r() / 255.0);
		green = float(bluColor->g() / 255.0);
		blue  = float(bluColor->b() / 255.0);
	} else {
		origGetGlowEffectColor(thisPtr, edx, &red, &green, &blue);
	}
}

void SetGlow() {
	int iEntCount = pEntityList->GetHighestEntityIndex();
	IClientEntity *cEntity;

	for (int i = 0; i < iEntCount; i++) {
		cEntity = pEntityList->GetClientEntity(i);

		// Ensure valid player entity
		if (cEntity == NULL || !(GetGameResources()->IsConnected(i)))
			continue;

		//Hook GetGlowEffectColor if it hasn't been done
		//TODO: Learn Casual's VMTHooks class
		if (origGetGlowEffectColor == NULL) {
			origGetGlowEffectColor = (void (__fastcall *)(void *, int, float*, float*, float*))
				HookVFunc(*(DWORD**)cEntity, Index_CBaseCombatCharacter_UpdateGlowEffect-1, 
				(DWORD*) &hookedGetGlowEffectColor);
		}

		*MakePtr(bool*, cEntity, WSOffsets::pCTFPlayer__m_bGlowEnabled) = glow_enabled;
		CBaseCombatCharacter_UpdateGlowEffect((CBaseCombatCharacter*)cEntity);
	}
}

// The plugin is a static singleton that is exported as an interface
AdvSpecPlugin g_AdvSpecPlugin;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(AdvSpecPlugin, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS, g_AdvSpecPlugin );

AdvSpecPlugin::AdvSpecPlugin()
{
}

AdvSpecPlugin::~AdvSpecPlugin()
{
}

bool AdvSpecPlugin::Load( CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory )
{
	ConnectTier1Libraries( &interfaceFactory, 1 );

	void* hmClient = ::GetModuleHandleA( "client.dll" );
	CreateInterfaceFn pfnClient = (CreateInterfaceFn) GetFuncAddress( hmClient, "CreateInterface" );
	pClient			= (IBaseClientDLL*)		pfnClient( "VClient017",			NULL );
	pEntityList		= (IClientEntityList*)	pfnClient( "VClientEntityList003",	NULL );

	WSOffsets::PrepareOffsets();

	bluColor = new Color( 88, 133, 162, 255);
	redColor = new Color(184,  56,  59, 255);

	//Everything seems ok!
	Msg("%s loaded!\n", PLUGIN_DESC);

	//Register cvars
	ConVar_Register( 0 );
	return true;
}

void AdvSpecPlugin::Unload( void )
{
	ConVar_Unregister( );
	DisconnectTier1Libraries();
}

void AdvSpecPlugin::FireGameEvent( KeyValues * event ) {}
void AdvSpecPlugin::Pause( void ){}
void AdvSpecPlugin::UnPause( void ){}
const char *AdvSpecPlugin::GetPluginDescription( void ){return PLUGIN_DESC;}
void AdvSpecPlugin::LevelInit( char const *pMapName ){}
void AdvSpecPlugin::ServerActivate( edict_t *pEdictList, int edictCount, int clientMax ){}
void AdvSpecPlugin::GameFrame( bool simulating ){}
void AdvSpecPlugin::LevelShutdown( void ){}
void AdvSpecPlugin::ClientActive( edict_t *pEntity ){}
void AdvSpecPlugin::ClientDisconnect( edict_t *pEntity ){}
void AdvSpecPlugin::ClientPutInServer( edict_t *pEntity, char const *playername ){}
void AdvSpecPlugin::SetCommandClient( int index ){}
void AdvSpecPlugin::ClientSettingsChanged( edict_t *pEdict ){}
PLUGIN_RESULT AdvSpecPlugin::ClientConnect( bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen ){return PLUGIN_CONTINUE;}
PLUGIN_RESULT AdvSpecPlugin::ClientCommand( edict_t *pEntity, const CCommand &args ){return PLUGIN_CONTINUE;}
PLUGIN_RESULT AdvSpecPlugin::NetworkIDValidated( const char *pszUserName, const char *pszNetworkID ){return PLUGIN_CONTINUE;}
void AdvSpecPlugin::OnQueryCvarValueFinished( QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue ){}
void AdvSpecPlugin::OnEdictAllocated( edict_t *edict ){}
void AdvSpecPlugin::OnEdictFreed( const edict_t *edict ){}

static void toggle_outlines() {
	glow_enabled = !glow_enabled;
	
	if (glow_enabled)
		Msg("[AS] Player outlines enabled\n");
	else
		Msg("[AS] Player outlines disabled\n");

	SetGlow();
}

__inline void PrintOutlineColorCommandUsage() {
	Msg("Usage: advspec_outline_color [r|b] [0-255] [0-255] [0-255]\n       Color values in Red, Green, Blue order\n");
}

static void outline_color( const CCommand& args )
{
	if (args.ArgC() < 5) {
		PrintOutlineColorCommandUsage();
		return;
	}

	char team = ((char *)args.Arg(1))[0];
	if (team != 'r' && team != 'b') {
		Msg("Invalid team '%c'\n", team);
		PrintOutlineColorCommandUsage();
		return;
	}

	int red = atoi(args.Arg(2));
	int green = atoi(args.Arg(3));
	int blue = atoi(args.Arg(4));
	if (red < 0 || red > 255) {
		Msg("Invalid red value '%d'\n", red);
		PrintOutlineColorCommandUsage();
		return;
	}

	if (green < 0 || green > 255) {
		Msg("Invalid green value '%d'\n", green);
		PrintOutlineColorCommandUsage();
		return;
	}

	if (blue < 0 || blue > 255) {
		Msg("Invalid blue value '%d'\n", blue);
		PrintOutlineColorCommandUsage();
		return;
	}

	if (team == 'r')
		redColor->SetColor(red, green, blue, 255);
	else
		bluColor->SetColor(red, green, blue, 255);

	SetGlow();
}
