#ifndef KEYVALUES3_DESERIALIZE_H
#define KEYVALUES3_DESERIALIZE_H
#ifdef _WIN32
#pragma once
#endif

template < typename T >
inline bool DeserializeKV3( const KeyValues3 *kv, T& t );

template< typename T >
inline bool DeserializeUtlVector( const KeyValues3 *kv, CUtlVector<T> &t )
{
	if ( !kv || kv->GetType() != KV3_TYPE_ARRAY )
		return false;

	t.SetCount( kv->GetArrayCount() );
	for ( size_t i = 0; i < kv->GetArrayCount(); i++ )
	{
		const KeyValues3* pElem = kv->GetArrayElement( i );
		if ( !pElem || !DeserializeKV3( pElem, t[i] ) )
			return false;
	}

	return true;
}

#define KV3_IMPL_DESERIALIZE_NUMBER( _type ) \
	template<> \
	inline bool DeserializeKV3( const KeyValues3 *kv, _type &t ) \
	{ \
		if ( !kv ) \
			return false; \
		t = kv->GetNumber( t ); \
		return true; \
	}

KV3_IMPL_DESERIALIZE_NUMBER( int8 );
KV3_IMPL_DESERIALIZE_NUMBER( uint8 );
KV3_IMPL_DESERIALIZE_NUMBER( int16 );
KV3_IMPL_DESERIALIZE_NUMBER( uint16 );
KV3_IMPL_DESERIALIZE_NUMBER( int32 );
KV3_IMPL_DESERIALIZE_NUMBER( uint32 );
KV3_IMPL_DESERIALIZE_NUMBER( int64 );
KV3_IMPL_DESERIALIZE_NUMBER( uint64 );
KV3_IMPL_DESERIALIZE_NUMBER( float );
KV3_IMPL_DESERIALIZE_NUMBER( double );

template<> 
inline bool DeserializeKV3( const KeyValues3 *kv, Vector &t )
{
	// Overall type check
	if ( !kv || kv->GetType() != KV3_TYPE_ARRAY )
		return false;

	if ( kv->GetArrayCount() != 3 )
		return false;

	for ( size_t i = 0; i < 3; i++ )
	{
		const KeyValues3 *pElem = kv->GetArrayElement( i );
		if ( !pElem || !DeserializeKV3( pElem, t[i] ) )
			return false;
	}

	return true;
}

template<> 
inline bool DeserializeKV3( const KeyValues3 *kv, Quaternion &t )
{
	// Overall type check
	if ( !kv || kv->GetType() != KV3_TYPE_ARRAY )
		return false;

	if ( kv->GetArrayCount() != 4 )
		return false;

	for ( size_t i = 0; i < 4; i++ )
	{
		const KeyValues3 *pElem = kv->GetArrayElement( i );
		if ( !pElem || !DeserializeKV3( pElem, t[i] ) )
			return false;
	}

	return true;
}

template <>
inline bool DeserializeKV3( const KeyValues3 *kv, CUtlString &t )
{
	if ( !kv )
		return false;

	if ( kv->GetType() != KV3_TYPE_STRING )
		return false;

	t = kv->GetString();

	return true;
}

#define BEGIN_KV3_TABLE( _type ) \
	template <> \
	inline bool DeserializeKV3( const KeyValues3* kv, _type &t ) \
	{ \
		if ( !kv || kv->GetType() != KV3_TYPE_TABLE ) \
			return false;

#define KV3_TABLE_ELEM( _name ) \
		if ( !DeserializeKV3( kv->GetTableElement( #_name ), t._name ) ) \
			return false;

#define KV3_TABLE_ARRY( _name ) \
		if ( !DeserializeUtlVector( kv->GetTableElement( #_name ), t._name ) ) \
			return false;

#define END_KV3_TABLE() \
		return true; \
	}

#define BEGIN_KV3_ENUM( _type ) \
	template <> \
	inline bool DeserializeKV3( const KeyValues3 *kv, _type &t ) \
	{ \
		typedef _type CurType; \
		if ( !kv ) \
			return false; \
		if ( kv->GetType() == KV3_TYPE_STRING ) \
		{

#define KV3_ENUM_VALUE( _name ) \
	if ( !Q_strcmp( kv->GetString(), #_name ) ) \
	{ \
		t = CurType::_name; \
		return true; \
	}

#define END_KV3_ENUM() \
			return false; \
		} \
		t = (CurType)kv->GetNumber<uint64>();\
		return true; \
	}

#endif // KEYVALUES3_DESERIALIZE_H