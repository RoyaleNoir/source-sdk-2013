#ifndef IRESOURCESYSTEM_H
#define IRESOURCESYSTEM_H

#include "appframework/IAppSystem.h"

#include "resourcefile.h"


//-----------------------------------------------------------------------------
// Handle typedefs.
//-----------------------------------------------------------------------------
typedef uint64 ResourceType_t;

// TODO: Use compile-time hashes instead?

#define RESOURCE_TYPE4( a, b, c, d ) \
	( ( (uint64)a << 24 ) | ( (uint64)b << 16 ) \
	| ( (uint64)c << 8  ) | ( (uint64)d << 0  ) )

#define RESOURCE_TYPE8( a, b, c, d, e, f, g, h ) \
	( ( (uint64)a << 56 ) | ( (uint64)b << 48 ) \
	| ( (uint64)c << 40 ) | ( (uint64)d << 32 ) \
	| ( (uint64)e << 24 ) | ( (uint64)f << 16 ) \
	| ( (uint64)g << 8  ) | ( (uint64)h << 0  ) )


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
	// Registers the handler for a resource type.
	virtual void RegisterTypeHandler( ResourceType_t resourceType, 
		IResourceTypeHandler *pHandler ) = 0;

	// Unregisters the handler for a resource type.
	virtual void UnregisterTypeHandler( ResourceType_t resourceType, 
		IResourceTypeHandler *pHandler ) = 0;
};

extern IResourceSystem *g_pResourceSystem;

#define RESOURCESYSTEM_INTERFACE_VERSION	"VResourceSystem001"


#endif // IRESOURCESYSTEM_H