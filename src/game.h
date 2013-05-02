#pragma once

//
// TF2 specific entity stuff
//

#include "sdk.h"

enum tfclass_t
{
	TFCLASS_CIVILIAN = 0,
	TFCLASS_SCOUT = 1,
	TFCLASS_SNIPER = 2,
	TFCLASS_SOLDIER = 3,
	TFCLASS_DEMOMAN = 4,
	TFCLASS_MEDIC = 5,
	TFCLASS_HEAVY = 6,
	TFCLASS_PYRO = 7,
	TFCLASS_SPY = 8,
	TFCLASS_ENGINEER = 9,
};

int GetObserverTarget( IClientEntity* pl );

class PlayerResource
{
public:
	int& iPlayerClass( int pl );
};


class TFPlayerResource : public PlayerResource
{
public:
	bool bConnected( int pl );
	int& iTeam( int pl );
	bool& bAlive( int pl );
	int& iHealth( int pl );
};