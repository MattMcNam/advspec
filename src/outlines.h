#pragma once

#include "sdk.h"
#include "sdk/convar.h"
#include "sdk/color.h"
#include "vmthooks.h"

class COutlines
{
public:
	bool Init();
	void Close();

	static void __cdecl MyToggleOutlines( const CCmdArgs& );
	static void __cdecl MyOutlineColor( const CCmdArgs& );

	bool glow_enabled;
	Color* bluColor;
	Color* redColor;

	ConCommand* as_toggle_outline;
	ConCommand* as_outline_color;

	//Force both of these to 1
	ConVar* glow_outline_effect_enable;
	ConVar* replay_rendersetting_renderglow;

	struct offets_t {
		int bGlowEnabled;
		int iTeamNum;
	} offsets;
	
	static COutlines g;
};
