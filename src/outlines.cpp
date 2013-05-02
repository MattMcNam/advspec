#include "stdafx.h"

#include "outlines.h"
#include "ifaces.h"
#include "util.h"
#include "game.h"

#include "sdk/icvar.h"
#include "sdk/cdll_int.h"
#include "sdk/globalvars.h"
#include "sdk/icliententitylist.h"

COutlines COutlines::g;

// Hooks n' shit
// Replace a function pointer with our own, returning pointer to  original
DWORD *HookVFunc(DWORD *vtable, int index, DWORD *newFunction)
{
	DWORD dwOldProt, *oldFunc;
	VirtualProtect(&vtable[index], 4, PAGE_EXECUTE_READWRITE, &dwOldProt);
	oldFunc=(DWORD*)vtable[index];
	vtable[index]=(DWORD)newFunction;
	VirtualProtect(&vtable[index], 4, dwOldProt, &dwOldProt);
	return oldFunc;
}

class EmptyClass {};

// CBaseCombatCharacter::UpdateGlowEffect()
void CBaseCombatCharacter_UpdateGlowEffect(IClientEntity *thisPtr) {
	//Magic from SourceMod
	void **this_ptr = *(void ***)&thisPtr;
	void **vtable = *(void ***)thisPtr;
	void *func = vtable[218];

	union {
		void (EmptyClass::*mfpnew)();
		void *addr;
	} u;
	u.addr = func;

	(void) (reinterpret_cast<EmptyClass*>(this_ptr)->*u.mfpnew)();
}

// CBaseCombatCharacter::GetGlowEffectColor(float &red, float &green, &float &blue);
void (__fastcall *origGetGlowEffectColor)(void* thisptr, int edx, float*, float*, float*);

void __fastcall hookedGetGlowEffectColor( IClientEntity *thisPtr, int edx, float &red, float &green, float &blue ) {
	int team = GetProp<int>(thisPtr, COutlines::g.offsets.iTeamNum);

	if (team == 2) {
		red   = float(COutlines::g.redColor->r() / 255.0);
		green = float(COutlines::g.redColor->g() / 255.0);
		blue  = float(COutlines::g.redColor->b() / 255.0 );
	} else if (team == 3) {
		red   = float(COutlines::g.bluColor->r() / 255.0);
		green = float(COutlines::g.bluColor->g() / 255.0);
		blue  = float(COutlines::g.bluColor->b() / 255.0);
	} else {
		origGetGlowEffectColor(thisPtr, edx, &red, &green, &blue);
	}
}

bool COutlines::Init()
{
	if (Ifaces.pEngine->GetDXSupportLevel() < 90) {
		Msg("[AS] Outline effect requires DX9!\n");
		return false;
	}

	as_toggle_outline = new ConCommand("advspec_toggle_outline", "Toggles glow effect on players", 0, &MyToggleOutlines);
	Ifaces.pCvar->RegisterConCommand(as_toggle_outline);

	as_outline_color = new ConCommand("advspec_outline_color", "Sets the RGB color of the glow per team", 0, &MyOutlineColor);
	Ifaces.pCvar->RegisterConCommand(as_outline_color);

	// Default to these, should change to convars
	redColor = new Color(184,  56,  59, 255);
	bluColor = new Color( 88, 133, 162, 255);

	// Enable glow effect by default
	glow_outline_effect_enable = Ifaces.pCvar->FindVar("glow_outline_effect_enable");
	glow_outline_effect_enable->SetInt(1);
	replay_rendersetting_renderglow = Ifaces.pCvar->FindVar("replay_rendersetting_renderglow");
	replay_rendersetting_renderglow->SetInt(1);

	// Get netvar offsets
	NETVAR("CBaseCombatCharacter", &offsets.bGlowEnabled, "m_bGlowEnabled");
	NETVAR("CTFPlayer", &offsets.iTeamNum, "m_iTeamNum");

	origGetGlowEffectColor = NULL;

	glow_enabled = false;

	return true;
}
void COutlines::Close()
{
	Ifaces.pCvar->UnregisterConCommand(as_toggle_outline);
	Ifaces.pCvar->UnregisterConCommand(as_outline_color);
	delete as_toggle_outline;
	delete as_outline_color;

	delete redColor;
	delete bluColor;

	// TODO: Unhook (aka use vmthooks)
}

__inline void SetGlow() {
	int iEntCount = Ifaces.pEntityList->GetHighestEntityIndex();
	IClientEntity *clientEntity;

	for (int i = 0; i < iEntCount; i++) {
		clientEntity = Ifaces.pEntityList->GetClientEntity(i);

		// Ensure valid player entity
		if (clientEntity == NULL || !(*Ifaces.ppGameRes)->bConnected(i))
			continue;

		//Hook GetGlowEffectColor if it hasn't been done
		//TODO: Learn Casual's VMTHooks class
		if (origGetGlowEffectColor == NULL) {
			origGetGlowEffectColor = (void (__fastcall *)(void *, int, float*, float*, float*))
				HookVFunc(*(DWORD**)clientEntity, 217, (DWORD*) &hookedGetGlowEffectColor);
		}

		// Set m_bGlowEnabled and call UpdateGlowEffect
		GetProp<bool>(clientEntity, COutlines::g.offsets.bGlowEnabled) = COutlines::g.glow_enabled;
		CBaseCombatCharacter_UpdateGlowEffect(clientEntity);
	}

	// Console message
	if (COutlines::g.glow_enabled)
		Msg("[AS] Player outlines enabled\n");
	else
		Msg("[AS] Player outlines disabled\n");
}

void __cdecl COutlines::MyToggleOutlines( const CCmdArgs& args )
{
	if (!Ifaces.pEngine->IsHLTV() && !Ifaces.pEngine->IsPlayingDemo()) {
		Msg("[AS] Outlines can only be toggled in STV or demos!\n");
		g.glow_enabled = false;
		return;
	}

	g.glow_enabled = !g.glow_enabled;

	SetGlow();
}

__inline void PrintOutlineColorCommandUsage() {
	Msg("Usage: advspec_outline_color [r|b] [0-255] [0-255] [0-255]\n       Color values in Red, Green, Blue order\n");
}

void __cdecl COutlines::MyOutlineColor( const CCmdArgs& args )
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
		g.redColor->SetColor(red, green, blue, 255);
	else
		g.bluColor->SetColor(red, green, blue, 255);

	SetGlow();
}
