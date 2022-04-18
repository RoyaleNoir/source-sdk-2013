#include "BaseVSShader.h"

#include "lightpass_vs30.inc"
#include "lightpass_blit_ps30.inc"

BEGIN_VS_SHADER(LightPass_Blit, "Help for light pass blit")
	BEGIN_SHADER_PARAMS
		SHADER_PARAM(ALBEDOTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "_rt_gAlbedo", "")
		SHADER_PARAM(NORMALTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "_rt_gNormal", "")
	END_SHADER_PARAMS
	
	SHADER_INIT
	{
		if (params[ALBEDOTEXTURE]->IsDefined())
		{
			LoadTexture(ALBEDOTEXTURE);
		}

		if (params[NORMALTEXTURE]->IsDefined())
		{
			LoadTexture(NORMALTEXTURE);
		}
	}
	
	SHADER_DRAW
	{
		SHADOW_STATE
		{
			// Vertex Shader State
			unsigned int flags = VERTEX_POSITION;
			pShaderShadow->VertexShaderVertexFormat(flags, 1, 0, 0);

			DECLARE_STATIC_VERTEX_SHADER(lightpass_vs30);
			SET_STATIC_VERTEX_SHADER(lightpass_vs30);

			// Pixel Shader State
			pShaderShadow->EnableTexture(SHADER_SAMPLER0, true);
			pShaderShadow->EnableTexture(SHADER_SAMPLER1, true);

			pShaderShadow->EnableDepthWrites(false);
			// pShaderShadow->EnableAlphaTest(true);

			DECLARE_STATIC_PIXEL_SHADER(lightpass_blit_ps30);
			SET_STATIC_PIXEL_SHADER(lightpass_blit_ps30);
		}

		DYNAMIC_STATE
		{
			DECLARE_DYNAMIC_VERTEX_SHADER(lightpass_vs30);
			SET_DYNAMIC_VERTEX_SHADER(lightpass_vs30);

			BindTexture(SHADER_SAMPLER0, ALBEDOTEXTURE, -1);
			BindTexture(SHADER_SAMPLER1, NORMALTEXTURE, -1);

			DECLARE_DYNAMIC_PIXEL_SHADER(lightpass_blit_ps30);
			SET_DYNAMIC_PIXEL_SHADER(lightpass_blit_ps30);
		}
		Draw();
	}
END_SHADER