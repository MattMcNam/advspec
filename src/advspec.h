/*
 *  advspec.h
 *  AdvSpec project
 *  
 *  Copyright (c) 2013 Matthew McNamara
 *  BSD 2-Clause License
 *  http://opensource.org/licenses/BSD-2-Clause
 *
 */

#ifndef ADVSPEC_H
#define ADVSPEC_H

#define CLIENT_DLL

#include "stdafx.h"

#include "tier1/tier1.h"
#include "engine/iserverplugin.h"
#include "igameevents.h"

#include "offsets.h"
#include "vfuncs.h"
#include "icvar.h"

#define PLUGIN_DESC "Advanced Spectator Plugin b4.1"
#define SHORT_DESC "AdvSpec b4.1"

#include "cbase.h"
#include "cdll_int.h"
#include "icliententitylist.h"
#include "icliententity.h"
#include "convar.h"
#include "edict.h"
#include "igameresources.h"
#include "igameevents.h"
#include "vgui\IPanel.h"
#include "vgui\ISurface.h"
#include "ehandle.h"

//class CBaseCombatCharacter;
class C_BaseCombatWeapon;
typedef unsigned int VPANEL;
typedef CHandle<C_BaseEntity> EHANDLE;

// client.dll
IBaseClientDLL* pClient;
IClientEntityList* pEntityList;
CBaseEntityList* g_pEntityList;

// engine.dll
IVEngineClient *pEngineClient;

// vgui2.dll
vgui::IPanel* pPanel;
vgui::ISurface* pSurface;
vgui::HFont m_font;

Color *bluColor;
Color *redColor;

struct medic_info_t {
	int weaponID;
	float charge;
};
medic_info_t bluMedic;
medic_info_t redMedic;

#define Round(x) ((x)>=0?(int)((x)+0.5):(int)((x)-0.5))

class AdvSpecPlugin: public IServerPluginCallbacks, public IGameEventListener
{
public:
	AdvSpecPlugin();
	~AdvSpecPlugin();

	// IServerPluginCallbacks methods
	virtual bool			Load(	CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory );
	virtual void			Unload( void );
	virtual void			Pause( void );
	virtual void			UnPause( void );
	virtual const char     *GetPluginDescription( void );      
	virtual void			LevelInit( char const *pMapName );
	virtual void			ServerActivate( edict_t *pEdictList, int edictCount, int clientMax );
	virtual void			GameFrame( bool simulating );
	virtual void			LevelShutdown( void );
	virtual void			ClientActive( edict_t *pEntity );
	virtual void			ClientDisconnect( edict_t *pEntity );
	virtual void			ClientPutInServer( edict_t *pEntity, char const *playername );
	virtual void			SetCommandClient( int index );
	virtual void			ClientSettingsChanged( edict_t *pEdict );
	virtual PLUGIN_RESULT	ClientConnect( bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen );
	virtual PLUGIN_RESULT	ClientCommand( edict_t *pEntity, const CCommand &args );
	virtual PLUGIN_RESULT	NetworkIDValidated( const char *pszUserName, const char *pszNetworkID );
	virtual void			OnQueryCvarValueFinished( QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue );
	virtual void			OnEdictAllocated( edict_t *edict );
	virtual void			OnEdictFreed( const edict_t *edict );

	// IGameEventListener Interface
	virtual void FireGameEvent( KeyValues * event );
};

#endif
