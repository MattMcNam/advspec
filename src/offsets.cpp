/*
 *  offsets.cpp
 *  WebSpec project
 *  Modified for AdvSpec
 *  
 *  Copyright (c) 2013 Matthew McNamara
 *  BSD 2-Clause License
 *  http://opensource.org/licenses/BSD-2-Clause
 *
 */

#include "offsets.h"

int WSOffsets::pCTFPlayer__m_bGlowEnabled = 0;
int WSOffsets::pCTFPlayer__m_iTeamNum = 0;
int WSOffsets::pCTFPlayer__m_iClass = 0;
int WSOffsets::pCTFPlayer__m_hMyWeapons = 0;
int WSOffsets::pCWeaponMedigun__m_flChargeLevel = 0;
int WSOffsets::pCWeaponMedigun__m_iItemDefinitionIndex = 0;

//=================================================================================
// Find the offsets for all stored NetVars
// TODO: change to bool when an offset can't be found
//=================================================================================
void WSOffsets::PrepareOffsets() {
	WSOffsets::pCTFPlayer__m_bGlowEnabled = WSOffsets::FindOffsetOfClassProp("CTFPlayer", "m_bGlowEnabled");
	WSOffsets::pCTFPlayer__m_iTeamNum = WSOffsets::FindOffsetOfClassProp("CTFPlayer", "m_iTeamNum");
	WSOffsets::pCTFPlayer__m_iClass = WSOffsets::FindOffsetOfClassProp("CTFPlayer", "m_iClass");
	WSOffsets::pCTFPlayer__m_hMyWeapons = WSOffsets::FindOffsetOfArrayEnt("CTFPlayer", "m_hMyWeapons", 1);
	WSOffsets::pCWeaponMedigun__m_flChargeLevel = WSOffsets::FindOffsetOfClassProp("CWeaponMedigun", "m_flChargeLevel");
	WSOffsets::pCWeaponMedigun__m_iItemDefinitionIndex = WSOffsets::FindOffsetOfClassProp("CBaseCombatWeapon", "m_iItemDefinitionIndex");
}

//=================================================================================
// Loop through all server classes until className is found, then crawl through the
// class to find the property
// TODO: return -1 when an offset is not found
//=================================================================================
int WSOffsets::FindOffsetOfClassProp(const char *className, const char *propName) {
	//ServerClass *sc = serverGameDLL->GetAllServerClasses();
	ClientClass *sc = ifaces.GetBaseClientDLL()->GetAllClasses();
	while (sc) {
		if (Q_strcmp(sc->GetName(), className) == 0) {
			//SendTable *sTable = sc->m_pTable;
			RecvTable *sTable = sc->m_pRecvTable;
			if (sTable) {
				int offset = 0;
				bool found = WSOffsets::CrawlForPropOffset(sTable, propName, offset);
				if (!found)
					offset = 0;
				return offset;
			}
		}
		sc = sc->m_pNext;
	}
	return 0;
}

int WSOffsets::FindOffsetOfArrayEnt(const char *classname, const char *arrayName, int element) {
	ClientClass *cc = ifaces.GetBaseClientDLL()->GetAllClasses();
	while (cc) {
		if (Q_strcmp(cc->GetName(), classname) == 0) {
			RecvTable *rTable = cc->m_pRecvTable;
			if (rTable) {
				int offset = 0;
				bool found = WSOffsets::CrawlForArrayEnt(rTable, arrayName, element, offset);
				if (!found)
					offset = 0;
				return offset;
			}
		}
		cc = cc->m_pNext;
	}

	return 0;
}

//=================================================================================
// Search through a class table, and any subtables, for a given property name
//=================================================================================
bool WSOffsets::CrawlForPropOffset(RecvTable *sTable, const char *propName, int &offset) {
	for (int i=0; i < sTable->GetNumProps(); i++) {
		//SendProp *sProp = sTable->GetProp(i);
		RecvProp *sProp = sTable->GetProp(i);
		if (strcmp(sProp->GetName(),"000") == 0) //End of an array
			continue;

		//SendTable *sChildTable = sProp->GetDataTable();
		RecvTable *sChildTable = sProp->GetDataTable();

		//Check if it is an array, don't care for these atm so skip them
		bool isArray = false;
		if (sChildTable && sChildTable->GetNumProps() > 0) {
			if (   !strcmp(sChildTable->GetProp(0)->GetName(), "000")
				|| !strcmp(sChildTable->GetProp(0)->GetName(), "lengthproxy"))
				isArray = true;
		}

		if (!isArray) {
			//If we have our property, add to the offset and start returning
			if (strcmp(sProp->GetName(), propName) == 0) {
				offset += sProp->GetOffset();
				return true;
			}
			
			//If we find a subtable, search it for the property, 
			//but keep current offset in case it isn't found here
			if (sProp->GetType() == DPT_DataTable) {
				int origOffset = offset;
				offset += sProp->GetOffset();
				bool found = WSOffsets::CrawlForPropOffset(sChildTable, propName, offset);
				if (found) {
					return true;
				} else {
					offset = origOffset;
				}
			}
		} else {
			continue;
		}

		if (strcmp(sProp->GetName(), "000") == 0) //More array stuff from dumping function, may not be needed here
			break;
	}
	return false;
}

bool WSOffsets::CrawlForArrayEnt(RecvTable *sTable, const char *propName, int element, int &offset) {
	for (int i=0; i < sTable->GetNumProps(); i++) {
		RecvProp *sProp = sTable->GetProp(i);
		if (strcmp(sProp->GetName(),"000") == 0) //End of an array
			continue;

		//SendTable *sChildTable = sProp->GetDataTable();
		RecvTable *sChildTable = sProp->GetDataTable();

		//Check if it is an array, don't care for these atm so skip them
		bool isArray = false;
		if (sChildTable && sChildTable->GetNumProps() > 0) {
			if (   !strcmp(sChildTable->GetProp(0)->GetName(), "000")
				|| !strcmp(sChildTable->GetProp(0)->GetName(), "lengthproxy"))
				isArray = true;
		}

		if (!isArray) {
			//If we have our property, add to the offset and start returning
			if (strcmp(sProp->GetName(), propName) == 0) {
				offset += sProp->GetOffset();
				return true;
			}
			
			//If we find a subtable, search it for the property, 
			//but keep current offset in case it isn't found here
			if (sProp->GetType() == DPT_DataTable) {
				int origOffset = offset;
				offset += sProp->GetOffset();
				bool found = WSOffsets::CrawlForArrayEnt(sChildTable, propName, element, offset);
				if (found) {
					return true;
				} else {
					offset = origOffset;
				}
			}
		} else {
			if (strcmp(sProp->GetName(), propName) != 0)
				continue;

			//We have our array
			offset += sProp->GetOffset();

			int elements = sProp->GetDataTable()->GetNumProps();
			if (element < 0 || element >= elements)
				return false;

			offset += sProp->GetDataTable()->GetProp(element)->GetOffset();
			return true;
		}

		if (strcmp(sProp->GetName(), "000") == 0) //More array stuff from dumping function, may not be needed here
			break;
	}
	return false;
}
