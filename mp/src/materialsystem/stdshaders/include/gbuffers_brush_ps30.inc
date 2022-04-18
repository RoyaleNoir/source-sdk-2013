#include "shaderlib/cshader.h"

#define shaderStaticTest_gbuffers_brush_ps30 (psh_forgot_to_set_static_BUMPMAP + 0)

class gbuffers_brush_ps30_Static_Index
{
public:
	gbuffers_brush_ps30_Static_Index( void )
	{
		m_nBUMPMAP = 0;
#ifdef _DEBUG
		m_bBUMPMAP = false;
#endif	// _DEBUG
	}

	int GetIndex( void )
	{
		// Asserts to make sure that we aren't using any skipped combinations.

#ifdef _DEBUG
		// Asserts to make sure that we are setting all of the combination vars.
		bool bAllStaticVarsDefined = m_bBUMPMAP;
		Assert( bAllStaticVarsDefined );
#endif	// _DEBUG

		return ( 0x1 * m_nBUMPMAP ) + 0;
	}

	void SetBUMPMAP( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBUMPMAP = i;
#ifdef _DEBUG
		m_bBUMPMAP = true;
#endif	// _DEBUG
	}

	void SetBUMPMAP( bool i )
	{
		m_nBUMPMAP = i ? 1 : 0;
#ifdef _DEBUG
		m_bBUMPMAP = true;
#endif	// _DEBUG
	}

private:
	 int m_nBUMPMAP;
#ifdef _DEBUG
	 bool m_bBUMPMAP;
#endif	// _DEBUG
};


#define shaderDynamicTest_gbuffers_brush_ps30 (0)

class gbuffers_brush_ps30_Dynamic_Index
{
public:
	gbuffers_brush_ps30_Dynamic_Index( void )
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
