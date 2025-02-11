#ifndef MODELRESOURCE_H
#define MODELRESOURCE_H

#include "resourcesystem/iresourcesystem.h"

#include "utlstring.h"
#include "utlvector.h"


struct PermModelInfo_t
{
	uint32		m_nFlags;
	Vector		m_vHullMin;
	Vector		m_vHullMax;
	Vector		m_vViewMin;
	Vector		m_vViewMax;
	float		m_flMass;
	Vector		m_vEyePosition;
	float		m_flMaxEyeDeflection;
	CUtlString	m_sSurfaceProperty;
	CUtlString	m_keyValueText;
};


struct MaterialGroup_t
{
	CUtlString	m_name;
	//CUtlVector< CStrongHandle< IMaterial2 > > m_materials;
};

struct ModelSkeletonData_t
{
	enum BoneFlags_t
	{
		FLAG_NO_BONE_FLAGS = 0,
		FLAG_BONEFLEXDRIVER = 4,
		FLAG_CLOTH = 8,
		FLAG_PHYSICS = 16,
		FLAG_ATTACHMENT = 32,
		FLAG_ANIMATION = 64,
		FLAG_MESH = 128,
		FLAG_HITBOX = 256,
		FLAG_RETARGET_SRC = 512,
		FLAG_BONE_USED_BY_VERTEX_LOD0 = 1024,
		FLAG_BONE_USED_BY_VERTEX_LOD1 = 2048,
		FLAG_BONE_USED_BY_VERTEX_LOD2 = 4096,
		FLAG_BONE_USED_BY_VERTEX_LOD3 = 8192,
		FLAG_BONE_USED_BY_VERTEX_LOD4 = 16384,
		FLAG_BONE_USED_BY_VERTEX_LOD5 = 32768,
		FLAG_BONE_USED_BY_VERTEX_LOD6 = 65536,
		FLAG_BONE_USED_BY_VERTEX_LOD7 = 131072,
		FLAG_BONE_MERGE_READ = 262144,
		FLAG_BONE_MERGE_WRITE = 524288,
		FLAG_ALL_BONE_FLAGS = 1048575,
		BLEND_PREALIGNED = 1048576,
		FLAG_RIGIDLENGTH = 2097152,
		FLAG_PROCEDURAL = 4194304,
	};

	CUtlVector< CUtlString >	m_boneName;
	CUtlVector< int16 >			m_nParent;
	CUtlVector< float32 >		m_boneSphere;
	CUtlVector< uint32 >		m_nFlag;
	CUtlVector< Vector >		m_bonePosParent;
	CUtlVector< Quaternion >	m_boneRotParent;
};

enum ModelBoneFlexComponent_t
{
	MODEL_BONE_FLEX_INVALID = -1,
	MODEL_BONE_FLEX_TX = 0,
	MODEL_BONE_FLEX_TY,
	MODEL_BONE_FLEX_TZ,
};

struct ModelBoneFlexDriverControl_t
{
	ModelBoneFlexComponent_t m_nBoneComponent;
	CUtlString	m_flexController;
	uint32		m_flexControllerToken;
	float		m_flMin;
	float		m_flMax;
};

struct ModelBoneFlexDriver_t
{
	CUtlString	m_boneName;
	uint32		m_boneNameToken;
	CUtlVector< ModelBoneFlexDriverControl_t > m_controls;
};

struct PermModelData_t
{
	CUtlString						m_name;
	PermModelInfo_t					m_modelInfo;
	//CUtlVector< PermModelExtPart_t > m_ExtParts;
	//m_refMeshes;
	CUtlVector< uint64 >			m_refMeshGroupMasks;
	CUtlVector< uint64 >			m_refPhysGroupMasks;
	CUtlVector< uint8 >				m_refLODGroupMasks;
	CUtlVector< float >				m_lodGroupSwitchDistances;
	//m_refPhysicsData;
	//m_refPhysicsHitboxData;
	//m_refAnimGroups;
	//m_refSequenceGroups;
	CUtlVector< CUtlString >		m_meshGroups;
	CUtlVector< MaterialGroup_t >	m_materialGroups;
	uint64							m_nDefaultMeshGroupMask;
	ModelSkeletonData_t				m_modelSkeleton;
	CUtlVector< uint16 >			m_remappingTable;
	CUtlVector< uint16 >			m_remappingTableStarts;
	CUtlVector< ModelBoneFlexDriver_t> m_boneFlexDrivers;
};

//-----------------------------------------------------------------------------
// Model resource.
//-----------------------------------------------------------------------------
class CModel
{
public:
	PermModelData_t &GetData()
	{
		return m_Data;
	}

	const PermModelData_t &GetData() const
	{
		return m_Data;
	}

private:
	PermModelData_t	m_Data;
};

template < >
struct ResourceTypeInfo< CModel >
{
	const static ResourceType_t RESOURCE_TYPE = RESOURCE_TYPE4( 'v', 'm', 'd', 'l' );
};
typedef CStrongHandle< CModel > HModel;


#endif // MODELRESOURCE_H