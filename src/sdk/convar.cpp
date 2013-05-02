
#include <cstdlib>
#include <cstring>
#include <cstdio>

#include "sdk/convar.h"
#include "sdk/icvar.h"
#include "ifaces.h"

int ConCommandBase::s_nDllIdentifier = 0;

ConCommandBase::ConCommandBase( const char* name, const char* helpstr, int flags ) : pNext(NULL),
	bRegistered(false), pszName(name), pszHelpString(helpstr), nFlags(flags)
{
	// Realloc the name
	size_t len = ::strlen( name )+1;
	char* buf = (char*) ::malloc( len );
	::memcpy( buf, name, len );

	pszName = buf;
}
ConCommandBase::~ConCommandBase()
{
	// Free the name
	::free( const_cast<char*>(pszName) );
}
bool ConCommandBase::IsCommand( void ) const
{
	// Game says so...
	return true;
}
bool ConCommandBase::IsFlagSet( int flag ) const
{
	return ( nFlags & flag )!=0;
}
void ConCommandBase::AddFlags( int flags )
{
	nFlags |= flags;
}
const char* ConCommandBase::GetName( void ) const
{
	return pszName;
}
const char* ConCommandBase::GetHelpText( void ) const
{
	return pszHelpString;
}
bool ConCommandBase::IsRegistered( void ) const
{
	return bRegistered;
}
int	ConCommandBase::GetDLLIdentifier() const
{
	// If you're curious what this is for, it's simple:
	//
	// Normally if you use the Source SDK libs all your ConCommands and ConVars are constructed and put in a linked list in your own .dll
	// Using the SDK func ConVar_Register it loops through this list and adds them to the global cvar list. The linked list in your own dll is now gone.
	//
	// To unregister all your ConVars it calls pCvar->UnregisterConCommands( s_nDllIdentifier ) which then magically removes all your ConVars.
	//
	// Note that I add my ConVars manually making this rather useless but oh well...
	//

	if ( !s_nDllIdentifier )
	{
		s_nDllIdentifier = Ifaces.pCvar->AllocateDLLIdentifier();
	}
	return s_nDllIdentifier;
}
void ConCommandBase::Create( const char *pName, const char *pHelpString, int flags )
{
#ifndef _LINUX
	// Not used
	__asm int 3
#endif
}
void ConCommandBase::Init()
{
#ifndef _LINUX
	// Not used
	__asm int 3
#endif
}





ConVar::ConVar( const char* name, const char* helpstr, int flags, const char* defval ) : ConCommandBase( name, helpstr, flags ),
	pParent(this), pszDefaultValue(defval), pszString(NULL), StringLength(0), bHasMin(false), fMinVal(0.0f), bHasMax(false), fMaxVal(0.0f), fnChangeCallback(NULL)
{
	InternalSetString( defval );
}
// ConCommandBase funcs
bool ConVar::IsCommand() const
{
	return false;
}
// IConVar funcs
void ConVar::SetString( const char* value )
{
	pParent->InternalSetString( value );
}
void ConVar::SetFloat( float value )
{
	pParent->InternalSetFloat( value );
}
void ConVar::SetInt( int value )
{
	pParent->InternalSetInt( value );
}
const char* ConVar::GetName( void ) const
{
	return pParent->pszName;
}
bool ConVar::IsFlagSet( int flag ) const
{
	return ( pParent->nFlags & flag )!=0;
}
// ConVar funcs
void ConVar::InternalSetString( const char* str )
{
	// Need this for onchange later
	char* pOldVal;
	float flOldVal;
	if ( fnChangeCallback )
	{
		flOldVal = fValue;
		pOldVal = (char*) ::malloc( StringLength );
		memcpy( pOldVal, pszString, StringLength );
	}

	int len = ::strlen( str )+1;
	// Realloc if needed
	{
		int size = len;
		if ( size<20 ) size = 20;
		if ( size>StringLength )
		{
			StringLength = size;
			::free( pszString );
			pszString = (char*) ::malloc( size );
		}
	}
	// Set new value
	memcpy( pszString, str, len );
	// Update integral values
	double fl = ::atof( str );
	fValue = static_cast<float>( fl );
	nValue = static_cast<int>( fl );

	// Notify callback
	if ( fnChangeCallback )
	{
		fnChangeCallback( this, pOldVal, flOldVal );
		::free( pOldVal );
	}
}
void ConVar::InternalSetFloat( float fl )
{
	fValue = fl;
	nValue = static_cast<int>( fl );
	sprintf( pszString, "%f", fl );
}
void ConVar::InternalSetInt( int i )
{
	nValue = i;
	fValue = static_cast<float>( i );
	sprintf( pszString, "%i", i );
}
bool ConVar::ClampValue( float& f )
{
	if ( bHasMin && f<fMinVal )
	{
		f = fMinVal;
		return false;
	}
	if ( bHasMax && f>fMaxVal )
	{
		f = fMaxVal;
		return false;
	}
	return true;
}
void ConVar::ChangeStringValue( const char*, float )
{
#ifndef _LINUX
	// Not used
	__asm int 3
#endif
}
void ConVar::Create( const char*, const char*, int, const char*, bool, float, bool, float, FnChangeCallback_t )
{
#ifndef _LINUX
	// Not used
	__asm int 3
#endif
}

ConCommand::ConCommand( const char* name, const char* helpstr, int flags, CommandCallbackFn cb ) : ConCommandBase( name, helpstr, flags ),
	fnCommandCallback(cb), fnCompletionCallback(NULL), iType(2)
{
}
int ConCommand::AutoCompleteSuggest( const char *partial, /*CUtlVector<CUtlString>&*/ void* args )
{
	return 0;
}
bool ConCommand::CanAutoComplete( void )
{
	return false;
}
void ConCommand::Dispatch( const CCmdArgs &args )
{
	fnCommandCallback( args );
}

