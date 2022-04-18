#include "BaseVSShader.h"

#include "lightpass_vs30.inc"
#include "lightpass_ssao_ps30.inc"

BEGIN_VS_SHADER(LightPass_SSAO, "Help for light pass")
	BEGIN_SHADER_PARAMS
		SHADER_PARAM(ALBEDOTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "_rt_gAlbedo", "")
		SHADER_PARAM(NORMALTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "_rt_gNormal", "")
		SHADER_PARAM(DEPTHTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "_rt_gDepth", "")
		SHADER_PARAM(NOISETEXTURE, SHADER_PARAM_TYPE_TEXTURE, "deferred/bluenoise", "")
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

		if (params[NOISETEXTURE]->IsDefined())
		{
			LoadTexture(NOISETEXTURE);
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
			pShaderShadow->EnableTexture(SHADER_SAMPLER3, true);

			// pShaderShadow->EnableSRGBRead(SHADER_SAMPLER0, true);
			// pShaderShadow->EnableSRGBWrite(true);

			pShaderShadow->EnableDepthWrites(false);

			// pShaderShadow->EnableBlending(true);
			// pShaderShadow->BlendFunc(SHADER_BLEND_ONE, SHADER_BLEND_ONE);

			DECLARE_STATIC_PIXEL_SHADER(lightpass_ssao_ps30);
			SET_STATIC_PIXEL_SHADER(lightpass_ssao_ps30);
		}

		DYNAMIC_STATE
		{
			DECLARE_DYNAMIC_VERTEX_SHADER(lightpass_vs30);
			SET_DYNAMIC_VERTEX_SHADER(lightpass_vs30);

			BindTexture(SHADER_SAMPLER0, ALBEDOTEXTURE, -1);
			BindTexture(SHADER_SAMPLER1, NORMALTEXTURE, -1);
			BindTexture(SHADER_SAMPLER2, DEPTHTEXTURE, -1);
			BindTexture(SHADER_SAMPLER3, NOISETEXTURE, -1);

			VMatrix* viewMatrix = (VMatrix*)pShaderAPI->GetIntRenderingParameter(INT_RENDERPARM_DEFERRED_INVVIEWMATRIX_ADDR);
			VMatrix* projMatrix = (VMatrix*)pShaderAPI->GetIntRenderingParameter(INT_RENDERPARM_DEFERRED_INVPROJMATRIX_ADDR);

			if (viewMatrix)
				pShaderAPI->SetPixelShaderConstant(0, viewMatrix->Base(), 4);

			if (projMatrix)
				pShaderAPI->SetPixelShaderConstant(4, projMatrix->Base(), 4);

			ITexture* src_texture = params[ALBEDOTEXTURE]->GetTextureValue();
			ITexture* noise_texture = params[NOISETEXTURE]->GetTextureValue();

			float screenSizeValue[4] = { 
				src_texture->GetActualWidth(), 
				src_texture->GetActualHeight(), 
				(float)src_texture->GetActualWidth() / noise_texture->GetActualWidth(),
				(float)src_texture->GetActualHeight() / noise_texture->GetActualHeight()
			};
			pShaderAPI->SetPixelShaderConstant(8, screenSizeValue);

			DECLARE_DYNAMIC_PIXEL_SHADER(lightpass_ssao_ps30);
			SET_DYNAMIC_PIXEL_SHADER(lightpass_ssao_ps30);
		}
		Draw();
	}
END_SHADER