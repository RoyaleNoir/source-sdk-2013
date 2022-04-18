#include "BaseVSShader.h"

#include "lightpass_vs30.inc"
#include "lightpass_ps30.inc"

BEGIN_VS_SHADER(LightPass, "Help for light pass")
	BEGIN_SHADER_PARAMS
		SHADER_PARAM(ALBEDOTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "_rt_gAlbedo", "")
		SHADER_PARAM(NORMALTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "_rt_gNormal", "")
		SHADER_PARAM(DEPTHTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "_rt_gDepth", "")
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

		if (params[DEPTHTEXTURE]->IsDefined())
		{
			LoadTexture(DEPTHTEXTURE);
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
			pShaderShadow->EnableTexture(SHADER_SAMPLER2, true);

			pShaderShadow->EnableSRGBRead(SHADER_SAMPLER0, true);
			pShaderShadow->EnableSRGBWrite(true);

			pShaderShadow->EnableDepthWrites(false);

			pShaderShadow->EnableBlending(true);
			pShaderShadow->BlendFunc(SHADER_BLEND_ONE, SHADER_BLEND_ONE);

			DECLARE_STATIC_PIXEL_SHADER(lightpass_ps30);
			SET_STATIC_PIXEL_SHADER(lightpass_ps30);
		}

		DYNAMIC_STATE
		{
			DECLARE_DYNAMIC_VERTEX_SHADER(lightpass_vs30);
			SET_DYNAMIC_VERTEX_SHADER(lightpass_vs30);

			BindTexture(SHADER_SAMPLER0, ALBEDOTEXTURE, -1);
			BindTexture(SHADER_SAMPLER1, NORMALTEXTURE, -1);
			BindTexture(SHADER_SAMPLER2, DEPTHTEXTURE, -1);

			VMatrix* viewMatrix = (VMatrix*)pShaderAPI->GetIntRenderingParameter(INT_RENDERPARM_DEFERRED_INVVIEWMATRIX_ADDR);
			VMatrix* projMatrix = (VMatrix*)pShaderAPI->GetIntRenderingParameter(INT_RENDERPARM_DEFERRED_INVPROJMATRIX_ADDR);

			Vector lightPos = pShaderAPI->GetVectorRenderingParameter(VECTOR_RENDERPARM_DEFERRED_LIGHTPOS);
			Vector lightColor = pShaderAPI->GetVectorRenderingParameter(VECTOR_RENDERPARM_DEFERRED_LIGHTCOLOR);

			float lightPosValue[4] = { lightPos.x, lightPos.y, lightPos.z, 0.0f };
			float lightColorValue[4] = { lightColor.x, lightColor.y, lightColor.z, 0.0f };

			lightPosValue[3] = pShaderAPI->GetFloatRenderingParameter(FLOAT_RENDERPARM_DEFERRED_LIGHTCUTOFF) * 0.0254f;

			if (viewMatrix)
				pShaderAPI->SetPixelShaderConstant(0, viewMatrix->Base(), 4);

			if (projMatrix)
				pShaderAPI->SetPixelShaderConstant(4, projMatrix->Base(), 4);

			pShaderAPI->SetPixelShaderConstant(8, lightPosValue, 1);
			pShaderAPI->SetPixelShaderConstant(9, lightColorValue, 1);

			int lightType = pShaderAPI->GetIntRenderingParameter(INT_RENDERPARM_DEFERRED_LIGHTTYPE);

			if (lightType == 1)
			{
				Vector lightDir = pShaderAPI->GetVectorRenderingParameter(VECTOR_RENDERPARM_DEFERRED_LIGHTDIR);

				float lightDirValue[4] = { lightDir.x, lightDir.y, lightDir.z, 0.0f };
				float lightConeValue[4] = { 
					pShaderAPI->GetFloatRenderingParameter(FLOAT_RENDERPARM_DEFERRED_LIGHTINNERCONE),
					pShaderAPI->GetFloatRenderingParameter(FLOAT_RENDERPARM_DEFERRED_LIGHTOUTERCONE),
					0.0f, 0.0f 
				};

				pShaderAPI->SetPixelShaderConstant(10, lightDirValue, 1);
				pShaderAPI->SetPixelShaderConstant(11, lightConeValue, 1);
			}

			DECLARE_DYNAMIC_PIXEL_SHADER(lightpass_ps30);
			SET_DYNAMIC_PIXEL_SHADER_COMBO(LIGHTTYPE, lightType == 1 ? 1 : 0);
			SET_DYNAMIC_PIXEL_SHADER(lightpass_ps30);
		}
		Draw();
	}
END_SHADER