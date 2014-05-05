/*
 *  medic_info_drawing.h
 *  AdvSpec project
 *  
 *  Copyright (c) 2013 Matthew McNamara
 *  BSD 2-Clause License
 *  http://opensource.org/licenses/BSD-2-Clause
 *
 */

#ifndef MEDIC_INFO_H
#define MEDIC_INFO_H

#define _S SCALED

#if defined(MEDIC_OMP)
inline void DrawMedicOmp(int offX, int offY, vgui::ISurface *pSurface)
{
	wchar_t wbuf[1024] = { '\0' };

	// Light BG
	pSurface->DrawSetColor(30, 30, 30, 30);
	pSurface->DrawFilledRect(_S(offX), _S(offY), _S(offX) + _S(139), _S(offY) + _S(37));
	// height = 3 + 14 + 3 + 14 + 3

	// Dark BG parts
	pSurface->DrawSetColor(30, 30, 30, 255);
	pSurface->DrawFilledRect(_S(offX),           _S(offY),          _S(offX) + _S(139), _S(offY) + _S(3));  //top
	pSurface->DrawFilledRect(_S(offX),           _S(offY) + _S(34), _S(offX) + _S(139), _S(offY) + _S(37)); //bottom
	pSurface->DrawFilledRect(_S(offX),           _S(offY) + _S(3),  _S(offX) + _S(3),   _S(offY) + _S(34)); //left
	pSurface->DrawFilledRect(_S(offX) + _S(136), _S(offY) + _S(3),  _S(offX) + _S(139), _S(offY) + _S(34)); //right
	pSurface->DrawFilledRect(_S(offX) + _S(27),  _S(offY) + _S(3),  _S(offX) + _S(30),  _S(offY) + _S(34)); //center |
	pSurface->DrawFilledRect(_S(offX) + _S(3),   _S(offY) + _S(17), _S(offX) + _S(136), _S(offY) + _S(20)); //center -

	// Gray Uber BGs
	pSurface->DrawSetColor(128, 120, 104, 255);
	pSurface->DrawFilledRect(_S(offX) + _S(3), _S(offY) + _S(3),  _S(offX) + _S(27), _S(offY) + _S(17));
	pSurface->DrawFilledRect(_S(offX) + _S(3), _S(offY) + _S(20), _S(offX) + _S(27), _S(offY) + _S(34));

	// Blu BG
	pSurface->DrawSetColor(104, 124, 155, 127);
	pSurface->DrawFilledRect(_S(offX) + _S(30), _S(offY) + _S(3), _S(offX) + _S(136), _S(offY) + _S(17));

	// Red BG
	pSurface->DrawSetColor(180, 92, 77, 127);
	pSurface->DrawFilledRect(_S(offX) + _S(30), _S(offY) + _S(20), _S(offX) + _S(136), _S(offY) + _S(34));

	int textWidth = 0;
	int i;

	// Uber %s & shadows
	// Blu
	//swprintf( wbuf, L"%d%%", Round(bluMedic.charge*100.0f) );
	swprintf( wbuf, 1024, L"%d%%", Round(bluMedic.charge*100.0f) );
	for (i = 0; wbuf[i] != 0 && i < 1024; i++)
		textWidth += pSurface->GetCharacterWidth(m_font, wbuf[i]);
	// Shadow
	pSurface->DrawSetTextColor(30, 30, 30, 255);
	pSurface->DrawSetTextPos(_S(offX) + _S(15) + _S(1) - (textWidth/2), _S(offY) + _S(4) + _S(1));
	pSurface->DrawPrintText(wbuf, wcslen( wbuf ));
	// Main
	pSurface->DrawSetTextColor(255, 255, 255, 255);
	pSurface->DrawSetTextPos(_S(offX) + _S(15) - (textWidth/2), _S(offY) + _S(4));
	pSurface->DrawPrintText(wbuf, wcslen( wbuf ));

	// Red
	textWidth = 0;
	swprintf( wbuf, 1024, L"%d%%", Round(redMedic.charge*100.0f) );
	for (i = 0; wbuf[i] != 0 && i < 1024; i++)
		textWidth += pSurface->GetCharacterWidth(m_font, wbuf[i]);
	// Shadow
	pSurface->DrawSetTextColor(30, 30, 30, 255);
	pSurface->DrawSetTextPos(_S(offX) + _S(15) + _S(1) - (textWidth/2), _S(offY) + _S(21) + _S(1));
	pSurface->DrawPrintText(wbuf, wcslen( wbuf ));
	// Main
	pSurface->DrawSetTextColor(255, 255, 255, 255);
	pSurface->DrawSetTextPos(_S(offX) + _S(15) - (textWidth/2), _S(offY) + _S(21));
	pSurface->DrawPrintText(wbuf, wcslen( wbuf ));

	// Gun names
	pSurface->DrawSetTextColor(255, 255, 255, 255);
	swprintf( wbuf, 1024, L"%S", nameForWeaponID(bluMedic.weaponID) );
	pSurface->DrawSetTextPos(_S(offX) + _S(34), _S(offY) + _S(4));
	pSurface->DrawPrintText(wbuf, wcslen( wbuf ));
	swprintf( wbuf, 1024, L"%S", nameForWeaponID(redMedic.weaponID) );
	pSurface->DrawSetTextPos(_S(offX) + _S(34), _S(offY) + _S(21));
	pSurface->DrawPrintText(wbuf, wcslen( wbuf ));

	//advantage
	int advantage = 0;
	int advantageOffY = 0;
	if (bluMedic.charge > redMedic.charge) {
		advantage = Round(bluMedic.charge*100.0f) - Round(redMedic.charge*100.0f);
	} else {
		advantage = Round(redMedic.charge*100.0f) - Round(bluMedic.charge*100.0f);
		advantageOffY = 17;
	}

	if (advantage > 0) {
		textWidth = 0;
		swprintf( wbuf, 1024, L"+%d%%", advantage );
		for (i = 0; wbuf[i] != 0 && i < 1024; i++)
			textWidth += pSurface->GetCharacterWidth(m_font, wbuf[i]);

		pSurface->DrawSetTextColor(30, 30, 30, 255);
		pSurface->DrawSetTextPos(_S(offX) + _S(132) + _S(1) - textWidth, _S(offY) + _S(advantageOffY) + _S(4) + _S(1)); //Shadow
		pSurface->DrawPrintText(wbuf, wcslen( wbuf ));
		pSurface->DrawSetTextColor(30, 225, 225, 255);
		pSurface->DrawSetTextPos(_S(offX) + _S(132) - textWidth, _S(offY) + _S(advantageOffY) + _S(4)); //Main
		pSurface->DrawPrintText(wbuf, wcslen( wbuf ));
	}
}
#endif

#if defined(MEDIC_VTV)
inline void DrawMedicVTV(int offX, int offY, vgui::ISurface *pSurface)
{
	wchar_t wbuf[1024] = { '\0' };

	// Blu texture
	pSurface->DrawSetTexture(m_iTextureBlu);
	pSurface->DrawSetColor(255, 255, 255, 255);
	pSurface->DrawTexturedRect(offX, offY, offX + 256, offY + 56);

	// Red texture
	pSurface->DrawSetTexture(m_iTextureRed);
	pSurface->DrawSetColor(255, 255, 255, 255);
	pSurface->DrawTexturedRect(offX, offY + 33, offX + 256, offY + 89);

	//Draw Uber %s
	pSurface->DrawSetTextColor(251, 251, 0, 255);

	int textWidth = 0;
	int i;
	swprintf( wbuf, 1024, L"%d%%", (int)(bluMedic.charge*100.0f) );
	for (i = 0; wbuf[i] != 0 && i < 1024; i++)
		textWidth += pSurface->GetCharacterWidth(m_font, wbuf[i]);
	pSurface->DrawSetTextPos(offX + 33 - textWidth, offY + 6);
	pSurface->DrawPrintText(wbuf, wcslen( wbuf ));
	textWidth = 0;
	swprintf( wbuf, 1024, L"%d%%", (int)(redMedic.charge*100.0f) );
	for (i = 0; wbuf[i] != 0 && i < 1024; i++)
		textWidth += pSurface->GetCharacterWidth(m_font, wbuf[i]);
	pSurface->DrawSetTextPos(offX + 33 - textWidth, offY + 33 + 6);
	pSurface->DrawPrintText(wbuf, wcslen( wbuf ));

	//Draw names
	pSurface->DrawSetTextColor(45, 45, 45, 255);
	pSurface->DrawSetTextPos(offX + 40, offY + 6);
	swprintf( wbuf, L"%S", nameForWeaponID(bluMedic.weaponID) );
	pSurface->DrawPrintText(wbuf, wcslen( wbuf ));
	pSurface->DrawSetTextPos(offX + 40, offY + 33 + 6);
	swprintf( wbuf, L"%S", nameForWeaponID(redMedic.weaponID) );
	pSurface->DrawPrintText(wbuf, wcslen( wbuf ));

	//Draw advantage
	int advantage = 0;
	int advantageOffY = 0;
	if (bluMedic.charge > redMedic.charge) {
		advantage = (int)(bluMedic.charge*100.0f) - (int)(redMedic.charge*100.0f);
	} else {
		advantage = (int)(redMedic.charge*100.0f) - (int)(bluMedic.charge*100.0f);
		advantageOffY = 33;
	}

	if (advantage > 0) {
		textWidth = 0;
		swprintf( wbuf, 1024, L"+%d", advantage );
		for (i = 0; wbuf[i] != 0 && i < 1024; i++)
			textWidth += pSurface->GetCharacterWidth(m_font, wbuf[i]);
		pSurface->DrawSetTextPos(offX + 200 - textWidth, offY + advantageOffY + 6);
		pSurface->DrawPrintText(wbuf, wcslen( wbuf ));
	}
}
#endif

#if defined(MEDIC_TFTV)
inline void DrawMedicTFTV(int offX, int offY, vgui::ISurface *pSurface)
{
	wchar_t wbuf[1024] = { '\0' };

	if (m_healthFont == 0) {
		vgui::HScheme scheme = pScheme->GetScheme("ClientScheme");
		m_healthFont = pScheme->GetIScheme(scheme)->GetFont("roboto12", true);
	}

	// Black BGs
	pSurface->DrawSetColor(27, 27, 27, 50);
	pSurface->DrawFilledRect(_S(offX), _S(offY) + _S(10), _S(offX) + _S(126), _S(offY) + _S(18));
	pSurface->DrawFilledRect(_S(offX), _S(offY) + _S(19) + _S(10), _S(offX) + _S(126), _S(offY) + _S(38));

	// Blu BG texture
	pSurface->DrawSetColor(255, 255, 255, 255);
	pSurface->DrawSetTexture(m_iTextureBlu);
	pSurface->DrawTexturedRect(_S(offX), _S(offY), _S(offX) + _S(126), _S(offY) + _S(19)); // 111

	// Red BG texture
	pSurface->DrawSetTexture(m_iTextureRed);
	pSurface->DrawTexturedRect(_S(offX), _S(offY) + _S(19), _S(offX) + _S(126), _S(offY) + _S(38)); //111

	// Draw Uber % BGs
	pSurface->DrawSetColor(200, 187, 161, 255);
	pSurface->DrawFilledRect(_S(offX), _S(offY) + _S(1), _S(offX) + _S(20), _S(offY) + _S(1) + _S(18)); //90 & 110
	pSurface->DrawFilledRect(_S(offX), _S(offY) + _S(19) + _S(1), _S(offX) + _S(20), _S(offY) + _S(19) + _S(1) + _S(18)); //90 & 110

	pSurface->DrawSetColor(44, 44, 44, 255);
	pSurface->DrawFilledRect(_S(offX) + _S(1), //90
		_S(offY) + _S(1) + _S(1),
		_S(offX) + _S(20) - _S(1), //110
		_S(offY) + _S(18));
	pSurface->DrawFilledRect(_S(offX) + _S(1), //90
		_S(offY) + _S(19) + _S(1) + _S(1),
		_S(offX) + _S(20) - _S(1), //110
		_S(offY) + _S(19) + _S(18));

	//Draw names
	swprintf( wbuf, L"%S", nameForWeaponID(bluMedic.weaponID) );
	//blu shadow
	pSurface->DrawSetTextColor(0, 0, 0, 100);
	pSurface->DrawSetTextPos(_S(offX) + _S(20) + _S(5), _S(offY) + _S(3) + _S(1));
	pSurface->DrawPrintText(wbuf, wcslen( wbuf ));
	//blumain
	pSurface->DrawSetTextColor(233, 233, 233, 255);
	pSurface->DrawSetTextPos(_S(offX) + _S(20) + _S(5), _S(offY) + _S(3));
	pSurface->DrawPrintText(wbuf, wcslen( wbuf ));
	//redshadow
	swprintf( wbuf, L"%S", nameForWeaponID(redMedic.weaponID) );
	pSurface->DrawSetTextColor(0, 0, 0, 100);
	pSurface->DrawSetTextPos(_S(offX) + _S(20) + _S(5), _S(offY) + _S(19) + _S(3) + _S(1));
	pSurface->DrawPrintText(wbuf, wcslen( wbuf ));
	//redmain
	pSurface->DrawSetTextColor(233, 233, 233, 255);
	pSurface->DrawSetTextPos(_S(offX) + _S(20) + _S(5), _S(offY) + _S(19) + _S(3));
	pSurface->DrawPrintText(wbuf, wcslen( wbuf ));

	//Draw Uber %s
	/// BGs
	pSurface->DrawSetColor(255, 255, 255, 255);
	pSurface->DrawSetTexture(m_iTextureHealth);
	int healthBGHeightBlu = _S(Round(16 * bluMedic.charge));
	int healthBGHeightRed = _S(Round(16 * redMedic.charge));
	
	int roundChargeBlu = Round(bluMedic.charge*100.0f);
	int roundChargeRed = Round(redMedic.charge*100.0f);

	if (roundChargeBlu == 100) {
		pSurface->DrawTexturedRect(_S(offX) + _S(1), //90
			_S(offY) + _S(1) + _S(1),
			_S(offX) + _S(20) - _S(1), //110
			_S(offY) + _S(18));
	} else {
		pSurface->DrawTexturedRect(_S(offX) + _S(1), //90
			_S(offY) + _S(18) - healthBGHeightBlu,
			_S(offX) + _S(20) - _S(1), //110
			_S(offY) + _S(18));
	}

	if (roundChargeRed == 100) {
		pSurface->DrawTexturedRect(_S(offX) + _S(1), //90
			_S(offY) + _S(19) + _S(1) + _S(1),
			_S(offX) + _S(20) - _S(1), //110
			_S(offY) + _S(19) + _S(18));
	} else {
		pSurface->DrawTexturedRect(_S(offX) + _S(1), //90
			_S(offY) + _S(19) + _S(18) - healthBGHeightRed,
			_S(offX) + _S(20) - _S(1), //110
			_S(offY) + _S(19) + _S(18));
	}

	pSurface->DrawSetTextFont(m_healthFont);

	/// Text
	int textWidth = 0;
	int i;
	swprintf( wbuf, 1024, L"%d", Floor_adv(bluMedic.charge*100.0f) );
	for (i = 0; wbuf[i] != 0 && i < 1024; i++)
		textWidth += pSurface->GetCharacterWidth(m_font, wbuf[i]);
	pSurface->DrawSetTextPos(_S(offX) + _S(10) - (textWidth/2), _S(offY) + _S(3) + _S(1)); //90
	pSurface->DrawSetTextColor(0, 0, 0, 205);
	pSurface->DrawPrintText(wbuf, wcslen( wbuf )); // shadow
	pSurface->DrawSetTextPos(_S(offX) + _S(10) - (textWidth/2), _S(offY) + _S(3)); //90
	pSurface->DrawSetTextColor(255, 255, 255, 255);
	pSurface->DrawPrintText(wbuf, wcslen( wbuf )); // main

	textWidth = 0;
	swprintf( wbuf, 1024, L"%d", Floor_adv(redMedic.charge*100.0f) );
	for (i = 0; wbuf[i] != 0 && i < 1024; i++)
		textWidth += pSurface->GetCharacterWidth(m_font, wbuf[i]);
	pSurface->DrawSetTextPos(_S(offX) + _S(10) - (textWidth/2), _S(offY) + _S(19) + _S(3) + _S(1)); //90
	pSurface->DrawSetTextColor(0, 0, 0, 205);
	pSurface->DrawPrintText(wbuf, wcslen( wbuf )); // shadow
	pSurface->DrawSetTextPos(_S(offX) + _S(10) - (textWidth/2), _S(offY) + _S(19) + _S(3)); //90
	pSurface->DrawSetTextColor(255, 255, 255, 255);
	pSurface->DrawPrintText(wbuf, wcslen( wbuf )); // main

	//Draw advantage
	int advantage = 0;
	int drawX = _S(offX) + _S(122); //86
	int drawY = _S(offY) + _S(3);
	if (bluMedic.charge > redMedic.charge) {
		advantage = Floor_adv(bluMedic.charge*100.0f) - Floor_adv(redMedic.charge*100.0f);
	} else {
		advantage = Floor_adv(redMedic.charge*100.0f) - Floor_adv(bluMedic.charge*100.0f);
		drawY += _S(19);
	}

	if (advantage > 0) {
		textWidth = 0;
		swprintf( wbuf, L"+%d%%", advantage );
		//swprintf( wbuf, L"+100%%" );
		for (i = 0; wbuf[i] != 0 && i < 1024; i++)
			textWidth += pSurface->GetCharacterWidth(m_font, wbuf[i]);
		pSurface->DrawSetTextPos(drawX - textWidth, drawY + _S(1));
		pSurface->DrawSetTextColor(0, 0, 0, 150);
		pSurface->DrawPrintText(wbuf, wcslen( wbuf )); // shadow
		pSurface->DrawSetTextPos(drawX - textWidth, drawY);
		pSurface->DrawSetTextColor(249, 255, 181, 255);
		pSurface->DrawPrintText(wbuf, wcslen( wbuf )); // main
	}
}
#endif

#if defined(MEDIC_EXTV)
ConVar extv("extv", "100", 0, "How many pixels from the left is the Medic Info box");
ConVar extv2("extv2", "100", 0, "How many pixels from the left is the Medic Info box");

void DrawMedicEXTV(int offX, int offY, vgui::ISurface *pSurface)
{
	wchar_t wbuf[1024] = { '\0' };

	if (m_healthFont == 0) {
		vgui::HScheme scheme = pScheme->GetScheme("ClientScheme");
		m_healthFont = pScheme->GetIScheme(scheme)->GetFont("CerbeticaBold16", true);
		m_advFont = pScheme->GetIScheme(scheme)->GetFont("Cerbetica12", true);
	}

	// NO MORE MANUAL HUDS
	int w, h;
	float s1, t1;

	w = 240; h = 28;
	s1 = 0.485f; t1 = 0.05;

	// BG colours
	pSurface->DrawSetColor(104, 124, 155, 127);
	pSurface->DrawTexturedRect(_S(offX), _S(offY), _S(offX) + w, _S(offY) + h);
	pSurface->DrawSetColor(180, 92, 77, 127);
	pSurface->DrawTexturedRect(_S(offX), _S(offY) + h, _S(offX) + w, _S(offY) + h*2);

	// BG texture
	pSurface->DrawSetColor(255, 255, 255, 255);
	pSurface->DrawSetTexture(m_iTextureFrame);
	pSurface->DrawTexturedSubRect(_S(offX), _S(offY), _S(offX) + w, _S(offY) + h, 
		0.0, 0.0, s1, t1);
	pSurface->DrawTexturedSubRect(_S(offX), _S(offY) + h, _S(offX) + w, _S(offY) + h*2, 
		0.0, 0.0, s1, t1);

	// Draw Uber % BGs
	pSurface->DrawSetColor(44, 44, 44, 255);
	pSurface->DrawFilledRect(_S(offX) + 30, _S(offY) + 4, _S(offX) + 66, _S(offY) + h - 4);
	pSurface->DrawFilledRect(_S(offX) + 30, _S(offY) + h + 4, _S(offX) + 66, _S(offY) + h*2 - 4);

	//Draw names
	swprintf( wbuf, L"%S", nameForWeaponID(bluMedic.weaponID) );
	//blu
	pSurface->DrawSetTextColor(235, 226, 202, 255);
	pSurface->DrawSetTextPos(_S(offX) + _S(68), _S(offY) + _S(4));
	pSurface->DrawPrintText(wbuf, wcslen( wbuf ));
	//red
	swprintf( wbuf, L"%S", nameForWeaponID(redMedic.weaponID) );
	pSurface->DrawSetTextPos(_S(offX) + _S(68), _S(offY) + _S(4) + h);
	pSurface->DrawPrintText(wbuf, wcslen( wbuf ));

	//Draw Uber %s
	/// BGs
	pSurface->DrawSetColor(255, 255, 255, 255);
	pSurface->DrawSetTexture(m_iTextureHealth);
	int healthBGHeightBlu = Round(20 * bluMedic.charge);
	int healthBGHeightRed = Round(20 * redMedic.charge);
	
	int roundChargeBlu = Round(bluMedic.charge*100.0f);
	int roundChargeRed = Round(redMedic.charge*100.0f);

	if (roundChargeBlu == 100) {
		pSurface->DrawTexturedRect(_S(offX) + 30, _S(offY) + 4, _S(offX) + 66, _S(offY) + h - 4);
	} else {
		pSurface->DrawTexturedRect(_S(offX) + 30, _S(offY) + h - 4 - healthBGHeightBlu, _S(offX) + 66, _S(offY) + h - 4);
	}

	if (roundChargeRed == 100) {
		pSurface->DrawTexturedRect(_S(offX) + 30, _S(offY) + h + 4, _S(offX) + 66, _S(offY) + h*2 - 4);
	} else {
		pSurface->DrawTexturedRect(_S(offX) + 30, _S(offY) + h*2 - 4 - healthBGHeightRed, _S(offX) + 66, _S(offY) + h*2 - 4);
	}

	pSurface->DrawSetTextFont(m_healthFont);

	/// Text
	int textWidth = 0;
	int i;
	swprintf( wbuf, 1024, L"%d", (int)(bluMedic.charge*100.0f) );
	for (i = 0; wbuf[i] != 0 && i < 1024; i++)
		textWidth += pSurface->GetCharacterWidth(m_font, wbuf[i]);
	pSurface->DrawSetTextPos(_S(offX) + 48 - (textWidth/2), _S(offY) + 3); //90
	pSurface->DrawSetTextColor(0, 0, 0, 255);
	pSurface->DrawPrintText(wbuf, wcslen( wbuf )); // shadow
	pSurface->DrawSetTextPos(_S(offX) + 47 - (textWidth/2), _S(offY) + 2); //90
	pSurface->DrawSetTextColor(235, 226, 202, 255);
	pSurface->DrawPrintText(wbuf, wcslen( wbuf )); // main

	textWidth = 0;
	swprintf( wbuf, 1024, L"%d", (int)(redMedic.charge*100.0f) );
	for (i = 0; wbuf[i] != 0 && i < 1024; i++)
		textWidth += pSurface->GetCharacterWidth(m_font, wbuf[i]);
	pSurface->DrawSetTextPos(_S(offX) + 48 - (textWidth/2), _S(offY) + 3 + h); //90
	pSurface->DrawSetTextColor(0, 0, 0, 255);
	pSurface->DrawPrintText(wbuf, wcslen( wbuf )); // shadow
	pSurface->DrawSetTextPos(_S(offX) + 47 - (textWidth/2), _S(offY) + 2 + h); //90
	pSurface->DrawSetTextColor(235, 226, 202, 255);
	pSurface->DrawPrintText(wbuf, wcslen( wbuf )); // main

	pSurface->DrawSetTextFont(m_advFont);

	//Draw advantage
	int advantage = 0;
	int drawX = _S(offX) + 236; //86
	int drawY = _S(offY) + 8;
	if (bluMedic.charge > redMedic.charge) {
		advantage = (int)(bluMedic.charge*100.0f) - (int)(redMedic.charge*100.0f);
	} else {
		advantage = (int)(redMedic.charge*100.0f) - (int)(bluMedic.charge*100.0f);
		drawY += 28;
	}

	if (advantage > 0) {
		textWidth = 0;
		swprintf( wbuf, L"+%d%%", advantage );
		//swprintf( wbuf, L"+100%%" );
		for (i = 0; wbuf[i] != 0 && i < 1024; i++)
			textWidth += pSurface->GetCharacterWidth(m_font, wbuf[i]);
		pSurface->DrawSetTextPos(drawX - textWidth + 2, drawY + 1);
		pSurface->DrawSetTextColor(0, 0, 0, 196);
		pSurface->DrawPrintText(wbuf, wcslen( wbuf )); // shadow
		pSurface->DrawSetTextPos(drawX - textWidth, drawY);
		pSurface->DrawSetTextColor(0, 255, 255, 255);
		pSurface->DrawPrintText(wbuf, wcslen( wbuf )); // main
	}
}
#endif

#if defined(MEDIC_TOTH)
inline void DrawMedicTotH(int offX, int offY, vgui::ISurface *pSurface)
{
	wchar_t wbuf[1024] = { '\0' };

	if (m_healthFont == 0) {
		vgui::HScheme scheme = pScheme->GetScheme("ClientScheme");
		m_healthFont = pScheme->GetIScheme(scheme)->GetFont("NoveMedium14", true);
	}

	pSurface->DrawSetTextFont(m_healthFont);

	// Black BG
	pSurface->DrawSetColor(18, 17, 17, 255);
	pSurface->DrawFilledRect(_S(offX), _S(offY), _S(offX) + _S(164), _S(offY) + _S(34));

	// White BG
	pSurface->DrawSetColor(244, 244, 244, 255);
	pSurface->DrawFilledRect(_S(offX + 35), _S(offY), _S(offX) + _S(133), _S(offY) + _S(34));

	// Uber % BGs
	int uberBGWidthBlu = (int)(35 * bluMedic.charge);
	int uberBGWidthRed = (int)(35 * redMedic.charge);
	// Blu
	pSurface->DrawSetColor(18, 127, 220, 255);
	pSurface->DrawFilledRect(_S(offX), _S(offY), _S(offX) + _S(uberBGWidthBlu), _S(offY) + _S(17));
	// Red
	pSurface->DrawSetColor(245, 54, 64, 255);
	pSurface->DrawFilledRect(_S(offX), _S(offY) + _S(17), _S(offX) + _S(uberBGWidthRed), _S(offY) + _S(34));

	int textWidth = 0;
	int i;

	// Uber %s
	// Blu
	//swprintf( wbuf, L"%d%%", Round(bluMedic.charge*100.0f) );
	swprintf( wbuf, 1024, L"%d%%", (int)(bluMedic.charge*100.0f) );
	for (i = 0; wbuf[i] != 0 && i < 1024; i++)
		textWidth += pSurface->GetCharacterWidth(m_healthFont, wbuf[i]);
	// Main
	pSurface->DrawSetTextColor(227, 227, 227, 255);
	pSurface->DrawSetTextPos(_S(offX) + _S(17) - (textWidth/2), _S(offY) + _S(1));
	pSurface->DrawPrintText(wbuf, wcslen( wbuf ));

	// Red
	textWidth = 0;
	swprintf( wbuf, 1024, L"%d%%", (int)(redMedic.charge*100.0f) );
	for (i = 0; wbuf[i] != 0 && i < 1024; i++)
		textWidth += pSurface->GetCharacterWidth(m_healthFont, wbuf[i]);
	// Main
	pSurface->DrawSetTextPos(_S(offX) + _S(17) - (textWidth/2), _S(offY) + _S(18));
	pSurface->DrawPrintText(wbuf, wcslen( wbuf ));

	//advantage
	int advantage = 0;
	int advantageOffY = 0;
	if (bluMedic.charge > redMedic.charge) {
		advantage = (int)(bluMedic.charge*100.0f) - (int)(redMedic.charge*100.0f);
	} else {
		advantage = (int)(redMedic.charge*100.0f) - (int)(bluMedic.charge*100.0f);
		advantageOffY = 17;
	}

	if (advantage > 0) {
		textWidth = 0;
		swprintf( wbuf, 1024, L"+%d", advantage );
		for (i = 0; wbuf[i] != 0 && i < 1024; i++)
			textWidth += pSurface->GetCharacterWidth(m_healthFont, wbuf[i]);

		pSurface->DrawSetTextPos(_S(offX) + _S(148) - (textWidth/2), _S(offY) + _S(advantageOffY) + _S(1)); //Main
		pSurface->DrawPrintText(wbuf, wcslen( wbuf ));
	}

	pSurface->DrawSetTextFont(m_font);

	// Gun names
	pSurface->DrawSetTextColor(18, 17, 17, 255);
	swprintf( wbuf, 1024, L"%S", nameForWeaponID(bluMedic.weaponID) );
	pSurface->DrawSetTextPos(_S(offX) + _S(37), _S(offY) + _S(3));
	pSurface->DrawPrintText(wbuf, wcslen( wbuf ));
	swprintf( wbuf, 1024, L"%S", nameForWeaponID(redMedic.weaponID) );
	pSurface->DrawSetTextPos(_S(offX) + _S(37), _S(offY) + _S(20));
	pSurface->DrawPrintText(wbuf, wcslen( wbuf ));
}
#endif

#endif
