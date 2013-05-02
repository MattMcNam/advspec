#ifndef HGUARD_SDK_RECVSTUFF
#define HGUARD_SDK_RECVSTUFF

struct RecvTable;
struct RecvProp;
struct CRecvDecoder;

enum SendPropType
{
	DPT_INT,
	DPT_FLOAT,
	DPT_VECTOR,
	DPT_STRING,
	DPT_ARRAY,
	DPT_DATATABLE,
};

struct DVariant
{
	union
	{
		float Float;
		long Int;
		char* pString;
		void* pData;	// For DataTables.
		float Vector[3];
	};
	SendPropType Type;
};

// This is passed into RecvProxy functions.
struct CRecvProxyData
{
	// The property it's receiving.
	const RecvProp* pRecvProp;
	// The value given to you to store.
	DVariant Value;
	// Which array element you're getting.
	int iElement;
	// The object being referred to.
	int ObjectID;
};


typedef void (*RecvVarProxyFn)( const CRecvProxyData *pData, void *pStruct, void *pOut );
typedef void (*ArrayLengthRecvProxyFn)( void *pStruct, int objectID, int currentArrayLength );
typedef void (*DataTableRecvVarProxyFn)(const RecvProp *pProp, void **pOut, void *pData, int objectID);



struct RecvProp
{
	char* pVarName;
	SendPropType RecvType;
	int Flags;
	int StringBufferSize;

	// Set to true by the engine if this property sits inside an array.
	bool bInsideArray;

	// Extra data that certain special property types bind to the property here.
	const void* pExtraData;

	// If this is an array (DPT_Array).
	RecvProp* pArrayProp;
	ArrayLengthRecvProxyFn ArrayLengthProxy;
	
	RecvVarProxyFn ProxyFn;
	DataTableRecvVarProxyFn DataTableProxyFn;	// For RDT_DataTable.

	RecvTable* pDataTable;		// For RDT_DataTable.
	int Offset;
	
	int ElementStride;
	int nElements;

	// If it's one of the numbered "000", "001", etc properties in an array, then
	// these can be used to get its array property name for debugging.
	const char* pParentArrayPropName;
};
struct RecvTable
{
	// Properties described in a table.
	RecvProp* pProps;
	int nProps;

	// The decoder. NOTE: this covers each RecvTable AND all its children (ie: its children
	// will have their own decoders that include props for all their children).
	CRecvDecoder* pDecoder;

	const char* pNetTableName;	// The name matched between client and server.

	bool bInitialized;
	bool bInMainList;
};

// The serial number that gets passed in is used for ehandles.
typedef IClientNetworkable*	(*CreateClientClassFn)( int entnum, int serialNum );
typedef IClientNetworkable*	(*CreateEventFn)();

struct ClientClass
{
	CreateClientClassFn pCreateFn;
	CreateEventFn pCreateEventFn;	// Only called for event objects.
	const char* pNetworkName;
	RecvTable* pRecvTable;
	ClientClass* pNext;
	int ClassID;	// Managed by the engine.
};

#endif // !HGUARD_SDK_RECVSTUFF
