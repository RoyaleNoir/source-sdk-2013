#include "resourcesystem/iresourcesystem.h"

#include "tier0/threadtools.h"
#include "tier1/utlmap.h"
#include "tier2/tier2.h"


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
	virtual void RegisterTypeHandler( ResourceType_t resourceType, 
		IResourceTypeHandler *pHandler );

	virtual void UnregisterTypeHandler( ResourceType_t resourceType, 
		IResourceTypeHandler *pHandler );

private:
	IResourceTypeHandler *GetTypeHandler( ResourceType_t resourceType );

	mutable CThreadSpinRWLock m_TypeHandlersLock;
	CUtlMap< ResourceType_t, IResourceTypeHandler* > m_TypeHandlers;

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

	// Remove all the type handlers
	m_TypeHandlersLock.LockForWrite();
	m_TypeHandlers.RemoveAll();
	m_TypeHandlersLock.UnlockWrite();

	BaseClass::Shutdown();
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
		DevMsg( "Registering handler for resource type %d\n", resourceType );
		m_TypeHandlers.Insert( resourceType, pHandler );
	}
	else
	{
		Warning( "Handler for resource type %d already registered\n", resourceType );
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
		Warning( "Attempting to unregister handler for resource type %d, which was never registered\n", resourceType );
	}
	else if ( m_TypeHandlers[i] != pHandler )
	{
		Warning( "Attempting to unregister mismatched handler for resource type %d\n", resourceType );
	}
	else
	{
		DevMsg( "Unregistering handler for resource type %d\n", resourceType );
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
