/*
 *  offsets.h
 *  WebSpec project
 *  Modified for AdvSpec
 *  
 *  Copyright (c) 2013 Matthew McNamara
 *  BSD 2-Clause License
 *  http://opensource.org/licenses/BSD-2-Clause
 *
 */

#ifndef OFFSETS_H
#define OFFSETS_H

#include "ifaces.h"

#include "cdll_client_int.h"
#include "client_class.h"

class WSOffsets {
public:
	static int pCTFPlayer__m_iTeamNum;
	static int pCTFPlayer__m_bGlowEnabled;
	static int pCTFPlayer__m_iClass;
	static int pCTFPlayer__m_hMyWeapons;
	static int pCWeaponMedigun__m_flChargeLevel;
	static int pCWeaponMedigun__m_iItemDefinitionIndex;

	static void PrepareOffsets();
	static int FindOffsetOfClassProp(const char *className, const char *propName);
	static int FindOffsetOfArrayEnt(const char *classname, const char *arrayName, int element);

private:
	static bool CrawlForPropOffset(RecvTable *sTable, const char *propName, int &offset);
	static bool CrawlForArrayEnt(RecvTable *sTable, const char *propName, int element, int &offset);
};

#define MakePtr( cast, ptr, addValue ) (cast)( (unsigned long)(ptr) + (unsigned long)(addValue))

#endif
