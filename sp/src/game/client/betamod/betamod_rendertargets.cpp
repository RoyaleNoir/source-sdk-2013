#include "cbase.h"
#include "betamod_rendertargets.h"
#include "materialsystem\imaterialsystem.h"
#include "rendertexture.h"

ITexture* CBMRenderTargets::CreateFloatLDRTexture(IMaterialSystem* pMaterialSystem)
{
	//	DevMsg("Creating Scope Render Target: _rt_Scope\n");
	return pMaterialSystem->CreateNamedRenderTargetTextureEx2(
		"_rt_LDRBuffer",
		1, 1, RT_SIZE_FULL_FRAME_BUFFER,
		IMAGE_FORMAT_RGBA16161616F,
		MATERIAL_RT_DEPTH_SHARED,
		TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT,
		CREATERENDERTARGETFLAGS_HDR);

}

ITexture* CBMRenderTargets::CreateFloatHDRTexture(IMaterialSystem* pMaterialSystem)
{
	//	DevMsg("Creating Scope Render Target: _rt_Scope\n");
	return pMaterialSystem->CreateNamedRenderTargetTextureEx2(
		"_rt_HDRBuffer",
		1, 1, RT_SIZE_FULL_FRAME_BUFFER,
		IMAGE_FORMAT_RGBA16161616F,
		MATERIAL_RT_DEPTH_SHARED,
		TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT,
		CREATERENDERTARGETFLAGS_HDR);

}

ITexture* CBMRenderTargets::CreateSSAODepthTexture(IMaterialSystem* pMaterialSystem, int width, int height, int index, const char* name)
{
	int divisor = (1 << index);

	return pMaterialSystem->CreateNamedRenderTargetTextureEx2(
		name,
		width / divisor, height / divisor, RT_SIZE_NO_CHANGE,
		IMAGE_FORMAT_R32F,
		MATERIAL_RT_DEPTH_NONE,
		TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT | TEXTUREFLAGS_RENDERTARGET | TEXTUREFLAGS_DEPTHRENDERTARGET,
		0);
}

ITexture* CBMRenderTargets::CreateZBufferTexture(IMaterialSystem* pMaterialSystem)
{
	return pMaterialSystem->CreateNamedRenderTargetTextureEx2(
		"_rt_ZBuffer",
		1, 1, RT_SIZE_FULL_FRAME_BUFFER,
		IMAGE_FORMAT_NV_INTZ,
		MATERIAL_RT_DEPTH_NONE,
		TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT | TEXTUREFLAGS_RENDERTARGET | TEXTUREFLAGS_DEPTHRENDERTARGET,
		0);
}

//-----------------------------------------------------------------------------
// Purpose: Called by the engine in material system init and shutdown.
//			Clients should override this in their inherited version, but the base
//			is to init all standard render targets for use.
// Input  : pMaterialSystem - the engine's material system (our singleton is not yet inited at the time this is called)
//			pHardwareConfig - the user hardware config, useful for conditional render target setup
//-----------------------------------------------------------------------------
void CBMRenderTargets::InitClientRenderTargets(IMaterialSystem* pMaterialSystem, IMaterialSystemHardwareConfig* pHardwareConfig)
{
	int iWidth, iHeight;
	pMaterialSystem->GetBackBufferDimensions(iWidth, iHeight);
	m_FloatLDRTexture.Init(CreateFloatLDRTexture(pMaterialSystem));
	m_FloatHDRTexture.Init(CreateFloatHDRTexture(pMaterialSystem));
	m_ZBufferTexture.Init(CreateZBufferTexture(pMaterialSystem));

	m_SSAODepth.Init(CreateSSAODepthTexture(pMaterialSystem, iWidth, iHeight, 0, "_rt_SSAODepth"));
	m_SSAODepth1.Init(CreateSSAODepthTexture(pMaterialSystem, iWidth, iHeight, 1, "_rt_SSAODepth1"));
	m_SSAODepth2.Init(CreateSSAODepthTexture(pMaterialSystem, iWidth, iHeight, 2, "_rt_SSAODepth2"));

	// Water effects & camera from the base class (standard HL2 targets) 
	BaseClass::InitClientRenderTargets(pMaterialSystem, pHardwareConfig);
}

//-----------------------------------------------------------------------------
// Purpose: Shut down each CTextureReference we created in InitClientRenderTargets.
//			Called by the engine in material system shutdown.
// Input  :  - 
//-----------------------------------------------------------------------------
void CBMRenderTargets::ShutdownClientRenderTargets()
{
	m_FloatHDRTexture.Shutdown();
	m_ZBufferTexture.Shutdown();

	m_SSAODepth.Shutdown();
	m_SSAODepth1.Shutdown();
	m_SSAODepth2.Shutdown();

	// Clean up standard HL2 RTs (camera and water) 
	BaseClass::ShutdownClientRenderTargets();
}

//add the interface!
static CBMRenderTargets g_BMRenderTargets;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CBMRenderTargets, IClientRenderTargets, CLIENTRENDERTARGETS_INTERFACE_VERSION, g_BMRenderTargets);
CBMRenderTargets* g_pBMRenderTargets = &g_BMRenderTargets;