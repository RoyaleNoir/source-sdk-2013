#ifndef IRESOURCESYSTEM_H
#define IRESOURCESYSTEM_H

#include "appframework/IAppSystem.h"
#include "tier0/threadtools.h"

#include "resourcefile.h"


//-----------------------------------------------------------------------------
// Handle typedefs.
//-----------------------------------------------------------------------------
typedef uint64 ResourceType_t;

// TODO: Use compile-time hashes instead?

#define RESOURCE_TYPE4( a, b, c, d ) \
	( ( (uint64)a << 56 ) | ( (uint64)b << 48 ) \
	| ( (uint64)c << 40 ) | ( (uint64)d << 32 ) )

#define RESOURCE_TYPE8( a, b, c, d, e, f, g, h ) \
	( ( (uint64)a << 56 ) | ( (uint64)b << 48 ) \
	| ( (uint64)c << 40 ) | ( (uint64)d << 32 ) \
	| ( (uint64)e << 24 ) | ( (uint64)f << 16 ) \
	| ( (uint64)g << 8  ) | ( (uint64)h << 0  ) )


template < typename T >
struct ResourceTypeInfo;


class ResourceInstance;
typedef ResourceInstance *WeakResourceHandle_t;


//-----------------------------------------------------------------------------
// Interface to create/destroy a given resource type.
//-----------------------------------------------------------------------------
abstract_class IResourceTypeHandler
{
public:
	// Creates a resource instance from a file.
	virtual void *CreateResource( const ResourceFileHeader_t *pHeader ) = 0;

	// Destroys a resource instance.
	virtual void DestroyResource( void *pResource ) = 0;
};


//-----------------------------------------------------------------------------
// ResourceSystem interface.
//-----------------------------------------------------------------------------
abstract_class IResourceSystem : public IAppSystem
{
public:
	virtual WeakResourceHandle_t FindOrCreateResource( const char *pszName, ResourceType_t nType ) = 0;
	virtual void DestroyResource( WeakResourceHandle_t hResource ) = 0;

	// Registers the handler for a resource type.
	virtual void RegisterTypeHandler( ResourceType_t resourceType, 
		IResourceTypeHandler *pHandler ) = 0;

	// Unregisters the handler for a resource type.
	virtual void UnregisterTypeHandler( ResourceType_t resourceType, 
		IResourceTypeHandler *pHandler ) = 0;

	template < typename T >
	WeakResourceHandle_t FindOrCreateResource( const char *pszName )
	{
		return FindOrCreateResource( pszName, ResourceTypeInfo< T >::RESOURCE_TYPE );
	}
};

extern IResourceSystem *g_pResourceSystem;

#define RESOURCESYSTEM_INTERFACE_VERSION	"VResourceSystem001"


//-----------------------------------------------------------------------------
// Loaded Resource
//-----------------------------------------------------------------------------
class ResourceInstance
{
public:
	ResourceInstance( ResourceType_t nType, void *pData ) :
		m_nType( nType ),
		m_pData( pData ),
		m_nRefCount( 0 )
	{
	}

	// TODO: Figure out a way to make this actually safe!!
	// For now making it so that 99% of the time users only have const references, apart from materialsystem reloads.
	template < typename T >
	const T *Get() const
	{
		// Make sure it's the correct type
		if ( m_nType != ResourceTypeInfo< T >::RESOURCE_TYPE )
			return NULL;

		return (T*)m_pData;
	}

	int AddRef()
	{
		return ++m_nRefCount;
	}

	int Release()
	{
		m_nRefCount--;

		if ( m_nRefCount <= 0 )
		{
			g_pResourceSystem->DestroyResource( this );
		}

		return m_nRefCount;
	}

	int GetRefCount()
	{
		return m_nRefCount;
	}

private:
	// Metadata
	ResourceType_t	m_nType;
	
	// The actual data
	void			*m_pData;

	// Reference counting
	CInterlockedInt	m_nRefCount;
};


//-----------------------------------------------------------------------------
// Strong resource handle.
//-----------------------------------------------------------------------------
template < typename T >
class CStrongHandle
{
public:
	CStrongHandle()
	{
		m_hResource = NULL;
	}

	CStrongHandle( WeakResourceHandle_t hResource )
	{
		m_hResource = hResource;
		if ( m_hResource )
		{
			m_hResource->AddRef();
		}
	}

	~CStrongHandle()
	{
		if ( m_hResource )
		{
			m_hResource->Release();
		}
	}

	const T *Get() const
	{
		if ( !m_hResource )
			return NULL;

		return m_hResource->Get<T>();
	}

	CStrongHandle &operator=( WeakResourceHandle_t &rhs )
	{
		// Release existing handle
		if ( m_hResource )
		{
			m_hResource->Release();
		}

		m_hResource = rhs;
		if ( m_hResource )
		{
			m_hResource->AddRef();
		}

		return *this;
	}

	/*CStrongHandle& operator=(const CStrongHandle& rhs)
	{
		// Release existing handle
		if ( m_hResource )
		{
			m_hResource->Release();
		}

		m_hResource = rhs.m_pResource;
		if ( m_hResource )
		{
			m_hResource->AddRef();
		}

		return *this;
	}*/

private:
	WeakResourceHandle_t	m_hResource;
};


#endif // IRESOURCESYSTEM_H
