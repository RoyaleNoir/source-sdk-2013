#ifndef DEFERRED_RENDERTARGETS_H
#define DEFERRED_RENDERTARGETS_H
#ifdef _WIN32
#pragma once
#endif

#include "baseclientrendertargets.h"

class CDeferredRenderTargets : public CBaseClientRenderTargets
{
	DECLARE_CLASS_GAMEROOT(CDeferredRenderTargets, CBaseClientRenderTargets);
public:
	virtual void InitClientRenderTargets(IMaterialSystem* pMaterialSystem, IMaterialSystemHardwareConfig* pHardwareConfig);
	virtual void ShutdownClientRenderTargets();

	ITexture* CreateAlbedoTexture(IMaterialSystem* pMaterialSystem);
	ITexture* CreateNormalTexture(IMaterialSystem* pMaterialSystem);
	ITexture* CreateDepthTexture(IMaterialSystem* pMaterialSystem);

	ITexture* GetAlbedoTexture();
	ITexture* GetNormalTexture();
	ITexture* GetDepthTexture();

private:
	CTextureReference m_albedoTexture;
	CTextureReference m_normalTexture;
	CTextureReference m_depthTexture;
};

extern CDeferredRenderTargets* g_pDeferredRenderTargets;

#endif // !DEFERRED_RENDERTARGETS_H
