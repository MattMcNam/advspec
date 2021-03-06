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
inline DWORD FindPattern ( DWORD dwAddress, DWORD dwSize, BYTE* pbSig, const char* szMask )
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
        funcadd = FindPattern( (DWORD) GetHandleOfModule( _T("client") ), 0x2680C6, (PBYTE) "\xA1\x00\x00\x00\x00\x85\xC0\x74\x06\x05", "x????xxxxx" );
        
    typedef IGameResources* (*GGR_t) (void);
    GGR_t GGR = (GGR_t) funcadd;
    return GGR();
}

static void outline_color( const CCommand& args );
static ConCommand outline_color_command("advspec_outline_color", outline_color, "Sets the RGB color of the glow per team");
static void toggle_outlines();
static ConCommand toggle_outlines_command("advspec_toggle_outline", toggle_outlines, "[Deprecated] Toggles glow effect on players");

static void outline_enabled_change(IConVar *var, const char *pOldValue, float flOldValue);
ConVar outline_enabled("advspec_outline_enabled", "0", 0, "Enable glow outline around player models", outline_enabled_change);
ConVar pov_outline_enabled("advspec_pov_outline_enabled", "0", 0, "Forces outlines to stay up-to-date in POV demos by checking every frame, may cause a noticable performance hit!");
ConVar medic_info_enabled("advspec_medic_info_enabled", "0", 0, "Shows which Medigun Medic's are using, and who has charge advantage");
ConVar medic_info_offset_x("advspec_medic_info_offset_x", "5", 0, "How many pixels from the left is the Medic Info box");
ConVar medic_info_offset_y("advspec_medic_info_offset_y", "5", 0, "How many pixels from the top is the Medic Info box");

// CBaseCombatCharacter::GetGlowEffectColor(float &red, float &green, &float &blue);
void (__fastcall *origGetGlowEffectColor)(void* thisptr, int edx, float*, float*, float*);
void (__fastcall *origPaintTraverse)(void* thisptr, int edx, vgui::VPANEL, bool, bool);

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

const char* nameForWeaponID(int wID) {
	switch(wID) {
	case 29:  // Normal
	case 211: // Strange
	case 663: // Festive
	case 796: // MvM
	case 805: // ^
	case 885: // ^
	case 894: // ^
	case 903: // ^
	case 912: // ^
	case 961: // ^
	case 970: // ^
		return "Medi Gun";
	case 35:
		return "Kritzkrieg";
	case 411:
		return "Quick-Fix";
	case 998:
		return "Vaccinator";
	default:
		return "Unknown!";
	}
}

void UpdateEntities() {
	IClientEntityList *entityList = ifaces.GetClientEntityList();

	int iEntCount = entityList->GetHighestEntityIndex();
	IClientEntity *cEntity;

	// Clear medic info
	bluMedic.weaponID = 0;
	redMedic.weaponID = 0;

	for (int i = 0; i < iEntCount; i++) {
		cEntity = entityList->GetClientEntity(i);

		// Ensure valid player entity
		if (cEntity == NULL || !(GetGameResources()->IsConnected(i)))
			continue;

		// Hook GetGlowEffectColor if it hasn't been done
		if (origGetGlowEffectColor == NULL) {
			origGetGlowEffectColor = (void (__fastcall *)(void *, int, float*, float*, float*))
				HookVFunc(*(DWORD**)cEntity, Index_UpdateGlowEffect-1, 
				(DWORD*) &hookedGetGlowEffectColor);
		}

		*MakePtr(bool*, cEntity, WSOffsets::pCTFPlayer__m_bGlowEnabled) = outline_enabled.GetBool();
		CBaseCombatCharacter_UpdateGlowEffect((C_BaseCombatCharacter*)cEntity);

		if (medic_info_enabled.GetBool()) {
			int playerClass = *MakePtr(int*, cEntity, WSOffsets::pCTFPlayer__m_iClass);

			// Medic class index is 5 internally
			if (playerClass == 5) {
				unsigned int index = reinterpret_cast<EHANDLE*>( (char*)cEntity + WSOffsets::pCTFPlayer__m_hMyWeapons)->GetEntryIndex();

				IClientEntity* medigun = entityList->GetClientEntity(index);

				if (medigun) {
					float uber = *MakePtr(float*, medigun, WSOffsets::pCWeaponMedigun__m_flChargeLevel);
					int playerTeam = *MakePtr(int*, cEntity, WSOffsets::pCTFPlayer__m_iTeamNum);
					int gunID = *MakePtr(int*, medigun, WSOffsets::pCWeaponMedigun__m_iItemDefinitionIndex);

					if (playerTeam == 3) {
						bluMedic.weaponID = gunID;
						bluMedic.charge = uber;
					} else {
						redMedic.weaponID = gunID;
						redMedic.charge = uber;
					}
				}
			}
		}
	}
}

#include "medic_info_drawing.h"

vgui::HPanel MatSystemTopPanelHandle = vgui::INVALID_PANEL;
void __fastcall hookedPaintTraverse( vgui::IPanel *thisPtr, int edx,  vgui::VPANEL vguiPanel, bool forceRepaint, bool allowForce = true ) {
	origPaintTraverse(thisPtr, edx, vguiPanel, forceRepaint, allowForce);
	
	if (pov_outline_enabled.GetBool() || medic_info_enabled.GetBool()) {
		UpdateEntities();
	}

	const char* panelName = g_pVGuiPanel->GetName(vguiPanel);
	vgui::HPanel hPanel = g_pVGui->PanelToHandle(vguiPanel);

	if (MatSystemTopPanelHandle == vgui::INVALID_PANEL && (Q_strcmp(panelName, "MatSystemTopPanel") == 0)) {
		MatSystemTopPanelHandle = hPanel;
	}

	if (hPanel == MatSystemTopPanelHandle) {
		IVEngineClient *engineClient = ifaces.GetEngineClient();
		if (engineClient->IsDrawingLoadingImage() || !engineClient->IsInGame( ) || !engineClient->IsConnected() || engineClient->Con_IsVisible( ))
			return;

		if (medic_info_enabled.GetBool() && 
			bluMedic.weaponID > 0 && 
			redMedic.weaponID > 0) 
		{
			g_pVGuiSurface->DrawSetTextFont(m_font);
			g_pVGuiSurface->DrawSetTextColor( 255, 255, 255, 255 );

			int offX = medic_info_offset_x.GetInt();
			int offY = medic_info_offset_y.GetInt();

			int offXs = SCALED(offX);
			int offYs = SCALED(offY);

			// Get font
			if (m_font == 0) {
				vgui::HScheme scheme = g_pVGuiSchemeManager->GetScheme("ClientScheme");
				m_font = g_pVGuiSchemeManager->GetIScheme(scheme)->GetFont(fontName, true);
			}

			// These methods are similar to the chunck of code below, just styled for each HUD
			// User-defined styles will be coming in v1.0
#if defined(MEDIC_OMP)
			DrawMedicOmp(offX, offY, pSurface);
#elif defined(MEDIC_VTV)
			DrawMedicVTV(offX, offY, pSurface);
#elif defined(MEDIC_TFTV)
			DrawMedicTFTV(offX, offY, pSurface);
#elif defined(MEDIC_TOTH)
			DrawMedicTotH(offX, offY, pSurface);
#elif defined(MEDIC_EXTV)
			DrawMedicEXTV(offX, offY, pSurface);
#else

			wchar_t wbuf[1024] = { '\0' };
			g_pVGuiSurface->DrawSetColor(32, 32, 32, 200);
			g_pVGuiSurface->DrawFilledRect(offX, offY, offX + 155, offY + 50);

			g_pVGuiSurface->DrawSetTextColor(88, 133, 162, 255);
			g_pVGuiSurface->DrawSetTextPos(offX + 2, offY + 17);
			swprintf( wbuf, 1024, L"%S", nameForWeaponID(bluMedic.weaponID) );
			g_pVGuiSurface->DrawPrintText(wbuf, wcslen( wbuf ));
			g_pVGuiSurface->DrawSetTextPos(offX + 77, offY + 17);
			swprintf( wbuf, 1024, L"%d%%", Round(bluMedic.charge*100.0f) );
			g_pVGuiSurface->DrawPrintText(wbuf, wcslen( wbuf ));

			g_pVGuiSurface->DrawSetTextColor( 184, 56, 59, 255 );
			g_pVGuiSurface->DrawSetTextPos(offX + 2, offY + 32);
			swprintf( wbuf, 1024, L"%S", nameForWeaponID(redMedic.weaponID) );
			g_pVGuiSurface->DrawPrintText(wbuf, wcslen( wbuf ));
			g_pVGuiSurface->DrawSetTextPos(offX + 77, offY + 32);
			swprintf( wbuf, 1024, L"%d%%", Round(redMedic.charge*100.0f) );
			g_pVGuiSurface->DrawPrintText(wbuf, wcslen( wbuf ));

			int advantage = 0;
			if (bluMedic.charge > redMedic.charge) {
				advantage = Round(bluMedic.charge*100.0f) - Round(redMedic.charge*100.0f);
				g_pVGuiSurface->DrawSetTextPos(offX + 114, offY + 17);
			} else {
				advantage = Round(redMedic.charge*100.0f) - Round(bluMedic.charge*100.0f);
				g_pVGuiSurface->DrawSetTextPos(offX + 114, offY + 32);
			}

			if (advantage > 0) {
				g_pVGuiSurface->DrawSetTextColor( 255, 255, 255, 255 );
				swprintf( wbuf, 1024, L"+%d%%", advantage );
				g_pVGuiSurface->DrawPrintText(wbuf, wcslen( wbuf ));
			}
#endif
		}
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
	ConnectTier1Libraries(&interfaceFactory, 1);
	ConnectTier2Libraries(&interfaceFactory, 1);
	ConnectTier3Libraries(&interfaceFactory, 1);

	if (!ifaces.Init()) {
		Warning("[AS] Unable to get all interfaces, not loading!\n");
		DisconnectTier1Libraries();
		DisconnectTier2Libraries();
		DisconnectTier3Libraries();
		return false;
	}

	m_font = 0;
#if defined(MEDIC_OMP)
	fontName = "FuturaHeavy11";
#elif defined(MEDIC_TOTH)
	fontName = "Bold12";
#elif defined(MEDIC_VTV)
	fontName = "HudGothic3Font";
	m_iTextureRed = pSurface->CreateNewTextureID();
	m_iTextureBlu = pSurface->CreateNewTextureID();
	pSurface->DrawSetTextureFile(m_iTextureRed, "HUD/advspec_med_red", 0, false);
	pSurface->DrawSetTextureFile(m_iTextureBlu, "HUD/advspec_med_blu", 0, false);
#elif defined(MEDIC_TFTV)
	m_healthFont = 0;
	fontName = "TFTV12";
	m_iTextureRed = pSurface->CreateNewTextureID();
	m_iTextureBlu = pSurface->CreateNewTextureID();
	m_iTextureHealth = pSurface->CreateNewTextureID();
	pSurface->DrawSetTextureFile(m_iTextureRed, "HUD/color_panel_red", 0, false);
	pSurface->DrawSetTextureFile(m_iTextureBlu, "HUD/color_panel_blu", 0, false);
	pSurface->DrawSetTextureFile(m_iTextureHealth, "HUD/health_color", 0, false);
#elif defined(MEDIC_EXTV)
	fontName = "Cerbetica14";
	m_healthFont = 0;
	m_advFont = 0;
	m_iTextureFrame = pSurface->CreateNewTextureID();
	m_iTextureHealth = pSurface->CreateNewTextureID();
	pSurface->DrawSetTextureFile(m_iTextureFrame, "vgui/replay/thumbnails/playerpanel-frame-gfx-720-alt2", 0, false);
	pSurface->DrawSetTextureFile(m_iTextureHealth, "HUD/uber_bg", 0, false);
#else
	fontName = "Default";
#endif

	//Hook PaintTraverse
	origPaintTraverse = (void (__fastcall *)(void *, int, vgui::VPANEL, bool, bool))
		HookVFunc(*(DWORD**)g_pVGuiPanel, Index_PaintTraverse, (DWORD*) &hookedPaintTraverse);

	// Get offsets
	WSOffsets::PrepareOffsets();

	// Set colours
	bluColor = new Color( 88, 133, 162, 255);
	redColor = new Color(184,  56,  59, 255);

	// Clear medic info
	bluMedic.weaponID = 0;
	redMedic.weaponID = 0;

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
	DisconnectTier2Libraries();
	DisconnectTier3Libraries();
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
	outline_enabled.SetValue(!outline_enabled.GetBool());
	
	if (outline_enabled.GetBool())
		Msg("[AS] Player outlines enabled\n");
	else
		Msg("[AS] Player outlines disabled\n");

	UpdateEntities();
}

static void outline_enabled_change(IConVar *var, const char *pOldValue, float flOldValue) {
	UpdateEntities();
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

	UpdateEntities();
}
