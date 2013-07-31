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

#include "cdll_client_int.h"
#include "client_class.h"

extern IBaseClientDLL *pClient;

class WSOffsets {
public:
	static int pCTFPlayer__m_iTeamNum;
	static int pCTFPlayer__m_bGlowEnabled;
	static int pCTFPlayer__m_puiGlowindex;

	static void PrepareOffsets();
	static int FindOffsetOfClassProp(const char *className, const char *propName);

private:
	static bool CrawlForPropOffset(RecvTable *sTable, const char *propName, int &offset);
};

#define MakePtr( cast, ptr, addValue ) (cast)( (unsigned long)(ptr) + (unsigned long)(addValue))

#endif
