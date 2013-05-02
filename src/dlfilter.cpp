#include "stdafx.h"

#include "dlfilter.h"
#include "util.h"
#include "ifaces.h"

CDownloadFilter CDownloadFilter::g;

bool CDownloadFilter::Init()
{
	cl_downloadfilter = Ifaces.pCvar->FindVar( "cl_downloadfilter" );

	// Signature for CL_QueueDownload
#ifndef _LINUX
	if ( char* p = (char*) SigScan( Ifaces.hmEngine,	
		"\x55\x8B\xEC\x51\x53\x56\x8B\x75\x10\x8B\xD9\x56\x89\x5D\xFC\xE8",
		"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF" ) )
	{
		if ( pInst = **(void***)( p+0x1F+2 ) )
		{
			pRetAddr = p+0x33;	  
#else
	if ( char* p = (char*) SigScan( (void*)Ifaces.pfnEngine,			
		"\x55\x89\xE5\x83\xEC\x48\x8B\x45\x10\x89\x5D\xF4\x8B\x5D\x14\x89",
		"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF" ) )
	{
		if (( pInst = **(void***)( p+0x40+1 ) ))
		{
			pRetAddr = p+0x5D;		
#endif
			hook.Init( pInst, 0x4 );
			hook.HookMethod( (void*)&HookedFileExists, 10 );
			hook.UserData( this );
			hook.Rehook();
			return true;
		}
	}
	return false;
}
void CDownloadFilter::Close()
{
	hook.Kill();
}


bool __fastcall CDownloadFilter::HookedFileExists( THIS_PARAM( void* _this ), const char* file, const char* pid )
{
	hook_t& hook = hook_t::GetHook( _this );
	CDownloadFilter* me = (CDownloadFilter*) hook.UserData();

	// Check if we're being called from CL_QueueDownload
#ifndef _LINUX
	if ( _ReturnAddress()==me->pRetAddr )
#else
	if ( __builtin_return_address(0)==me->pRetAddr )
#endif
	{
		// Check if we only want maps
		if ( me->cl_downloadfilter && !strcmp( me->cl_downloadfilter->GetString(), "mapsonly" ) )
		{
			// Get extension
			const char* ext = strrchr( file, '.' );
			if ( !( ext && !strcmp( ext, ".bsp" ) ) )
			{
				// This is not a mapfile, pretend we already have the file
				return true;
			}
		}
	}
	// Fallback to real FileExists
	return hook.GetMethod<bool (__thiscall*)( void*, const char*, const char* )>( 10 )( _this, file, pid );
}
