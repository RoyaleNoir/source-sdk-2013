#include "resourcesystem/iresourcesystem.h"

#include "tier0/threadtools.h"
#include "tier0/tslist.h"
#include "tier2/tier2.h"

#include "filesystem.h"
#include "fmtstr.h"
#include "utlbuffer.h"
#include "utlsymbol.h"
#include "utlmap.h"

#include "resourcesystem/modelresource.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


// TODO: Use a factory system
extern void RegisterModelHandlers();
extern void UnregisterModelHandlers();


//-----------------------------------------------------------------------------
// ResourceSystem implementation.
//-----------------------------------------------------------------------------
class CResourceSystem : public CTier2AppSystem< IResourceSystem >
{
	typedef CTier2AppSystem< IResourceSystem > BaseClass;

public:
	CResourceSystem();
	~CResourceSystem();

	// IAppSystem
public:
	virtual bool Connect( CreateInterfaceFn factory );
	virtual void Disconnect();
	virtual void *QueryInterface( const char *pInterfaceName );
	virtual InitReturnVal_t Init();
	virtual void Shutdown();

	// IResourceSystem
public:
	virtual WeakResourceHandle_t FindOrCreateResource( const char *pszName, ResourceType_t nType );
	virtual void DestroyResource( WeakResourceHandle_t hResource );

	virtual void RegisterTypeHandler( ResourceType_t resourceType, 
		IResourceTypeHandler *pHandler );

	virtual void UnregisterTypeHandler( ResourceType_t resourceType, 
		IResourceTypeHandler *pHandler );

private:
	IResourceTypeHandler *GetTypeHandler( ResourceType_t resourceType );

	mutable CThreadSpinRWLock m_TypeHandlersLock;
	CUtlMap< ResourceType_t, IResourceTypeHandler* > m_TypeHandlers;

	CThreadFastMutex m_mutex;
	CUtlMap<CUtlSymbol, WeakResourceHandle_t> m_Resources;
	CTSQueue<WeakResourceHandle_t> m_ResourcesToDelete;

	bool m_bInitialized;
};


//-----------------------------------------------------------------------------
// Singleton instance
//-----------------------------------------------------------------------------
CResourceSystem g_ResourceSystem;
IResourceSystem *g_pResourceSystem = &g_ResourceSystem;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CResourceSystem, IResourceSystem, RESOURCESYSTEM_INTERFACE_VERSION, g_ResourceSystem );


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CResourceSystem::CResourceSystem() :
	m_TypeHandlers( DefLessFunc( ResourceType_t ) ),
	m_Resources( DefLessFunc( CUtlSymbol ) ),
	m_bInitialized( false )
{
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CResourceSystem::~CResourceSystem()
{
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : factory - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CResourceSystem::Connect( CreateInterfaceFn factory )
{
	// Don't connect twice
	static bool bConnected = false;
	if ( bConnected )
	{
		return true;
	}
	bConnected = true;

	if ( !BaseClass::Connect( factory ) )
		return false;

	return true;
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CResourceSystem::Disconnect()
{
	BaseClass::Disconnect();
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : pInterfaceName - 
// Output : void*
//-----------------------------------------------------------------------------
void *CResourceSystem::QueryInterface( const char *pInterfaceName )
{
	return Sys_GetFactoryThis()( pInterfaceName, NULL );
}


//-----------------------------------------------------------------------------
// Purpose: 
// Output : InitReturnVal_t
//-----------------------------------------------------------------------------
InitReturnVal_t CResourceSystem::Init()
{
	// Only init once
	if ( m_bInitialized )
		return INIT_OK;
	m_bInitialized = true;

	RegisterModelHandlers();

	HModel hModel = g_pResourceSystem->FindOrCreateResource< CModel >( "models/creatures/headcrab_classic/headcrab_classic.vmdl" );

	return BaseClass::Init();
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CResourceSystem::Shutdown()
{
	// Only shutdown once
	if ( !m_bInitialized )
		return;
	m_bInitialized = false;

	UnregisterModelHandlers();

	// Remove all the type handlers
	m_TypeHandlersLock.LockForWrite();
	m_TypeHandlers.RemoveAll();
	m_TypeHandlersLock.UnlockWrite();

	BaseClass::Shutdown();
}


//-----------------------------------------------------------------------------
// Purpose: Gets a resource by name.
// Input  : pszName - 
//			nType - 
//-----------------------------------------------------------------------------
WeakResourceHandle_t CResourceSystem::FindOrCreateResource( const char* pszName, 
	ResourceType_t nType )
{
	m_mutex.Lock();

	// TODO: Normalize name

	// Try to find it in the map
	unsigned short iResource = m_Resources.Find( pszName );
	if ( iResource != m_Resources.InvalidIndex() )
	{
		return m_Resources[iResource];
	}

	// Create initial empty version (prevents infinite recursion)
	m_Resources.Insert( pszName, NULL );

	m_mutex.Unlock();	// Unlocking to prevent a deadlock

	// Otherwise try to create it.
	m_TypeHandlersLock.LockForRead();
	unsigned short i = m_TypeHandlers.Find( nType );
	IResourceTypeHandler *pHandler = i != m_TypeHandlers.InvalidIndex() ? m_TypeHandlers[i] : NULL;
	m_TypeHandlersLock.UnlockRead();

	if ( !pHandler )
	{
		Warning( "Attempted to create resource %s with unknown type %d\n", pszName, nType );
		return NULL;
	}

	// TODO: Some kind of async system?

	CUtlBuffer buf;
	if ( !g_pFullFileSystem->ReadFile( CFmtStrN<MAX_PATH>( "%s_c", pszName ), NULL, buf ) )
	{
		Warning( "Could not open resource file %s\n", pszName );
		return NULL;
	}

	ResourceFileHeader_t *pHeader = (ResourceFileHeader_t*)buf.Base();
	if ( pHeader->m_nHeaderVersion != RESOURCEFILE_HEADER_VERSION )
	{
		Warning( "Resource file %s has wrong header version %d\n", pszName, pHeader->m_nHeaderVersion );
		return NULL;
	}

	void *pData = pHandler->CreateResource( pHeader );
	if ( !pData )
	{
		Warning( "Failed to create resource %s\n", pszName );
		return NULL;
	}

	ResourceInstance *pInstance = new ResourceInstance( nType, pData );

	m_mutex.Lock();
	m_Resources.InsertOrReplace( pszName, pInstance );
	m_mutex.Unlock();

	return pInstance;
}


//-----------------------------------------------------------------------------
// Purpose: Destroys a resource.
// Input  : hResource 
//-----------------------------------------------------------------------------
void CResourceSystem::DestroyResource( WeakResourceHandle_t hResource )
{
	// This shouldn't be needed
	//AUTO_LOCK( m_mutex );

	if ( hResource == NULL )
		return;

	m_ResourcesToDelete.PushItem( hResource );
}


//-----------------------------------------------------------------------------
// Purpose: Registers the handler for a resource type.
// Input  : resourceType - 
//			pHandler - 
//-----------------------------------------------------------------------------
void CResourceSystem::RegisterTypeHandler( ResourceType_t resourceType, 
	IResourceTypeHandler *pHandler )
{
	m_TypeHandlersLock.LockForWrite();
	if ( m_TypeHandlers.Find( resourceType ) == m_TypeHandlers.InvalidIndex() )
	{
		DevMsg( "Registering handler for resource type %lld\n", resourceType );
		m_TypeHandlers.Insert( resourceType, pHandler );
	}
	else
	{
		Warning( "Handler for resource type %lld already registered\n", resourceType );
	}
	m_TypeHandlersLock.UnlockWrite();
}


//-----------------------------------------------------------------------------
// Purpose: Unregisters the handler for a resource type.
// Input  : resourceType - 
//			pHandler - 
//-----------------------------------------------------------------------------
void CResourceSystem::UnregisterTypeHandler( ResourceType_t resourceType, 
	IResourceTypeHandler *pHandler )
{
	m_TypeHandlersLock.LockForWrite();

	unsigned short i = m_TypeHandlers.Find( resourceType );
	if ( i == m_TypeHandlers.InvalidIndex() )
	{
		Warning( "Attempting to unregister handler for resource type %lld, which was never registered\n", resourceType );
	}
	else if ( m_TypeHandlers[i] != pHandler )
	{
		Warning( "Attempting to unregister mismatched handler for resource type %lld\n", resourceType );
	}
	else
	{
		DevMsg( "Unregistering handler for resource type %lld\n", resourceType );
		m_TypeHandlers.Remove( resourceType );
	}
	m_TypeHandlersLock.UnlockWrite();
}


//-----------------------------------------------------------------------------
// Purpose: Gets the handler for a resource type.
// Input  : resourceType - 
// Output : IResourceTypeHandler*
//-----------------------------------------------------------------------------
IResourceTypeHandler *CResourceSystem::GetTypeHandler( ResourceType_t resourceType )
{
	IResourceTypeHandler *ret = NULL;

	m_TypeHandlersLock.LockForRead();
	unsigned short i = m_TypeHandlers.Find( resourceType );
	if ( i != m_TypeHandlers.InvalidIndex() )
	{
		ret = m_TypeHandlers[i];
	}
	m_TypeHandlersLock.UnlockRead();

	return ret;
}
