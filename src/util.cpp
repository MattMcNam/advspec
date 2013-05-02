#include "stdafx.h"

#include "ifaces.h"
#include "util.h"
#include "sdk/recvstuff.h"
#include "sdk/cdll_int.h"
#include "sdk/globalvars.h"

unsigned GetModuleSize( unsigned int& hmod )
{
#ifndef _LINUX
	IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*) hmod;
	IMAGE_NT_HEADERS* nt = (IMAGE_NT_HEADERS*)( ((char*)dos) + dos->e_lfanew );
	return nt->OptionalHeader.SizeOfImage;
#else
	// From AlliedMods sig scanning
	Dl_info info;
	struct stat buf;

	if(!dladdr((void*)hmod, &info))
		return 0;

	if(!info.dli_fbase || !info.dli_fname)
		return 0;

	if(stat(info.dli_fname, &buf) != 0)
		return 0;
  
	hmod = (unsigned int)info.dli_fbase;
	return buf.st_size;
#endif
}

// hmod is not a module handle on linux, but a ptr to a function inside the module. It is later changed to the module handle.
void* SigScan( void* hmod, const unsigned char* pat, const unsigned char* mask, unsigned int len )
{
	unsigned int iModuleSize = GetModuleSize( (unsigned int&)hmod );
	if(iModuleSize != 0)
	{
		// Find module end
		unsigned char* end = ((unsigned char*)hmod) + iModuleSize - len;

		// Scan for signature
		for ( unsigned char* p = (unsigned char*)hmod; p<=end; ++p )
		{
			unsigned int i = 0;
			do
			{
				if ( (p[i]&mask[i])!=pat[i] )
					goto cont;
			}
			while ( ++i<len );
			return p;
cont:;
		}
	}

	return NULL;
}


RecvProp* NetworkedSearch( RecvTable* table, const char* name )
{
	while ( table )
	{
		RecvProp* prop = table->pProps;
		RecvProp* end = prop+table->nProps;
		table = NULL;
		for ( table; prop<end; ++prop )
		{
			if ( !strcmp( prop->pVarName, name ) )
				return prop;
			if ( !strcmp( prop->pVarName, "baseclass" ) )
				table = prop->pDataTable;
		}
	}
	return NULL;
}
RecvProp* NetworkedVar( const char* cls, int* offset_opt, const char* const* vars )
{
	int dummy;
	int& offset = offset_opt?*offset_opt:dummy;
	offset = 0;

	for ( ClientClass* cc = Ifaces.pClient->GetAllClasses(); cc; cc = cc->pNext )
	{
		if ( !strcmp( cc->pNetworkName, cls ) )
		{
			RecvProp* prop;
			RecvTable* table = cc->pRecvTable;
			do
			{
				prop = NetworkedSearch( table, *vars );
				if ( !prop )
					return NULL;
				offset += prop->Offset;
				if ( !*++vars )
					return prop;
			}
			while ( (table = prop->pDataTable) );
			break;
		}
	}
	return NULL;
}

#ifndef _LINUX
bool GetInformationPointer( void* ptr, char (&buf)[64], unsigned int& off )
{
	// Paranoid debugging :DDD

	*(int*) buf = *(const int*) "ERR\0";
	off = (unsigned int)ptr;
	
	char temp[MAX_PATH];

	HMODULE hm = NULL;
	if ( ::GetModuleHandleExA( GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCSTR)ptr, &hm ) && hm )
	{
		::GetModuleFileNameA( hm, temp, MAX_PATH );

		char* name = temp;
		for ( char* it = temp; *it; ++it ) { if ( *it=='\\' ) name = it+1; }

		strcpy( buf, name );
		off = ((unsigned int)ptr) - ((unsigned int)hm);

		return true;
	}
	return false;
}
#endif

int FindPlayerInServer( const char* partial, player_info_t* pinfo )
{
	if ( Ifaces.pEngine->IsInGame() )
	{
		int maxClients = Ifaces.pGlobals->maxClients;

		// Try to find exact match
		for ( int i = 1; i<=maxClients; i++ )
		{
			if ( Ifaces.pEngine->GetPlayerInfo( i, pinfo ) && !strcmp( pinfo->name, partial ) )
			{
				return i;
			}
		}
		// Try to find a weaker match
		for ( int i = 1; i<=maxClients; i++ )
		{
			if ( Ifaces.pEngine->GetPlayerInfo( i, pinfo ) && strstr( pinfo->name, partial ) )
			{
				// Find another match
				for ( int j = i+1; j<=maxClients; j++ )
				{
					if ( Ifaces.pEngine->GetPlayerInfo( j, pinfo ) && strstr( pinfo->name, partial ) )
					{
						// Multiple matches detected
						return -2;
					}
				}
				// Unique partial match
				Ifaces.pEngine->GetPlayerInfo( i, pinfo );
				return i;
			}
		}
	}
	// Nobody by that name found
	return -1;
}


void NetPropHook::Hook( RecvProp* prop, RecvVarProxyFn hook )
{
	this->prop = prop;
	this->oldproxy = prop->ProxyFn;
	prop->ProxyFn = hook;
}
void NetPropHook::Unhook()
{
	prop->ProxyFn = oldproxy;
	prop = NULL;
	oldproxy = NULL;
}