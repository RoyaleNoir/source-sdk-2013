#include "keyvalues3/keyvalues3.h"
#include "tier0/dbg.h"
#include "utlbuffer.h"
#include "utlstring.h"
#include <Guiddef.h>
#include <lz4.h>

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>


#define KV3_MAGIC 0x4B563301


KeyValues3::Table_t::Table_t( int nInitialCapacity )
{
	m_Names.EnsureCapacity( nInitialCapacity );
	m_Elements.EnsureCapacity( nInitialCapacity );
}

KeyValues3::Table_t::~Table_t()
{
	m_Names.Purge();
	m_Elements.PurgeAndDeleteElements();
}

void KeyValues3::Table_t::AddElement( const char *pszName, KeyValues3 *pElement )
{
	Assert( m_Names.Count() == m_Elements.Count() );

	m_Names.AddToTail( pszName );
	m_Elements.AddToTail( pElement );
}


const KeyValues3 *KeyValues3::Table_t::GetElement( const char *pszName ) const
{
	Assert( m_Names.Count() == m_Elements.Count() );

	CUtlString tmp( pszName );

	// TODO: Make this faster
	for ( int i = 0; i < m_Elements.Count(); i++ )
	{
		if ( CUtlString::SortCaseInsensitive( &m_Names[i], &tmp ) == 0 )
			return m_Elements[i];
	}

	return NULL;
}


KeyValues3::Array_t::Array_t( int nInitialCapacity )
{
	m_Elements.EnsureCapacity( nInitialCapacity );
}

KeyValues3::Array_t::~Array_t()
{
	m_Elements.PurgeAndDeleteElements();
}


size_t KeyValues3::Array_t::Count() const
{
	return m_Elements.Count();
}


void KeyValues3::Array_t::AddElement( KeyValues3 *pElement )
{
	m_Elements.AddToTail( pElement );
}


const KeyValues3 *KeyValues3::Array_t::GetElement( size_t i ) const
{
	Assert( i < (size_t)m_Elements.Count() );

	if ( i >= (size_t)m_Elements.Count() )
		return NULL;

	return m_Elements[i];
}


KeyValues3::KeyValues3() :
	m_nType( KV3_TYPE_INVALID )
{
	m_iValue64 = 0;	// Zeros out everything
}


KeyValues3::~KeyValues3()
{
	Free();
}


bool KeyValues3::GetBool( bool bDefaultValue ) const
{
	return GetNumber<int>( bDefaultValue ) != 0;
}


int64 KeyValues3::GetInt64( int64 iDefaultValue ) const
{
	return GetNumber( iDefaultValue );
}


uint64 KeyValues3::GetUnt64( uint64 uDefaultValue ) const
{
	return GetNumber( uDefaultValue );
}


double KeyValues3::GetDouble( double flDefaultValue ) const
{
	return GetNumber( flDefaultValue );
}


const char *KeyValues3::GetString( const char *pszDefaultValue ) const
{
	if ( m_nType == KV3_TYPE_STRING )
		return m_pszValue;

	return pszDefaultValue;
}


int32 KeyValues3::GetInt32( int32 iDefaultValue ) const
{
	return GetNumber( iDefaultValue );
}


uint32 KeyValues3::GetUnt32( uint32 uDefaultValue ) const
{
	return GetNumber( uDefaultValue );
}


float KeyValues3::GetFloat( float flDefaultValue ) const
{
	return GetNumber( flDefaultValue );
}


const byte *KeyValues3::GetBlobBase() const
{
	Assert( m_nType == KV3_TYPE_BINARY_BLOB );

	if ( m_nType != KV3_TYPE_BINARY_BLOB )
		return NULL;

	return m_pBlob->Base();
}


size_t KeyValues3::GetBlobSize() const
{
	Assert( m_nType == KV3_TYPE_BINARY_BLOB );

	if ( m_nType != KV3_TYPE_BINARY_BLOB )
		return 0;

	return m_pBlob->Size();
}


const KeyValues3 *KeyValues3::GetArrayElement( size_t i ) const
{
	Assert( m_nType == KV3_TYPE_ARRAY );

	if ( m_nType != KV3_TYPE_ARRAY )
		return NULL;

	return m_pArray->GetElement( i );
}


size_t KeyValues3::GetArrayCount() const
{
	Assert( m_nType == KV3_TYPE_ARRAY );

	if ( m_nType != KV3_TYPE_ARRAY )
		return 0;

	return m_pArray->Count();
}


const KeyValues3 *KeyValues3::GetTableElement( const char *pszName ) const
{
	Assert( m_nType == KV3_TYPE_TABLE );
	if ( m_nType != KV3_TYPE_TABLE )
		return NULL;

	return m_pTable->GetElement( pszName );
}


void KeyValues3::SetNull()
{
	SetTypeAndFree( KV3_TYPE_NULL );
}


void KeyValues3::SetBool( bool bValue )
{
	SetTypeAndFree( KV3_TYPE_BOOL );
	m_bValue = bValue;
}


void KeyValues3::SetInt64( int64 iValue )
{
	SetTypeAndFree( KV3_TYPE_INT64 );
	m_iValue64 = iValue;
}


void KeyValues3::SetUnt64( uint64 uValue )
{
	SetTypeAndFree( KV3_TYPE_UINT64 );
	m_uValue64 = uValue;
}


void KeyValues3::SetDouble( double flValue )
{
	SetTypeAndFree( KV3_TYPE_DOUBLE );
	m_flValue64 = flValue;
}


void KeyValues3::SetString( const char *pszValue )
{
	SetTypeAndFree( KV3_TYPE_STRING );

	int nLength = Q_strlen( pszValue ) + 1;
	m_pszValue = new char[nLength];

	// Copy over the value
	Q_strncpy( m_pszValue, pszValue, nLength );
	m_pszValue[nLength - 1] = '\0';
}


void KeyValues3::SetInt32( int32 iValue )
{
	SetTypeAndFree( KV3_TYPE_INT32 );
	m_iValue32 = iValue;
}


void KeyValues3::SetUnt32( uint32 uValue )
{
	SetTypeAndFree( KV3_TYPE_UINT32 );
	m_uValue32 = uValue;
}


void KeyValues3::SetFloat( float flValue )
{
	SetTypeAndFree( KV3_TYPE_FLOAT );
	m_flValue32 = flValue;
}


void KeyValues3::SetBlob( const byte *pData, size_t nSize )
{
	SetTypeAndFree( KV3_TYPE_BINARY_BLOB );
	m_pBlob = new Blob_t( pData, nSize );
}


void KeyValues3::SetEmptyArray( size_t nInitialCapacity )
{
	SetTypeAndFree( KV3_TYPE_ARRAY );
	m_pArray = new Array_t( nInitialCapacity );
}


bool KeyValues3::AddArrayElement( KeyValues3 *pElement )
{
	Assert( m_nType == KV3_TYPE_ARRAY );
	if ( m_nType != KV3_TYPE_ARRAY )
		return false;

	m_pArray->AddElement( pElement );
	return true;
}


void KeyValues3::SetEmptyTable( size_t nInitialCapacity )
{
	SetTypeAndFree( KV3_TYPE_TABLE );
	m_pTable = new Table_t( nInitialCapacity );
}


bool KeyValues3::AddTableElement( const char *pszName, KeyValues3 *pElement )
{
	Assert( m_nType == KV3_TYPE_TABLE );
	if ( m_nType != KV3_TYPE_TABLE )
		return false;

	m_pTable->AddElement( pszName, pElement );
	return true;
}


void KeyValues3::SetVector( float x, float y, float z )
{
	SetEmptyArray( 3 );

	KeyValues3 *pX = new KeyValues3;
	KeyValues3 *pY = new KeyValues3;
	KeyValues3 *pZ = new KeyValues3;

	pX->SetFloat( x );
	pX->SetFloat( y );
	pX->SetFloat( z );

	AddArrayElement( pX );
	AddArrayElement( pY );
	AddArrayElement( pZ );
}


void KeyValues3::SetQAngle( float x, float y, float z )
{
	SetVector( x, y, z );
}


void KeyValues3::SetColor( byte r, byte g, byte b, byte a )
{
	SetEmptyArray( 4 );

	KeyValues3 *pR = new KeyValues3;
	KeyValues3 *pG = new KeyValues3;
	KeyValues3 *pB = new KeyValues3;
	KeyValues3 *pA = new KeyValues3;

	pR->SetInt32( r );
	pG->SetInt32( g );
	pB->SetInt32( b );
	pA->SetInt32( a );

	AddArrayElement( pR );
	AddArrayElement( pG );
	AddArrayElement( pB );
	AddArrayElement( pA );
}


bool KeyValues3::LoadFromBuffer( CUtlBuffer &buf )
{
	int magic = buf.GetInt();
	if ( magic != KV3_MAGIC )
	{
		Warning( "[KeyValues3] LoadFromBuffer(): Invalid KV3 magic %x\n", magic );
		return false;
	}

	_GUID encodingGUID;
	buf.Get( &encodingGUID, sizeof( encodingGUID ) );

	int nCompressionType = buf.GetInt();
	int nNumBlobBytes = buf.GetInt();
	int nNumDWords = buf.GetInt();
	int nNumQWords = buf.GetInt();

	CUtlBuffer decBuf;

	if ( nCompressionType == 0 )
	{
		decBuf.EnsureCapacity( buf.GetInt() );
		decBuf.CopyBuffer( buf );
	}
	else if ( nCompressionType == 1 )
	{
		int nDecompressedSize = buf.GetInt();
		//decBuf.EnsureCapacity( nDecompressedSize );

		byte* decData = new byte[nDecompressedSize];

		if ( !LZ4_decompress_safe( (const char*)buf.PeekGet(), (char*)decData, buf.GetBytesRemaining(), nDecompressedSize))
		{
			Warning( "[KeyValues3] LoadFromBuffer(): LZ4 decompression failed\n", nCompressionType );
			delete[] decData;
			return false;
		}

		decBuf.CopyBuffer( decData, nDecompressedSize );
		delete[] decData;
	}
	else
	{
		Warning( "[KeyValues3] LoadFromBuffer(): Unknown compression type %d\n", nCompressionType );
		return false;
	}

	int binaryOffset = decBuf.TellGet();
	decBuf.SeekGet( CUtlBuffer::SEEK_HEAD, nNumBlobBytes );

	decBuf.SeekGet( CUtlBuffer::SEEK_CURRENT, ( 4 - ( decBuf.TellGet() % 4 ) ) % 4 );

	CUtlVector<uint32> dwordTable( 0, nNumDWords );
	for ( int i = 0; i < nNumDWords; i++ )
	{
		dwordTable.AddToTail( decBuf.GetInt() );
	}

	decBuf.SeekGet( CUtlBuffer::SEEK_CURRENT, ( 8 - ( decBuf.TellGet() % 8 ) ) % 8 );

	CUtlVector<uint64> qwordTable( 0, nNumQWords );
	for ( int i = 0; i < nNumQWords; i++ )
	{
		qwordTable.AddToTail( decBuf.GetInt64() );
	}

	int stringCount = dwordTable[0];
	CUtlVector<CUtlString> stringTable( 0, stringCount );
	for ( int i = 0; i < stringCount; i++ )
	{
		char s[512];
		decBuf.GetString( s );
		stringTable.AddToTail( s );
	}

	CUtlVector<uint8> typeTable( 0, stringCount );
	while ( decBuf.GetBytesRemaining() > 4 )
	{
		typeTable.AddToTail( decBuf.GetChar() );
	}

	ReadContext readContext =
	{
		&dwordTable,
		&qwordTable,
		&stringTable,
		&typeTable,
		1,	// Already read the number of strings
		0, 
		0
	};

	// Move the reader back to the data
	decBuf.SeekGet( CUtlBuffer::SEEK_HEAD, binaryOffset );

	if ( !LoadBinary_R( decBuf, readContext ) )
		return false;

	return true;
}


void KeyValues3::Free()
{
	/*
	char	*m_pszValue;
	byte	*m_pBlob;
	Array_t	*m_pArray;
	Table_t	*m_pTable;
	*/

	switch ( m_nType )
	{
	case KV3_TYPE_STRING:
		delete[] m_pszValue;
		m_pszValue = NULL;
		break;

	case KV3_TYPE_BINARY_BLOB:
		delete m_pBlob;
		m_pBlob = NULL;
		break;

	case KV3_TYPE_ARRAY:
	case KV3_TYPE_ARRAY_TYPED:
	case KV3_TYPE_ARRAY_TYPED_BYTE_LENGTH:
		delete m_pArray;
		m_pArray = NULL;
		break;

	case KV3_TYPE_TABLE:
		delete m_pTable;
		m_pTable = NULL;
		break;

	default:
		break;
	}
}


void KeyValues3::SetTypeAndFree( KV3Type_t nNewType )
{
	// Free any resources associated with the previous type
	Free();

	m_nType = nNewType;
}


KV3Type_t NormalizeType( KV3Type_t nType )
{
	return nType;
}


bool KeyValues3::LoadBinary_R( CUtlBuffer &buf, ReadContext &context, 
	KV3Type_t nType, uint8_t nTypeFlags )
{
	if ( nType == KV3_TYPE_INVALID )
	{
		nType = (KV3Type_t)context.typeTable->Element( context.nCurrentType++ );
		nTypeFlags = 0;
		
		if ( ( nType & 0x80 ) != 0 )
		{
			nType = (KV3Type_t)( nType & 0x7f );
			nTypeFlags = context.typeTable->Element( context.nCurrentType++ );
		}
	}

	KV3Type_t nNormType = NormalizeType( nType );
	SetTypeAndFree( nNormType );
	m_nFlags = nTypeFlags;
	
	switch( nType )
	{
	case KV3_TYPE_NULL:
		SetTypeAndFree( KV3_TYPE_NULL );
		break;

	case KV3_TYPE_BOOL:
		SetBool( buf.GetUnsignedChar() != 0 );
		break;

	case KV3_TYPE_BOOL_TRUE:
		SetBool( true );
		break;

	case KV3_TYPE_BOOL_FALSE:
		SetBool( false );
		break;

	case KV3_TYPE_INT64:
		SetInt64( context.qwordTable->Element( context.nCurrentQWord++ ) );
		break;

	case KV3_TYPE_INT64_ZERO:
		SetInt64( 0 );
		break;

	case KV3_TYPE_INT64_ONE:
		SetInt64( 1 );
		break;

	case KV3_TYPE_DOUBLE:
		SetDouble( *(double*)( &context.qwordTable->Element( context.nCurrentQWord++ ) ) );
		break;

	case KV3_TYPE_DOUBLE_ZERO:
		SetDouble( 0 );
		break;

	case KV3_TYPE_DOUBLE_ONE:
		SetDouble( 1 );
		break;

	case KV3_TYPE_STRING:
		{
			int nStringID = context.dwordTable->Element( context.nCurrentDWord++ );

			if ( nStringID > 0 )
			{
				const CUtlString &sValue = context.stringTable->Element( nStringID );
				SetString( sValue );
			}
			else
			{
				SetString( "" );
			}
		}
		break;

	case KV3_TYPE_BINARY_BLOB:
		{
			int nBlobLength = context.dwordTable->Element( context.nCurrentDWord++ );
			SetBlob( (const byte*)buf.PeekGet(), nBlobLength );
			buf.SeekGet( CUtlBuffer::SEEK_CURRENT, nBlobLength );
		}
		break;

	case KV3_TYPE_ARRAY:
	case KV3_TYPE_ARRAY_TYPED:
	case KV3_TYPE_ARRAY_TYPED_BYTE_LENGTH:
		{
			int nElementCount;
			if ( nType == KV3_TYPE_ARRAY_TYPED_BYTE_LENGTH )
			{
				nElementCount = buf.GetUnsignedChar();
			}
			else
			{
				nElementCount = context.dwordTable->Element( context.nCurrentDWord++ );
			}
			SetEmptyArray( nElementCount );

			KV3Type_t nArrayType = KV3_TYPE_INVALID;
			uint8_t nArrayTypeFlag = 0;

			if ( nType == KV3_TYPE_ARRAY_TYPED || nType == KV3_TYPE_ARRAY_TYPED_BYTE_LENGTH )
			{
				nArrayType = (KV3Type_t)context.typeTable->Element( context.nCurrentType++ );
				nArrayTypeFlag = 0;
				
				if ( ( nArrayType & 0x80 ) != 0 )
				{
					nArrayType = (KV3Type_t)( nArrayType & 0x7f );
					nArrayTypeFlag = context.typeTable->Element( context.nCurrentType++ );
				}
			}

			for ( int i = 0; i < nElementCount; i++ )
			{
				KeyValues3 *pElement = new KeyValues3();
				if ( !pElement->LoadBinary_R( buf, context, nArrayType, nArrayTypeFlag ) )
				{
					delete pElement;
					return false;
				}

				AddArrayElement( pElement );
			}
		}
		break;

	case KV3_TYPE_TABLE:
		{
			int nElementCount = context.dwordTable->Element( context.nCurrentDWord++ );
			SetEmptyTable( nElementCount );

			for ( int i = 0; i < nElementCount; i++ )
			{
				int nNameID = context.dwordTable->Element( context.nCurrentDWord++ );
				const CUtlString &sElementName = context.stringTable->Element( nNameID );
				
				KeyValues3 *pElement = new KeyValues3();
				if ( !pElement->LoadBinary_R( buf, context ) )
				{
					delete pElement;
					return false;
				}

				AddTableElement( sElementName, pElement );
			}
		}
		break;

	case KV3_TYPE_INT32:
		SetInt32( context.dwordTable->Element( context.nCurrentDWord++ ) );
		break;

	default:
		Warning( "[KeyValues3] LoadBinary_R(): Unknown KeyValue type %d\n", nType );
		return false;
	}

	return true;
}


KeyValues3::Blob_t::Blob_t( const byte *pData, size_t nSize )
{
	m_pData = new byte[nSize];
	m_nSize = nSize;
	Q_memcpy( m_pData, pData, nSize );
}


KeyValues3::Blob_t::~Blob_t()
{
	if ( m_pData )
		delete[] m_pData;

	m_nSize = 0;
}
