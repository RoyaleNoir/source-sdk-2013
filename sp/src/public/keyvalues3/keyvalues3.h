#ifndef KEYVALUES3_H
#define KEYVALUES3_H
#ifdef _WIN32
#pragma once
#endif

#include "utlbuffer.h"
#include "utlstring.h"
#include "utlvector.h"
#include "mathlib/vector.h"
#include "mathlib/vector4d.h"

enum KV3Type_t : uint8
{
	KV3_TYPE_INVALID = 0,
	
	KV3_TYPE_NULL,
	KV3_TYPE_BOOL,
	KV3_TYPE_INT64,
	KV3_TYPE_UINT64,
	KV3_TYPE_DOUBLE,
	KV3_TYPE_STRING,
	KV3_TYPE_BINARY_BLOB,
	KV3_TYPE_ARRAY,
	KV3_TYPE_TABLE,
	KV3_TYPE_ARRAY_TYPED,
	KV3_TYPE_INT32,
	KV3_TYPE_UINT32,

	KV3_TYPE_BOOL_TRUE,
	KV3_TYPE_BOOL_FALSE,
	KV3_TYPE_INT64_ZERO,
	KV3_TYPE_INT64_ONE,
	KV3_TYPE_DOUBLE_ZERO,
	KV3_TYPE_DOUBLE_ONE,

	KV3_TYPE_FLOAT,

	KV3_TYPE_UNK_20,
	KV3_TYPE_UNK_21,
	KV3_TYPE_UNK_22,

	KV3_TYPE_INT32_AS_BYTE,
	KV3_TYPE_ARRAY_TYPED_BYTE_LENGTH,
};

enum KV3Flags_t : uint8
{
	KV3_FLAG_RESOURCE			= ( 1 << 0 ),
	KV3_FLAG_RESOURCE_NAME		= ( 1 << 1 ),
	KV3_FLAG_MULTILINE_STRING	= ( 1 << 2 ),
	KV3_FLAG_PANORAMA			= ( 1 << 3 ),
	KV3_FLAG_SOUND_EVENT		= ( 1 << 4 ),
	KV3_FLAG_SUBCLASS			= ( 1 << 5 ),
};

// NOTE: ONLY BINARY SUPPORT FOR NOW

class KeyValues3
{
public:
	KeyValues3();
	~KeyValues3();

	KV3Type_t GetType() const;
	uint8 GetFlags() const;

	// Generic accessor for numeric values
	template<typename T> T GetNumber( T defaultValue = 0 ) const
	{
		switch ( m_nType )
		{
		case KV3_TYPE_BOOL:
			return static_cast<T>( m_bValue ? 1 : 0 );
		case KV3_TYPE_INT64:
			return static_cast<T>( m_iValue64 );
		case KV3_TYPE_UINT64:
			return static_cast<T>( m_uValue64 );
		case KV3_TYPE_DOUBLE:
			return static_cast<T>( m_flValue64 );
		case KV3_TYPE_INT32:
			return static_cast<T>( m_iValue32 );
		case KV3_TYPE_UINT32:
			return static_cast<T>( m_uValue32 );
		case KV3_TYPE_FLOAT:
			return static_cast<T>( m_flValue32 );
		default:
			Assert( 0 );
			return defaultValue;
		}
	}

	bool	GetBool( bool bDefaultValue = false ) const;
	int64	GetInt64( int64 iDefaultValue = 0 ) const;
	uint64	GetUnt64( uint64 uDefaultValue = 0 ) const;
	double	GetDouble( double flDefaultValue = 0.0 ) const;
	const char *GetString( const char *pszDefaultValue = "" ) const;
	int32	GetInt32( int32 iDefaultValue = 0 ) const;
	uint32	GetUnt32( uint32 uDefaultValue = 0 ) const;
	float	GetFloat( float flDefaultValue = 0.0f ) const;

	const byte *GetBlobBase() const;
	size_t GetBlobSize() const;
	
	// GetArray
	const KeyValues3 *GetArrayElement( size_t i ) const;
	size_t GetArrayCount() const;

	// GetTable
	const KeyValues3 *GetTableElement( const char *pszName ) const;

	void SetNull();
	void SetBool( bool bValue );
	void SetInt64( int64 iValue );
	void SetUnt64( uint64 uValue );
	void SetDouble( double flValue );
	void SetString( const char *pszValue );
	void SetInt32( int32 iValue );
	void SetUnt32( uint32 uValue );
	void SetFloat( float flValue );

	void SetBlob( const byte *pData, size_t nSize );

	void SetEmptyArray( size_t nInitialCapacity = 0 );
	bool AddArrayElement( KeyValues3 *pElement );

	void SetEmptyTable( size_t nInitialCapacity = 0 );
	bool AddTableElement( const char *pszName, KeyValues3 *pElement );

	void SetVector( float x, float y, float z );
	void SetQAngle( float x, float y, float z );

	void SetColor( byte r, byte g, byte b, byte a );

	bool LoadFromBuffer( CUtlBuffer &buf );

private:
	void Free();
	void SetTypeAndFree( KV3Type_t nNewType );

	struct ReadContext
	{
		const CUtlVector<uint32> *dwordTable;
		const CUtlVector<uint64> *qwordTable;
		const CUtlVector<const char*> *stringTable;
		const CUtlVector<uint8> *typeTable;
		int nCurrentDWord;
		int nCurrentQWord;
		int nCurrentType;
	};

	bool LoadBinary_R( CUtlBuffer &buf, ReadContext &context, 
		KV3Type_t nType = KV3_TYPE_INVALID, uint8 nTypeFlags = 0 );

private:
	KV3Type_t	m_nType;
	uint8		m_nFlags;

	struct Blob_t
	{
		Blob_t( const byte *pBlob, size_t nSize);
		~Blob_t();

		const byte *Base() const
		{
			return m_pData;
		}

		const size_t Size() const
		{
			return m_nSize;
		}

	private:
		byte	*m_pData;
		size_t	m_nSize;
	};

	struct Array_t
	{
		Array_t( int nInitialCapacity = 0 );
		~Array_t();

		size_t Count() const;

		// NOTE: Takes ownership of pElement
		void AddElement( KeyValues3 *pElement );

		const KeyValues3 *GetElement( size_t i ) const;

	private:
		CUtlVector<KeyValues3*> m_Elements;
	};

	struct Table_t
	{
		Table_t( int nInitialCapacity = 0 );
		~Table_t();

		// NOTE: Takes ownership of pElement
		void AddElement( const char *pszName, KeyValues3 *pElement );

		const KeyValues3 *GetElement( const char *pszName ) const;

	private:
		CUtlVector<CUtlString> m_Names;
		CUtlVector<KeyValues3*> m_Elements;
	};

	union
	{
		bool	m_bValue;
		int64	m_iValue64;
		uint64	m_uValue64;
		double	m_flValue64;
		char	*m_pszValue;
		Blob_t	*m_pBlob;
		Array_t	*m_pArray;
		Table_t	*m_pTable;
		int32	m_iValue32;
		uint32	m_uValue32;
		float	m_flValue32;
	};
};


inline KV3Type_t KeyValues3::GetType() const
{
	return m_nType;
}


inline uint8 KeyValues3::GetFlags() const
{
	return m_nFlags;
}

#endif // KEYVALUES3_H