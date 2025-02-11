#ifndef RESOURCEFILE_H
#define RESOURCEFILE_H

#include "tier0/basetypes.h"


#define RESOURCEFILE_HEADER_VERSION 12


typedef uint32 ResourceBlockId_t;


//-----------------------------------------------------------------------------
// An array defined by an offset into the file and count.
//-----------------------------------------------------------------------------
template < typename T >
class ResourceOffsetArray
{
public:
	const T *Base() const
	{
		return (T*)( (uint8*)( this ) + m_nOffset );
	}

	const size_t Count() const
	{
		return m_nCount;
	}

	const T *Get( size_t i ) const
	{
		if ( i >= m_nCount )
			return nullptr;

		return Base() + i;
	}

	const T *operator[]( size_t i ) const
	{
		return Get( i );
	}

private:
	uint32	m_nOffset;
	uint32	m_nCount;
};


//-----------------------------------------------------------------------------
// Resource file data blocks.
//-----------------------------------------------------------------------------
struct ResourceFileBlock_t
{
	ResourceBlockId_t			m_nId;
	ResourceOffsetArray<void>	m_Data;
};


//-----------------------------------------------------------------------------
// Resource file header data.
//-----------------------------------------------------------------------------
struct ResourceFileHeader_t
{
	template <typename T> const T *GetBlock( uint32 nId ) const;
	const ResourceOffsetArray<void> *GetDataBlock() const;

	uint32	m_nFileSize;
	uint16	m_nHeaderVersion;
	uint16	m_nFormatVersion;
	ResourceOffsetArray<ResourceFileBlock_t> m_Blocks;
};


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : nId - 
// Output : const T*
//-----------------------------------------------------------------------------
template<typename T>
inline const T *ResourceFileHeader_t::GetBlock( uint32 nId ) const
{
	for ( uint32 i = 0; i < m_Blocks.Count(); i++)
	{
		if ( m_Blocks[i]->m_nId == nId )
		{
			return (const T*)m_Blocks[i]->m_Data.Base();
		}
	}

	return NULL;
}


//-----------------------------------------------------------------------------
// Purpose: 
// Output : const ResourceFileOffsetArray_t<void>
//-----------------------------------------------------------------------------
inline const ResourceOffsetArray<void> *ResourceFileHeader_t::GetDataBlock() const
{
	for ( uint32 i = 0; i < m_Blocks.Count(); i++)
	{
		if ( m_Blocks[i]->m_nId == MAKEID( 'D', 'A', 'T', 'A' ) )
		{
			return &m_Blocks[i]->m_Data;
		}
	}

	return NULL;
}


#endif // RESOURCEFILE_H