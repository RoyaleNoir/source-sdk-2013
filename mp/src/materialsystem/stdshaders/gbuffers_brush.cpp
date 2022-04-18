#include "BaseVSShader.h"

#include "gbuffers_brush_vs30.inc"
#include "gbuffers_brush_ps30.inc"

BEGIN_VS_SHADER(GBuffers_Brush, "Help for brush GBuffer pass")
	BEGIN_SHADER_PARAMS
		SHADER_PARAM(ALPHATESTREFERENCE, SHADER_PARAM_TYPE_FLOAT, "0.5", "")
		SHADER_PARAM(BUMPMAP, SHADER_PARAM_TYPE_TEXTURE, "models/shadertest/shader1_normal", "bump map")
		SHADER_PARAM(BUMPFRAME, SHADER_PARAM_TYPE_INTEGER, "0", "frame number for $bumpmap")
	END_SHADER_PARAMS
	
	SHADER_INIT
	{
		LoadTexture(BASETEXTURE);

		if (g_pConfig->UseBumpmapping() && params[BUMPMAP]->IsDefined())
		{
			LoadBumpMap(BUMPMAP);
		}
	}
	
	SHADER_DRAW
	{
		SHADOW_STATE
		{
			bool hasBump = (params[BUMPMAP]->IsTexture());

			// Vertex Shader State
			unsigned int flags = VERTEX_POSITION | VERTEX_TANGENT_S | VERTEX_TANGENT_T | VERTEX_NORMAL;
			pShaderShadow->VertexShaderVertexFormat(flags, 1, 0, 0);

			DECLARE_STATIC_VERTEX_SHADER(gbuffers_brush_vs30);
			SET_STATIC_VERTEX_SHADER(gbuffers_brush_vs30);

			// Pixel Shader State
			pShaderShadow->EnableTexture(SHADER_SAMPLER0, true);

			if (hasBump)
				pShaderShadow->EnableTexture(SHADER_SAMPLER1, true);

			pShaderShadow->EnableAlphaTest(IS_FLAG_SET(MATERIAL_VAR_ALPHATEST));
			if (params[ALPHATESTREFERENCE]->IsDefined() && params[ALPHATESTREFERENCE]->GetFloatValue() > 0.0f)
			{
				pShaderShadow->AlphaFunc(SHADER_ALPHAFUNC_GEQUAL, params[ALPHATESTREFERENCE]->GetFloatValue());
			}

			DECLARE_STATIC_PIXEL_SHADER(gbuffers_brush_ps30);
			SET_STATIC_PIXEL_SHADER_COMBO(BUMPMAP, hasBump);
			SET_STATIC_PIXEL_SHADER(gbuffers_brush_ps30);
		}

		DYNAMIC_STATE
		{

			BindTexture(SHADER_SAMPLER0, BASETEXTURE, FRAME);

			if (params[BUMPMAP]->IsTexture())
				BindTexture(SHADER_SAMPLER1, BUMPMAP, BUMPFRAME);

			DECLARE_DYNAMIC_VERTEX_SHADER(gbuffers_brush_vs30);
			SET_DYNAMIC_VERTEX_SHADER(gbuffers_brush_vs30);

			DECLARE_DYNAMIC_PIXEL_SHADER(gbuffers_brush_ps30);
			SET_DYNAMIC_PIXEL_SHADER(gbuffers_brush_ps30);
		}
		Draw();
	}
END_SHADER
