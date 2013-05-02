#ifndef HGUARD_SDK_CONVAR
#define HGUARD_SDK_CONVAR



// ConVar flags
#define FCVAR_NONE				0 

#define FCVAR_UNREGISTERED		(1<<0)
#define FCVAR_DEVELOPMENTONLY	(1<<1)
#define FCVAR_GAMEDLL			(1<<2)
#define FCVAR_CLIENTDLL			(1<<3)
#define FCVAR_HIDDEN			(1<<4)

#define FCVAR_PROTECTED			(1<<5)
#define FCVAR_SPONLY			(1<<6)
#define	FCVAR_ARCHIVE			(1<<7)
#define	FCVAR_NOTIFY			(1<<8)
#define	FCVAR_USERINFO			(1<<9)
#define FCVAR_CHEAT				(1<<14)

#define FCVAR_PRINTABLEONLY		(1<<10)
#define FCVAR_UNLOGGED			(1<<11)
#define FCVAR_NEVER_AS_STRING	(1<<12)
#define FCVAR_RELEASE			(1<<19)

#define FCVAR_REPLICATED		(1<<13)
#define FCVAR_DEMO				(1<<16)
#define FCVAR_DONTRECORD		(1<<17)

#define FCVAR_NOT_CONNECTED		(1<<22)
#define FCVAR_ARCHIVE_XBOX		(1<<24)
#define FCVAR_SERVER_CAN_EXECUTE	(1<<28)
#define FCVAR_SERVER_CANNOT_QUERY	(1<<29)
#define FCVAR_CLIENTCMD_CAN_EXECUTE	(1<<30)






// 10 vfuncs
class ConCommandBase
{
public:
	ConCommandBase( const char* name, const char* helpstr, int flags = 0 );
	virtual ~ConCommandBase( void );

	virtual	bool IsCommand( void ) const;
	virtual bool IsFlagSet( int flag ) const;
	virtual void AddFlags( int flags );
	virtual const char* GetName( void ) const;
	virtual const char* GetHelpText( void ) const;
	virtual bool IsRegistered( void ) const;
	virtual int	GetDLLIdentifier() const;
	virtual void Create( const char *pName, const char *pHelpString = 0, int flags = 0 );
	virtual void Init();

public:
	ConCommandBase* pNext;
	bool bRegistered;
	const char* pszName;
	const char* pszHelpString;
	int nFlags;
public:
	static int s_nDllIdentifier;
};




class IConVar;

typedef void (* FnChangeCallback_t)( IConVar* var, const char* pOldValue, float flOldValue );

class IConVar
{
public:
	// Value set
	virtual void SetInt( int nValue ) = 0;
	virtual void SetFloat( float flValue ) = 0;
	virtual void SetString( const char *pValue ) = 0;
	// Return name of command
	virtual const char* GetName( void ) const = 0;
	// Accessors...
	virtual bool IsFlagSet( int nFlag ) const = 0;
};
class ConVar : public ConCommandBase, public IConVar
{
public:
	ConVar( const char* name, const char* helpstr, int flags = 0, const char* defval = "" );

	// Inherited from ConCommandBase
	virtual	bool IsCommand( void ) const;

	// Inherited from IConVar
	virtual void SetString( const char *pValue );
	virtual void SetFloat( float flValue );
	virtual void SetInt( int nValue );
	virtual const char* GetName( void ) const;
	virtual bool IsFlagSet( int nFlag ) const;
	
	// ConVar virtuals
	virtual void InternalSetString( const char* str );
	virtual void InternalSetFloat( float fl );
	virtual void InternalSetInt( int i );
	virtual bool ClampValue( float& f );
	virtual void ChangeStringValue( const char* temp, float old );
	virtual void Create( const char*, const char*, int, const char*, bool, float, bool, float, FnChangeCallback_t );

	// Inline accessors
	inline int GetInt() const { return nValue; }
	inline float GetFloat() const { return fValue; }
	inline bool GetBool() const { return nValue!=0; }
	inline const char* GetString() const { return pszString; }

	// Change value without invoking the change callback.
	void ForceSetString( const char* str );
	void ForceSetFloat( float fl );
	void ForceSetInt( int i );

public:
	ConVar* pParent;
	const char* pszDefaultValue;
	char* pszString;
	int StringLength;
	float fValue;
	int nValue;
	bool bHasMin;
	float fMinVal;
	bool bHasMax;
	float fMaxVal;
	FnChangeCallback_t fnChangeCallback;
};






// Also known as CCommand but that's a horrible name
struct CCmdArgs
{
	int ArgC() const;
	const char *Arg( int nIndex ) const;

	enum
	{
		COMMAND_MAX_ARGC = 64,
		COMMAND_MAX_LENGTH = 512,
	};

	int		nArgc;
	int		nArgv0Size;
	char	pArgSBuffer[ COMMAND_MAX_LENGTH ];
	char	pArgvBuffer[ COMMAND_MAX_LENGTH ];
	const char*	ppArgv[ COMMAND_MAX_ARGC ];
};

inline int CCmdArgs::ArgC() const
{
	return nArgc;
}

inline const char *CCmdArgs::Arg( int nIndex ) const
{
	// FIXME: Many command handlers appear to not be particularly careful
	// about checking for valid argc range. For now, we're going to
	// do the extra check and return an empty string if it's out of range
	if ( nIndex < 0 || nIndex >= nArgc )
		return "";
	return ppArgv[nIndex];
}

typedef void (* CommandCallbackFn)( const CCmdArgs& );
// 3 extra vfuncs (13 total)
class ConCommand : public ConCommandBase
{
public:
	ConCommand( const char* name, const char* helpstr, int flags, CommandCallbackFn cb );

	virtual int AutoCompleteSuggest( const char *partial, /*CUtlVector<CUtlString>&*/ void* args );
	virtual bool CanAutoComplete( void );
	virtual void Dispatch( const CCmdArgs &args );

public:
	CommandCallbackFn fnCommandCallback;
	void* fnCompletionCallback;
	int iType;
};









// Inlines

inline void ConVar::ForceSetString( const char* str )
{
	FnChangeCallback_t old = fnChangeCallback;
	fnChangeCallback = NULL;
	InternalSetString( str );
	fnChangeCallback = old;
}
inline void ConVar::ForceSetFloat( float fl )
{
	FnChangeCallback_t old = fnChangeCallback;
	fnChangeCallback = NULL;
	InternalSetFloat( fl );
	fnChangeCallback = old;
}
inline void ConVar::ForceSetInt( int i )
{
	FnChangeCallback_t old = fnChangeCallback;
	fnChangeCallback = NULL;
	InternalSetInt( i );
	fnChangeCallback = old;
}


#endif // !HGUARD_SDK_CONVAR
