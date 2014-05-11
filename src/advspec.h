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

#include "tier3/tier3.h"
#include "engine/iserverplugin.h"
#include "igameevents.h"

#include "ifaces.h"
#include "offsets.h"
#include "vfuncs.h"
#include "icvar.h"

#define PLUGIN_DESC "Advanced Spectator Plugin b4.3"
#define SHORT_DESC "AdvSpec b4.3"

// Custom medic info displays for various orgs
// All require medic_info_drawing.h
//#define MEDIC_VTV		// VTV - requires full hud
//#define MEDIC_OMP		// BOTV - requires font
//#define MEDIC_TFTV  	// TFTV - requires full hud
//#define MEDIC_EXTV  	// EXTV - requires full hud
//#define MEDIC_TOTH  	// TOTH - requires font

#define SCALED(normalValue) g_pVGuiSchemeManager->GetProportionalScaledValue(normalValue)

#include "cbase.h"
#include "cdll_int.h"
#include "icliententitylist.h"
#include "icliententity.h"
#include "convar.h"
#include "edict.h"
#include "igameresources.h"
#include "igameevents.h"
#include "vgui/IPanel.h"
#include "vgui/ISurface.h"
#include "vgui/IScheme.h"
#include "vgui/IVgui.h"
#include "ehandle.h"

//class CBaseCombatCharacter;
class C_BaseCombatWeapon;
typedef CHandle<C_BaseEntity> EHANDLE;

vgui::HFont m_font;

#if defined(MEDIC_VTV) || defined(MEDIC_TFTV)
int m_iTextureBlu;
int m_iTextureRed;
#endif

#if defined(MEDIC_TFTV)
vgui::HFont m_healthFont;
int m_iTextureHealth;
#endif

#if defined(MEDIC_TOTH)
vgui::HFont m_healthFont;
#endif

#if defined(MEDIC_EXTV)
vgui::HFont m_healthFont;
vgui::HFont m_advFont;
int m_iTextureFrame;
int m_iTextureHealth;
#endif

char *fontName;

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
