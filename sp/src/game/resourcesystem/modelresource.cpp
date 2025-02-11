#include "resourcesystem/modelresource.h"

#include "resourcesystem/iresourcesystem.h"

#include "keyvalues3/keyvalues3.h"
#include "keyvalues3/keyvalues3_deserialize.h"
#include "utlbuffer.h"
#include "utlstring.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


BEGIN_KV3_TABLE( PermModelInfo_t )
	KV3_TABLE_ELEM( m_nFlags )
	KV3_TABLE_ELEM( m_vHullMin )
	KV3_TABLE_ELEM( m_vHullMax )
	KV3_TABLE_ELEM( m_vViewMin )
	KV3_TABLE_ELEM( m_vViewMax )
	KV3_TABLE_ELEM( m_flMass )
	KV3_TABLE_ELEM( m_vEyePosition )
	KV3_TABLE_ELEM( m_flMaxEyeDeflection )
	KV3_TABLE_ELEM( m_sSurfaceProperty )
	KV3_TABLE_ELEM( m_keyValueText )
END_KV3_TABLE()

BEGIN_KV3_TABLE( MaterialGroup_t )
	KV3_TABLE_ELEM( m_name )
	//KV3_TABLE_ARRY( m_materials )
END_KV3_TABLE()

BEGIN_KV3_TABLE( ModelSkeletonData_t )
	KV3_TABLE_ARRY( m_boneName )
	KV3_TABLE_ARRY( m_nParent )
	KV3_TABLE_ARRY( m_boneSphere )
	KV3_TABLE_ARRY( m_nFlag )
	KV3_TABLE_ARRY( m_bonePosParent )
	KV3_TABLE_ARRY( m_boneRotParent )
END_KV3_TABLE()

BEGIN_KV3_ENUM( ModelBoneFlexComponent_t )
	KV3_ENUM_VALUE( MODEL_BONE_FLEX_INVALID )
	KV3_ENUM_VALUE( MODEL_BONE_FLEX_TX )
	KV3_ENUM_VALUE( MODEL_BONE_FLEX_TY )
	KV3_ENUM_VALUE( MODEL_BONE_FLEX_TZ )
END_KV3_ENUM()

BEGIN_KV3_TABLE( ModelBoneFlexDriverControl_t )
	KV3_TABLE_ELEM( m_nBoneComponent )
	KV3_TABLE_ELEM( m_flexController )
	KV3_TABLE_ELEM( m_flexControllerToken )
	KV3_TABLE_ELEM( m_flMin )
	KV3_TABLE_ELEM( m_flMax )
END_KV3_TABLE()

BEGIN_KV3_TABLE( ModelBoneFlexDriver_t )
	KV3_TABLE_ELEM( m_boneName )
	KV3_TABLE_ELEM( m_boneNameToken )
	KV3_TABLE_ARRY( m_controls )
END_KV3_TABLE()

BEGIN_KV3_TABLE( PermModelData_t )
	KV3_TABLE_ELEM( m_name )
	KV3_TABLE_ELEM( m_modelInfo )
	//KV3_TABLE_ELEM( m_ExtParts )
	//KV3_TABLE_ELEM( m_refMeshes )
	KV3_TABLE_ARRY( m_refMeshGroupMasks )
	KV3_TABLE_ARRY( m_refPhysGroupMasks )
	KV3_TABLE_ARRY( m_refLODGroupMasks )
	KV3_TABLE_ARRY( m_lodGroupSwitchDistances )
	//KV3_TABLE_ELEM( m_refPhysicsData )
	//KV3_TABLE_ELEM( m_refPhysicsHitboxData )
	//KV3_TABLE_ELEM( m_refAnimGroups )
	//KV3_TABLE_ELEM( m_refSequenceGroups )
	KV3_TABLE_ARRY( m_meshGroups )
	KV3_TABLE_ARRY( m_materialGroups )
	KV3_TABLE_ELEM( m_nDefaultMeshGroupMask )
	KV3_TABLE_ELEM( m_modelSkeleton )
	KV3_TABLE_ARRY( m_remappingTable )
	KV3_TABLE_ARRY( m_remappingTableStarts )
	KV3_TABLE_ARRY( m_boneFlexDrivers )
END_KV3_TABLE()


//-----------------------------------------------------------------------------
// Resource type handler.
//-----------------------------------------------------------------------------
class CModelResourceTypeHandler : public IResourceTypeHandler
{
public:
	virtual void *CreateResource( const ResourceFileHeader_t *pHeader )
	{
		const ResourceOffsetArray<void> *pDataBlock = pHeader->GetDataBlock();
		if ( !pDataBlock )
		{
			Warning( "CreateResource: Failed to load KV3!\n" );
			return NULL;
		}

		// Decode data block
		CUtlBuffer buf( pDataBlock->Base(), pDataBlock->Count(), CUtlBuffer::READ_ONLY );
		KeyValues3 kv;
		if ( !kv.LoadFromBuffer( buf ) )
		{
			Warning( "CreateResource: Failed to load KV3!\n" );
			return NULL;
		}

		CModel *pModel = new CModel;

		if ( !DeserializeKV3( &kv, pModel->GetData() ) )
			Warning( "CreateResource: KV3 deserialization failed\n" );

		return pModel;
	}

	virtual void DestroyResource( void *pResource ) override
	{
		CModel *pMesh = (CModel*)pResource;
		delete pMesh;
	}
};

CModelResourceTypeHandler g_ModelHandler;


void RegisterModelHandlers()
{
	g_pResourceSystem->RegisterTypeHandler( RESOURCE_TYPE4( 'v', 'm', 'd', 'l'), &g_ModelHandler );
}


void UnregisterModelHandlers()
{
	g_pResourceSystem->UnregisterTypeHandler( RESOURCE_TYPE4( 'v', 'm', 'd', 'l'), &g_ModelHandler );
}