#include "BaseVSShader.h"

#include "SDK_screenspaceeffect_vs20.inc"

#include "ldr_to_hdr_ps20.inc"
#include "ldr_to_hdr_ps20b.inc"

BEGIN_SHADER( LDR_To_HDR, "Help for the LDR_To_HDR shader." )

	// ----------------------------------------------------------------------------
	// This block is where you'd define inputs that users can feed to your
	// shader.
	// ----------------------------------------------------------------------------
	BEGIN_SHADER_PARAMS
	END_SHADER_PARAMS

	// ----------------------------------------------------------------------------
	// This is the shader initialization block. This disgusting macro defines
	// a bunch of ick that makes this shader work.
	// ----------------------------------------------------------------------------
	SHADER_INIT
	{
		if (params[BASETEXTURE]->IsDefined())
		{
			LoadTexture(BASETEXTURE);
		}
	}

	// ----------------------------------------------------------------------------
	// We want this shader to operate on the frame buffer itself. Therefore,
	// we need to set this to true.
	// ----------------------------------------------------------------------------
	bool NeedsFullFrameBufferTexture(IMaterialVar **params, bool bCheckSpecificToThisFrame /* = true */) const
	{
		return true;
	}

	// ----------------------------------------------------------------------------
	// This block should return the name of the shader to fall back to if
	// we fail to bind this shader for any reason.
	// ----------------------------------------------------------------------------
	SHADER_FALLBACK
	{
		// Requires DX9 + above
		if (g_pHardwareConfig->GetDXSupportLevel() < 90)
		{
			Assert(0);
			return "Wireframe";
		}
		return 0;
	}

	// ----------------------------------------------------------------------------
	// This implements the guts of the shader drawing code.
	// ----------------------------------------------------------------------------
	SHADER_DRAW
	{
		// ----------------------------------------------------------------------------
		// This section is called when the shader is bound for the first time.
		// You should setup any static state variables here.
		// ----------------------------------------------------------------------------
		SHADOW_STATE
		{
			// Setup the vertex format.
			int fmt = VERTEX_POSITION;
			pShaderShadow->VertexShaderVertexFormat(fmt, 1, 0, 0);

			// We don't need to write to the depth buffer.
			pShaderShadow->EnableDepthWrites(false);

			pShaderShadow->EnableTexture(SHADER_SAMPLER0, true);

			// Precache and set the screenspace shader.
			DECLARE_STATIC_VERTEX_SHADER(sdk_screenspaceeffect_vs20);
			SET_STATIC_VERTEX_SHADER(sdk_screenspaceeffect_vs20);

			// Precache and set the example shader.
			if (g_pHardwareConfig->SupportsPixelShaders_2_b())
			{
				DECLARE_STATIC_PIXEL_SHADER(ldr_to_hdr_ps20b);
				SET_STATIC_PIXEL_SHADER(ldr_to_hdr_ps20b);
			}
			else
			{
				DECLARE_STATIC_PIXEL_SHADER(ldr_to_hdr_ps20);
				SET_STATIC_PIXEL_SHADER(ldr_to_hdr_ps20);
			}
		}

		// ----------------------------------------------------------------------------
		// This section is called every frame.
		// ----------------------------------------------------------------------------
		DYNAMIC_STATE
		{
			BindTexture(SHADER_SAMPLER0, BASETEXTURE, -1);

			// Use the sdk_screenspaceeffect_vs20 vertex shader.
			DECLARE_DYNAMIC_VERTEX_SHADER(sdk_screenspaceeffect_vs20);
			SET_DYNAMIC_VERTEX_SHADER(sdk_screenspaceeffect_vs20);

			// Use our custom pixel shader.
			if (g_pHardwareConfig->SupportsPixelShaders_2_b())
			{
				DECLARE_DYNAMIC_PIXEL_SHADER(ldr_to_hdr_ps20b);
				SET_DYNAMIC_PIXEL_SHADER(ldr_to_hdr_ps20b);
			}
			else
			{
				DECLARE_DYNAMIC_PIXEL_SHADER(ldr_to_hdr_ps20);
				SET_DYNAMIC_PIXEL_SHADER(ldr_to_hdr_ps20);
			}
		}

		// NEVER FORGET THIS CALL! This is what actually
		// draws your shader!
		Draw();
	}

END_SHADER