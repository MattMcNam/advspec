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

#if defined(DEBUG)
static void scaled_value(const CCommand& args);
static ConCommand scaled_value_command("advspec_dbg_scaled_value", scaled_value, "Prints scaled value");
#endif

static void reload_overlay(const CCommand& args);
static ConCommand reload_overlay_cmd("advspec_reload_overlay", reload_overlay, "Reloads the UI overlay");

static void outline_enabled_change(IConVar *var, const char *pOldValue, float flOldValue);
ConVar outline_enabled("advspec_outline_enabled", "0", 0, "Enable glow outline around player models", outline_enabled_change);
ConVar pov_outline_enabled("advspec_pov_outline_enabled", "0", 0, "Forces outlines to stay up-to-date in POV demos by checking every frame, may cause a noticable performance hit!");
ConVar medic_info_enabled("advspec_medic_info_enabled", "0", 0, "Shows which Medigun Medic's are using, and who has charge advantage");
ConVar medic_info_offset_x("advspec_medic_info_offset_x", "0", 0, "How many pixels from the left is the Medic Info box");
ConVar medic_info_offset_y("advspec_medic_info_offset_y", "230", 0, "How many pixels from the top is the Medic Info box");

// CBaseCombatCharacter::GetGlowEffectColor(float &red, float &green, &float &blue);
void (__fastcall *origGetGlowEffectColor)(void* thisptr, int edx, float*, float*, float*);
void (__fastcall *origPaintTraverse)(void* thisptr, int edx, VPANEL, bool, bool);

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
	int iEntCount = pEntityList->GetHighestEntityIndex();
	IClientEntity *cEntity;

	// Clear medic info
	bluMedic.weaponID = 0;
	redMedic.weaponID = 0;

	for (int i = 0; i < iEntCount; i++) {
		cEntity = pEntityList->GetClientEntity(i);

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

				IClientEntity* medigun = pEntityList->GetClientEntity(index);

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

#if defined(MEDIC_OMP) || defined(MEDIC_TFTV) || defined(MEDIC_VTV)
#include "medic_private.h"
#endif

void __fastcall hookedPaintTraverse( vgui::IPanel *thisPtr, int edx,  VPANEL vguiPanel, bool forceRepaint, bool allowForce = true ) {
	// Let TF2 draw it's stuff first, then AdvSpec's overlay
	origPaintTraverse(thisPtr, edx, vguiPanel, forceRepaint, allowForce);

	if (pov_outline_enabled.GetBool() || medic_info_enabled.GetBool()) {
		UpdateEntities();
	}

	const char* panelName = pPanel->GetName(vguiPanel);
	if (panelName[0] == 'M' && panelName[3] == 'S' &&
		panelName[9] == 'T' && panelName[12] == 'P')
	{
		// TODO: string search only once then store panel id/address/something
		if (pEngineClient->IsDrawingLoadingImage() || !pEngineClient->IsInGame( ) || !pEngineClient->IsConnected() || pEngineClient->Con_IsVisible( ))
			return;

		if (medic_info_enabled.GetBool() /*&& 
			bluMedic.weaponID > 0 && 
			redMedic.weaponID > 0*/) 
		{
			m_WebCore->Update();
			m_BitmapSurface = (Awesomium::BitmapSurface*)m_WebView->surface();
			if (m_BitmapSurface && m_iNearestPowerWidth + m_iNearestPowerHeight > 0) {
				// Need to convert Awesomium BGRA to RGBA, so make a temp buffer
				const int bytesPerPixel = 4; // TODO: Define in header
				unsigned char* buffer = new unsigned char[m_iNearestPowerWidth * m_iNearestPowerHeight * bytesPerPixel];
				m_BitmapSurface->CopyTo(buffer, m_BitmapSurface->width() * bytesPerPixel, bytesPerPixel, true, false);

				pSurface->DrawSetTextureRGBA(m_iAwesomiumTextureId, buffer, m_BitmapSurface->width(), m_BitmapSurface->height(), true, true);

				pSurface->DrawSetTexture(m_iAwesomiumTextureId);
				pSurface->DrawSetColor(255, 255, 255, 255);
				float scalerW = float(m_BitmapSurface->width()) / float(m_iNearestPowerWidth);
				float scalerT = float(m_BitmapSurface->height()) / float(m_iNearestPowerHeight);
				pSurface->DrawTexturedSubRect(0, 0, m_BitmapSurface->width(), m_BitmapSurface->height(), 0.0f, 0.0f, scalerW, scalerT);
				
				// Cleanup RGBA conversion
				delete buffer;
			}
		}
	}
}

int NearestPowerOfTwo(int v)
{
        // http://stackoverflow.com/questions/466204/rounding-off-to-nearest-power-of-2
        v--;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        v++;
        return v;
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

	void* hmClient = GetHandleOfModule("client");
	CreateInterfaceFn pfnClient = (CreateInterfaceFn) GetFuncAddress(hmClient, "CreateInterface");
	pClient     = (IBaseClientDLL*)    pfnClient("VClient017", NULL);
	pEntityList = (IClientEntityList*) pfnClient("VClientEntityList003", NULL);

	void* hmEngine = GetHandleOfModule("engine");
	CreateInterfaceFn pfnEngine = (CreateInterfaceFn) GetFuncAddress(hmEngine, "CreateInterface");
	pEngineClient = (IVEngineClient*) pfnEngine("VEngineClient013", NULL);

	void* hmVGUI2          = GetHandleOfModule("vgui2");
	void* hmVGUIMatSurface = GetHandleOfModule("vguimatsurface");
	CreateInterfaceFn pfnVGUI2          = (CreateInterfaceFn) GetFuncAddress(hmVGUI2, "CreateInterface");
	CreateInterfaceFn pfnVGUIMatSurface = (CreateInterfaceFn) GetFuncAddress(hmVGUIMatSurface, "CreateInterface");

	pPanel   = (vgui::IPanel*)   pfnVGUI2("VGUI_Panel009", NULL);
	pSurface = (vgui::ISurface*) pfnVGUIMatSurface("VGUI_Surface030", NULL);

	// Set screenWidth & height, TODO: handle resize
	pSurface->GetScreenSize(screenWidth, screenHeight);

	m_iAwesomiumTextureId = pSurface->CreateNewTextureID(true);
	Awesomium::WebConfig config;

	//TODO: Fix my DEBUG ifdef :/
	/*config.remote_debugging_port = 9090;
	config.remote_debugging_host = WSLit("127.0.0.1");*/

	m_WebCore = Awesomium::WebCore::Initialize(config);
	m_WebView = m_WebCore->CreateWebView(screenWidth, screenHeight);
	m_WebView->SetTransparent(true);

	m_WebView->LoadURL(Awesomium::WebURL(Awesomium::WSLit("http://mattmcn.com/omp.html")));

	Awesomium::JSValue jsObjResult = m_WebView->CreateGlobalJavascriptObject(Awesomium::WSLit("advspec"));
	if (jsObjResult.IsObject()) {
		Awesomium::JSObject& advspecObj = jsObjResult.ToObject();
		m_MethodDispatcher.BindWithRetval(advspecObj, Awesomium::WSLit("getMedicInfo"), JSDelegateWithRetval(this, &AdvSpecPlugin::OnJSGetMedicInfo));
	}

	m_WebView->set_js_method_handler(&m_MethodDispatcher);

	m_iNearestPowerWidth = NearestPowerOfTwo(screenWidth);
	m_iNearestPowerHeight = NearestPowerOfTwo(screenHeight);

	//Hook PaintTraverse
	origPaintTraverse = (void (__fastcall *)(void *, int, VPANEL, bool, bool))
		HookVFunc(*(DWORD**)pPanel, Index_PaintTraverse, (DWORD*) &hookedPaintTraverse);

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
	m_WebView->Destroy();
	Awesomium::WebCore::Shutdown();

	//TODO: clean unhooking

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

JSValue AdvSpecPlugin::OnJSGetMedicInfo(WebView *webView, const JSArray &args) {
	Awesomium::JSObject medicInfo;

	Awesomium::JSObject bluMedicObj;
	bluMedicObj.SetProperty(Awesomium::WSLit("weaponId"), Awesomium::JSValue(bluMedic.weaponID));
	bluMedicObj.SetProperty(Awesomium::WSLit("charge"), Awesomium::JSValue(Round(bluMedic.charge*100.0f)));

	Awesomium::JSObject redMedicObj;
	redMedicObj.SetProperty(Awesomium::WSLit("weaponId"), Awesomium::JSValue(redMedic.weaponID));
	redMedicObj.SetProperty(Awesomium::WSLit("charge"), Awesomium::JSValue(Round(redMedic.charge*100.0f)));

	medicInfo.SetProperty(Awesomium::WSLit("bluMedic"), Awesomium::JSValue(bluMedicObj));
	medicInfo.SetProperty(Awesomium::WSLit("redMedic"), Awesomium::JSValue(redMedicObj));

	return Awesomium::JSValue(medicInfo);
}

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

static void reload_overlay(const CCommand& args) {
	//TODO: do i need this,
	//      local file loading
	Awesomium::WebURL blank(Awesomium::WSLit("about:blank"));
	Awesomium::WebURL url;

	if (args.ArgC() > 1) {
		url = Awesomium::WebURL(Awesomium::WSLit(args.Arg(1)));
	} else {
		url = Awesomium::WebURL(Awesomium::WSLit("http://mattmcn.com/omp.html"));
	}
	
	m_WebView->LoadURL(blank);
	m_WebView->LoadURL(url);

	m_WebView->Resize(screenWidth, screenHeight);
}

__inline void PrintOutlineColorCommandUsage() {
	Msg("Usage: advspec_outline_color [r|b] [0-255] [0-255] [0-255]\n       Color values in Red, Green, Blue order\n");
}

#if defined(DEBUG)
static void scaled_value(const CCommand& args)
{
	int arg = atoi(args.Arg(1));
	Msg("%d -> %d\n", arg, SCALED(arg));
}
#endif

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
