#include "stdafx.h"

#include <cstdlib>

#include "vmthooks.h"


unsigned int CountFuncs( void** vmt )
{
	unsigned int i = -1;
	do ++i; while ( vmt[i] );
	return i;
}


// VMTBaseManager
VMTBaseManager& VMTBaseManager::Init( void* inst, unsigned int offset, unsigned int vfuncs )
{
	_vftable = &_getvtbl( inst, offset );
	_oldvmt = *_vftable;
	// Count vfuncs ourself if needed
	if ( !vfuncs )
	{
		vfuncs = CountFuncs( _oldvmt );
		assert( vfuncs>=1 );
	}
	// Allocate room for the new vtable + 3 places before + 1 place after for a null ptr
	void** arr = _array = (void**) ::malloc( (vfuncs+4)*sizeof(void*) );
	arr[0] = this; // Ptr back to the hook object
	arr[1] = (void*)GUARD; // Guard tells us if the vtable is hooked
	(arr+3)[vfuncs] = NULL; // Marks the end of the vtable
	// Copy over the other vfuncs + RTTI ptr
	{
		int i = -1;
		do arr[i+3] = _oldvmt[i];
		while ( ++i<static_cast<int>(vfuncs) );
	}
	return *this;
}
void VMTBaseManager::Kill()
{
	if ( _vftable )
	{
		Unhook();
		_vftable = NULL;
	}
	::free( _array );
	_array = NULL;
}
void VMTBaseManager::EraseHooks()
{
	unsigned int i = 0;
	void** vmt = _array+3;
	do vmt[i] = _oldvmt[i];
	while ( vmt[++i] );
}
unsigned int VMTBaseManager::CountNumFuncs() const
{
	// Used to be stored in the hook.
	// Since it's not needed aside from a few asserts, might as well count them every time.
	unsigned int i;
	void** vmt = _array+3;
	for ( i = 0; vmt[i]; ++i );
	return i;
}
