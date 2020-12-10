#include "BaseVSShader.h"
#include "ssao_downsample_vs30.inc"
#include "ssao_downsample_ps30.inc"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

BEGIN_VS_SHADER_FLAGS(ssao_downsample, "Help for the SSAO shader.", SHADER_NOT_EDITABLE)

BEGIN_SHADER_PARAMS
SHADER_PARAM(BASETEXTURE, SHADER_PARAM_TYPE_TEXTURE, "_rt_SSAODepth", "")
END_SHADER_PARAMS

SHADER_INIT
{
	if (params[BASETEXTURE]->IsDefined())
	{
		LoadTexture(BASETEXTURE);
	}
}

SHADER_FALLBACK
{
	return 0;
}

SHADER_DRAW
{
	SHADOW_STATE
	{
		pShaderShadow->VertexShaderVertexFormat(VERTEX_POSITION, 1, 0, 0);

		pShaderShadow->EnableTexture(SHADER_SAMPLER0, true);

		DECLARE_STATIC_VERTEX_SHADER(ssao_downsample_vs30);
		SET_STATIC_VERTEX_SHADER(ssao_downsample_vs30);

		DECLARE_STATIC_PIXEL_SHADER(ssao_downsample_ps30);
		SET_STATIC_PIXEL_SHADER(ssao_downsample_ps30);
	}

	DYNAMIC_STATE
	{
		BindTexture(SHADER_SAMPLER0, BASETEXTURE, -1);

		DECLARE_DYNAMIC_VERTEX_SHADER(ssao_downsample_vs30);
		SET_DYNAMIC_VERTEX_SHADER(ssao_downsample_vs30);

		DECLARE_DYNAMIC_PIXEL_SHADER(ssao_downsample_ps30);
		SET_DYNAMIC_PIXEL_SHADER(ssao_downsample_ps30);

	}
	Draw();
}

END_SHADER