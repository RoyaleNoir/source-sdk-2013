#include "shaderlib/cshader.h"

#define shaderStaticTest_lightpass_ps30 (0)

class lightpass_ps30_Static_Index
{
public:
	lightpass_ps30_Static_Index( void )
	{
#ifdef _DEBUG
#endif	// _DEBUG
	}

	int GetIndex( void )
	{
		// Asserts to make sure that we aren't using any skipped combinations.

#ifdef _DEBUG
		// Asserts to make sure that we are setting all of the combination vars.
#endif	// _DEBUG

		return 0;
	}

private:
#ifdef _DEBUG
#endif	// _DEBUG
};


#define shaderDynamicTest_lightpass_ps30 (psh_forgot_to_set_dynamic_LIGHTTYPE + 0)

class lightpass_ps30_Dynamic_Index
{
public:
	lightpass_ps30_Dynamic_Index( void )
	{
		m_nLIGHTTYPE = 0;
#ifdef _DEBUG
		m_bLIGHTTYPE = false;
#endif	// _DEBUG
	}

	int GetIndex( void )
	{
		// Asserts to make sure that we aren't using any skipped combinations.

#ifdef _DEBUG
		// Asserts to make sure that we are setting all of the combination vars.
		bool bAllDynamicVarsDefined = m_bLIGHTTYPE;
		Assert( bAllDynamicVarsDefined );
#endif	// _DEBUG

		return ( 0x1 * m_nLIGHTTYPE ) + 0;
	}

	void SetLIGHTTYPE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nLIGHTTYPE = i;
#ifdef _DEBUG
		m_bLIGHTTYPE = true;
#endif	// _DEBUG
	}

	void SetLIGHTTYPE( bool i )
	{
		m_nLIGHTTYPE = i ? 1 : 0;
#ifdef _DEBUG
		m_bLIGHTTYPE = true;
#endif	// _DEBUG
	}

private:
	 int m_nLIGHTTYPE;
#ifdef _DEBUG
	 bool m_bLIGHTTYPE;
#endif	// _DEBUG
};
