#ifndef BETAMOD_RENDERTARGETS_H
#define BETAMOD_RENDERTARGETS_H

#ifdef WIN32
#pragma once
#endif

#include "baseclientrendertargets.h" // Base class, with interfaces called by engine and inherited members to init common render   targets

// externs
class IMaterialSystem;
class IMaterialSystemHardwareConfig;

class CBMRenderTargets : public CBaseClientRenderTargets
{
	// no networked vars 
	DECLARE_CLASS_GAMEROOT(CBMRenderTargets, CBaseClientRenderTargets);
public:
	virtual void InitClientRenderTargets(IMaterialSystem* pMaterialSystem, IMaterialSystemHardwareConfig* pHardwareConfig);
	virtual void ShutdownClientRenderTargets();

	ITexture* CreateFloatLDRTexture(IMaterialSystem* pMaterialSystem);
	ITexture* CreateFloatHDRTexture(IMaterialSystem* pMaterialSystem);
	ITexture* CreateSSAODepthTexture(IMaterialSystem* pMaterialSystem, int width, int height, int index, const char* name);

	ITexture* CreateZBufferTexture(IMaterialSystem* pMaterialSystem);

private:
	CTextureReference		m_FloatLDRTexture;
	CTextureReference		m_FloatHDRTexture;
	CTextureReference		m_ZBufferTexture;

	CTextureReference		m_SSAODepth;
	CTextureReference		m_SSAODepth1;
	CTextureReference		m_SSAODepth2;
};

extern CBMRenderTargets* g_pBMRenderTargets;

#endif // !BETAMOD_RENDERTARGETS_H
