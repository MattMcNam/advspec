#ifndef HGUARD_SDK_CDLL_INT
#define HGUARD_SDK_CDLL_INT

#include "const.h"

//-----------------------------------------------------------------------------
// forward declarations
//-----------------------------------------------------------------------------
struct ClientClass;
struct model_t;
class CSentence;
struct vrect_t;
struct cmodel_t;
class IMaterial;
class CAudioSource;
class CMeasureSection;
class SurfInfo;
class ISpatialQuery;
struct cache_user_t;
class IMaterialSystem;
class VMatrix;
struct ScreenFade_t;
struct ScreenShake_t;
class CViewSetup;
class CEngineSprite;
class CGlobalVarsBase;
class CPhysCollide;
class CSaveRestoreData;
class INetChannelInfo;
struct datamap_t;
struct typedescription_t;
class CStandardRecvProxies;
struct client_textmessage_t;
class IAchievementMgr;
class CGamestatsData;

class Vector;
class QAngle;
struct matrix3x4_t;
struct OcclusionParams_t;



struct player_info_t
{
	// scoreboard information
	char			name[MAX_PLAYER_NAME_LENGTH];
	// local server user ID, unique while server is running
	int				userID;
	// global unique player identifer
	char			guid[SIGNED_GUID_LEN + 1];
	// friends identification number
	unsigned int	friendsID;
	// friends name
	char			friendsName[MAX_PLAYER_NAME_LENGTH];
	// true, if player is a bot controlled by game.dll
	bool			fakeplayer;
	// true if player is the HLTV proxy
	bool			ishltv;
	// custom files CRC for this player
	unsigned int	customFiles[MAX_CUSTOM_FILES];
	// this counter increases each time the server downloaded a new file
	unsigned char	filesDownloaded;
};


class IVEngineClient
{
public:
	virtual int				GetIntersectingSurfaces( const model_t *model, const Vector &vCenter, const float radius, const bool bOnlyVisibleSurfaces, SurfInfo *pInfos, const int nMaxInfos ) = 0;
	virtual Vector			GetLightForPoint (const Vector &pos, bool bClamp) = 0;
	virtual IMaterial			*TraceLineMaterialAndLighting( const Vector &start, const Vector &end, Vector &diffuseLightColor, Vector& baseColor ) = 0;
	virtual const char			*ParseFile( const char *data, char *token, int maxlen ) = 0;
	virtual bool				CopyFile( const char *source, const char *destination ) = 0;
	virtual void				GetScreenSize( int& width, int& height ) = 0;
	virtual void				ServerCmd( const char *szCmdString, bool bReliable = true ) = 0;
	virtual void				ClientCmd( const char *szCmdString ) = 0;
	virtual bool				GetPlayerInfo( int ent_num, player_info_t *pinfo ) = 0;
	virtual int				GetPlayerForUserID( int userID ) = 0;
	virtual client_textmessage_t *TextMessageGet( const char *pName ) = 0;
	virtual bool				Con_IsVisible( void ) = 0;
	virtual int				GetLocalPlayer( void ) = 0;
	virtual const model_t		*LoadModel( const char *pName, bool bProp = false ) = 0;
	virtual float				Time( void ) = 0; 
	virtual float				GetLastTimeStamp( void ) = 0; 
	virtual CSentence			*GetSentence( CAudioSource *pAudioSource ) = 0;
	virtual float				GetSentenceLength( CAudioSource *pAudioSource ) = 0;
	virtual bool				IsStreaming( CAudioSource *pAudioSource ) const = 0;
	virtual void				GetViewAngles( QAngle& va ) = 0;
/*20*/virtual void				SetViewAngles( QAngle& va ) = 0;
	virtual int				GetMaxClients( void ) = 0;
	virtual	const char			*Key_LookupBinding( const char *pBinding ) = 0;
	virtual const char			*Key_BindingForKey( int &code ) = 0;
	virtual void				StartKeyTrapMode( void ) = 0;
	virtual bool				CheckDoneKeyTrapping( int &code ) = 0;
	virtual bool				IsInGame( void ) = 0;
	virtual bool				IsConnected( void ) = 0;
	virtual bool				IsDrawingLoadingImage( void ) = 0;
	virtual void				Con_NPrintf( int pos, const char *fmt, ... ) = 0;
	virtual void				Con_NXPrintf( const struct con_nprint_s *info, const char *fmt, ... ) = 0;
	virtual int				IsBoxVisible( const Vector& mins, const Vector& maxs ) = 0;
	virtual int				IsBoxInViewCluster( const Vector& mins, const Vector& maxs ) = 0;
	virtual bool				CullBox( const Vector& mins, const Vector& maxs ) = 0;
	virtual void				Sound_ExtraUpdate( void ) = 0;
	virtual const char			*GetGameDirectory( void ) = 0;
	virtual const VMatrix& 		WorldToScreenMatrix() = 0;
	virtual const VMatrix& 		WorldToViewMatrix() = 0;
	virtual int					GameLumpVersion( int lumpId ) const = 0;
	virtual int					GameLumpSize( int lumpId ) const = 0;
/*40*/virtual bool				LoadGameLump( int lumpId, void* pBuffer, int size ) = 0;
	virtual int					LevelLeafCount() const = 0;
	virtual ISpatialQuery*		GetBSPTreeQuery() = 0;

	virtual void		LinearToGamma( float* linear, float* gamma ) = 0;
	virtual float		LightStyleValue( int style ) = 0;
	virtual void		ComputeDynamicLighting( const Vector& pt, const Vector* pNormal, Vector& color ) = 0;
	virtual void		GetAmbientLightColor( Vector& color ) = 0;
	virtual int			GetDXSupportLevel() = 0;
	virtual bool        SupportsHDR() = 0;
	virtual void		Mat_Stub( IMaterialSystem *pMatSys ) = 0;
	virtual void		GetChapterName( char *pchBuff, int iMaxLength ) = 0;
	virtual char const	*GetLevelName( void ) = 0;

	//NEW 4-15-2011
	virtual void*		GetLevelVersion(void) = 0;
	//NEW

	virtual struct IVoiceTweak_s *GetVoiceTweakAPI( void ) = 0;
	virtual void		EngineStats_BeginFrame( void ) = 0;
	virtual void		EngineStats_EndFrame( void ) = 0;
	virtual void		FireEvents() = 0;
	virtual int			GetLeavesArea( int *pLeaves, int nLeaves ) = 0;
	virtual bool		DoesBoxTouchAreaFrustum( const Vector &mins, const Vector &maxs, int iArea ) = 0;
	virtual void		SetAudioState( const struct AudioState_t& state ) = 0;
/*60*/virtual int			SentenceGroupPick( int groupIndex, char *name, int nameBufLen ) = 0;
	virtual int			SentenceGroupPickSequential( int groupIndex, char *name, int nameBufLen, int sentenceIndex, int reset ) = 0;
	virtual int			SentenceIndexFromName( const char *pSentenceName ) = 0;
	virtual const char *SentenceNameFromIndex( int sentenceIndex ) = 0;
	virtual int			SentenceGroupIndexFromName( const char *pGroupName ) = 0;
	virtual const char *SentenceGroupNameFromIndex( int groupIndex ) = 0;
	virtual float		SentenceLength( int sentenceIndex ) = 0;
	virtual void		ComputeLighting( const Vector& pt, const Vector* pNormal, bool bClamp, Vector& color, Vector *pBoxColors=NULL ) = 0;
	virtual void		ActivateOccluder( int nOccluderIndex, bool bActive ) = 0;
	virtual bool		IsOccluded( const Vector &vecAbsMins, const Vector &vecAbsMaxs ) = 0;
	virtual void		*SaveAllocMemory( size_t num, size_t size ) = 0;
	virtual void		SaveFreeMemory( void *pSaveMem ) = 0;
	virtual INetChannelInfo	*GetNetChannelInfo( void ) = 0;
	virtual void		DebugDrawPhysCollide( const CPhysCollide *pCollide, IMaterial *pMaterial, matrix3x4_t& transform, const int &color ) = 0;
	virtual void		CheckPoint( const char *pName ) = 0;
	virtual void		DrawPortals() = 0;
	virtual bool		IsPlayingDemo( void ) = 0;
	virtual bool		IsRecordingDemo( void ) = 0;
	virtual bool		IsPlayingTimeDemo( void ) = 0;

	// UNKNOWN RETURN VALUES - NEW FUNCTIONS
	virtual void		*GetDemoRecordingTick ( void ) = 0;
/*80*/virtual void		*GetDemoPlaybackTick ( void ) = 0;
	virtual void		*GetDemoPlaybackStartTick ( void ) = 0;
	virtual void		*GetDemoPlaybackTimeScale ( void ) = 0;
	virtual void		*GetDemoPlaybackTotalTicks ( void ) = 0;
	// END

	virtual bool		IsPaused( void ) = 0;
	virtual bool		IsTakingScreenshot( void ) = 0;
	virtual bool		IsHLTV( void ) = 0;

	/*/ 
	// Removed 4-15-2011
	virtual bool		IsReplay( void ) = 0;
	virtual void		*DownloadReplay ( int unknown1 ) = 0;
	virtual bool		IsReplayEnabled ( void ) = 0;
	virtual bool		IsReplayRecording ( void ) = 0;
	virtual void		*GetReplayRecordingTick ( void ) = 0;
	/END/*/ 

	virtual bool		IsLevelMainMenuBackground( void ) = 0;
	virtual void		GetMainMenuBackgroundName( char *dest, int destlen ) = 0;
	virtual void		*GetVideoModes ( int& iUnknown, void *vmode_s ) = 0;
	virtual void		SetOcclusionParameters( const OcclusionParams_t &params ) = 0;
	virtual void		GetUILanguage( char *dest, int destlen ) = 0;
	virtual int IsSkyboxVisibleFromPoint( const Vector &vecPoint ) = 0;
	virtual const char*	GetMapEntitiesString() = 0;
	virtual bool		IsInEditMode( void ) = 0;
	virtual float		GetScreenAspectRatio() = 0;
	virtual bool		REMOVED_SteamRefreshLogin( const char *password, bool isSecure ) = 0;
	virtual bool		REMOVED_SteamProcessCall( bool & finished ) = 0;

	virtual unsigned int	GetEngineBuildNumber() = 0; // engines build
	virtual const char *	GetProductVersionString() = 0; // mods version number (steam.inf)
	virtual void			GrabPreColorCorrectedFrame( int x, int y, int width, int height ) = 0;
	virtual bool			IsHammerRunning( ) const = 0;
	virtual void			ExecuteClientCmd( const char *szCmdString ) = 0;
	virtual bool			MapHasHDRLighting(void) = 0;
	virtual int			GetAppID() = 0;
	virtual Vector		GetLightForPointFast(const Vector &pos, bool bClamp) = 0;
	virtual void			ClientCmd_Unrestricted( const char *szCmdString ) = 0;
	virtual void			SetRestrictServerCommands( bool bRestrict ) = 0;
	virtual void			SetRestrictClientCommands( bool bRestrict ) = 0;
	virtual void			SetOverlayBindProxy( int iOverlayID, void *pBindProxy ) = 0;
	virtual bool			CopyFrameBufferToMaterial( const char *pMaterialName ) = 0;
	virtual void			ChangeTeam( const char *pTeamName ) = 0;
	virtual void			ReadConfiguration( const bool readDefault = false ) = 0; 
	virtual void			SetAchievementMgr( IAchievementMgr *pAchievementMgr ) = 0;
	virtual IAchievementMgr *GetAchievementMgr() = 0;
	virtual bool			MapLoadFailed( void ) = 0;
	virtual void			SetMapLoadFailed( bool bState ) = 0;
	virtual bool			IsLowViolence() = 0;
	virtual const char		*GetMostRecentSaveGame( void ) = 0;
	virtual void			SetMostRecentSaveGame( const char *lpszFilename ) = 0;
	virtual void			StartXboxExitingProcess() = 0;
	virtual bool			IsSaveInProgress() = 0;
	virtual unsigned int	OnStorageDeviceAttached( void ) = 0;
	virtual void			OnStorageDeviceDetached( void ) = 0;
	virtual void			ResetDemoInterpolation( void ) = 0;
	virtual void SetGamestatsData( CGamestatsData *pGamestatsData ) = 0;
	virtual CGamestatsData *GetGamestatsData() = 0;

	// Removed 4-15-2011
	//virtual void			LoadFilmmaker ( void ) = 0;
	//virtual void			UnloadFilmmaker ( void ) = 0;
	virtual void			*GetMouseDelta ( int&, int&, bool ) = 0;
	virtual void			ServerCmdKeyValues ( void* ) = 0;
	
	// New 4-15-2011
	virtual bool			IsSkippingPlayback ( void ) = 0;
	// New 4-15-2011
	virtual bool			IsLoadingDemo ( void ) = 0;

	virtual bool			IsInCommentaryMode ( void ) = 0;
};



class IBaseClientDLL
{
public:
	// Called once when the client DLL is loaded
	virtual int			Init( CreateInterfaceFn appSystemFactory, 
								CreateInterfaceFn physicsFactory,
								class CGlobalVarsBase *pGlobals ) = 0;

	virtual void			PostInit() = 0;

	// Called once when the client DLL is being unloaded
	virtual void			Shutdown( void ) = 0;
	
	// Called at the start of each level change
	virtual void			LevelInitPreEntity( char const* pMapName ) = 0;
	// Called at the start of a new level, after the entities have been received and created
	virtual void			LevelInitPostEntity( ) = 0;
	// Called at the end of a level
	virtual void			LevelShutdown( void ) = 0;

	virtual void Unknown006() = 0;
	virtual void Unknown007() = 0;

	// Request a pointer to the list of client datatable classes
	virtual ClientClass		*GetAllClasses( void ) = 0;

	// Called once per level to re-initialize any hud element drawing stuff
	virtual int				HudVidInit( void ) = 0;
	// Called by the engine when gathering user input
	virtual void			HudProcessInput( bool bActive ) = 0;
	// Called oncer per frame to allow the hud elements to think
	virtual void			HudUpdate( bool bActive ) = 0;
	// Reset the hud elements to their initial states
	virtual void			HudReset( void ) = 0;
	// Display a hud text message
	virtual void			HudText( const char * message ) = 0;


	// More...
};


#endif // !HGUARD_SDK_CDLL_INT
