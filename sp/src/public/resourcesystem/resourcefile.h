#ifndef RESOURCEFILE_H
#define RESOURCEFILE_H

#include "tier0/basetypes.h"


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
	ResourceBlockId_t			m_nBlockId;
	ResourceOffsetArray<void>	m_Data;
};


//-----------------------------------------------------------------------------
// Resource file header data.
//-----------------------------------------------------------------------------
struct ResourceFileHeader_t
{
	uint32	m_nFileSize;
	uint16	m_nHeaderVersion;
	uint16	m_nFormatVersion;
	ResourceOffsetArray<ResourceFileBlock_t> m_Blocks;
};

#endif // RESOURCEFILE_H