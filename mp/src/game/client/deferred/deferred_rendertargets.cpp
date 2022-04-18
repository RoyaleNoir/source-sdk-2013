#include "cbase.h"
#include "deferred_rendertargets.h"

#include "tier0/memdbgon.h"


//===========//
// Interface //
//===========//

static CDeferredRenderTargets g_DeferredRenderTargets;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CDeferredRenderTargets, IClientRenderTargets, CLIENTRENDERTARGETS_INTERFACE_VERSION, g_DeferredRenderTargets);
CDeferredRenderTargets* g_pDeferredRenderTargets = &g_DeferredRenderTargets;

void CDeferredRenderTargets::InitClientRenderTargets(IMaterialSystem* pMaterialSystem, IMaterialSystemHardwareConfig* pHardwareConfig)
{
	m_albedoTexture.Init(CreateAlbedoTexture(pMaterialSystem));
	m_normalTexture.Init(CreateNormalTexture(pMaterialSystem));
	m_depthTexture.Init(CreateDepthTexture(pMaterialSystem));

	BaseClass::InitClientRenderTargets(pMaterialSystem, pHardwareConfig);
}

void CDeferredRenderTargets::ShutdownClientRenderTargets()
{
	m_albedoTexture.Shutdown();
	m_normalTexture.Shutdown();
	m_depthTexture.Shutdown();

	BaseClass::ShutdownClientRenderTargets();
}


//======================//
// Texture Initializers //
//======================//

ITexture* CDeferredRenderTargets::CreateAlbedoTexture(IMaterialSystem* pMaterialSystem)
{
	return pMaterialSystem->CreateNamedRenderTargetTexture(
		"_rt_gAlbedo", 0, 0, RT_SIZE_FULL_FRAME_BUFFER, IMAGE_FORMAT_RGBA8888
	);
}

ITexture* CDeferredRenderTargets::CreateNormalTexture(IMaterialSystem* pMaterialSystem)
{
	return pMaterialSystem->CreateNamedRenderTargetTexture(
		"_rt_gNormal", 0, 0, RT_SIZE_FULL_FRAME_BUFFER, IMAGE_FORMAT_RGBA8888
	);
}

ITexture* CDeferredRenderTargets::CreateDepthTexture(IMaterialSystem* pMaterialSystem)
{
	return pMaterialSystem->CreateNamedRenderTargetTexture(
		"_rt_gDepth", 0, 0, RT_SIZE_FULL_FRAME_BUFFER, IMAGE_FORMAT_RGBA8888
	);
}


//===================//
// Texture Accessors //
//===================//

ITexture* CDeferredRenderTargets::GetAlbedoTexture()
{
	return m_albedoTexture;
}

ITexture* CDeferredRenderTargets::GetNormalTexture()
{
	return m_normalTexture;
}

ITexture* CDeferredRenderTargets::GetDepthTexture()
{
	return m_depthTexture;
}
