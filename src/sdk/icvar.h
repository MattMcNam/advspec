#ifndef HGUARD_SDK_ICVAR
#define HGUARD_SDK_ICVAR

#include "sdk.h"

class ICvar : public IAppSystem
{
public:
	// Allocate a unique DLL identifier
	virtual int AllocateDLLIdentifier() = 0;

	// Register, unregister commands
	virtual void RegisterConCommand( ConCommandBase *pCommandBase ) = 0;
	virtual void UnregisterConCommand( ConCommandBase *pCommandBase ) = 0;
	virtual void UnregisterConCommands( int id ) = 0;

	// If there is a +<varname> <value> on the command line, this returns the value.
	// Otherwise, it returns NULL.
	virtual const char* GetCommandLineValue( const char *pVariableName ) = 0;

	// Try to find the cvar pointer by name
	virtual ConCommandBase *FindCommandBase( const char *name ) = 0;
	virtual const ConCommandBase *FindCommandBase( const char *name ) const = 0;
	virtual ConVar			*FindVar ( const char *var_name ) = 0;
	virtual const ConVar	*FindVar ( const char *var_name ) const = 0;
	virtual ConCommand		*FindCommand( const char *name ) = 0;
	virtual const ConCommand *FindCommand( const char *name ) const = 0;

	// Get first ConCommandBase to allow iteration
	virtual ConCommandBase	*GetCommands( void ) = 0;
	virtual const ConCommandBase *GetCommands( void ) const = 0;

	//// Install a global change callback (to be called when any convar changes) 
	//virtual void			InstallGlobalChangeCallback( FnChangeCallback_t callback ) = 0;
	//virtual void			RemoveGlobalChangeCallback( FnChangeCallback_t callback ) = 0;
	//virtual void			CallGlobalChangeCallbacks( ConVar *var, const char *pOldString, float flOldValue ) = 0;

	//// Install a console printer
	//virtual void			InstallConsoleDisplayFunc( IConsoleDisplayFunc* pDisplayFunc ) = 0;
	//virtual void			RemoveConsoleDisplayFunc( IConsoleDisplayFunc* pDisplayFunc ) = 0;
	//virtual void			ConsoleColorPrintf( const Color& clr, const char *pFormat, ... ) const = 0;
	//virtual void			ConsolePrintf( const char *pFormat, ... ) const = 0;
	//virtual void			ConsoleDPrintf( const char *pFormat, ... ) const = 0;

	//// Reverts cvars which contain a specific flag
	//virtual void			RevertFlaggedConVars( int nFlag ) = 0;

	//// Method allowing the engine ICvarQuery interface to take over
	//// A little hacky, owing to the fact the engine is loaded
	//// well after ICVar, so we can't use the standard connect pattern
	//virtual void			InstallCVarQuery( ICvarQuery *pQuery ) = 0;
};

#endif // HGUARD_SDK_ICVAR
