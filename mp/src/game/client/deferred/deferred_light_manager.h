#ifndef DEFERRED_LIGHT_MANAGER_H
#define DEFERRED_LIGHT_MANAGER_H
#ifdef _WIN32
#pragma once
#endif // _WIN32

struct DeferredLightData_t
{
	Vector position;
	Vector direction;
	Vector color;
	float innerCone;
	float outerCone;
	int type;
	bool valid;

	Vector mins;
	Vector maxs;
	bool boundsCalced;
};

class CDeferredLightManager : public CAutoGameSystem
{
public:
	CDeferredLightManager(const char* name);

	virtual void LevelShutdownPostEntity();

	void GetLightBounds(float& radius, Vector& mins, Vector& maxs, DeferredLightData_t* light);

public:
	CUtlVector<DeferredLightData_t *> m_lightData;
};

CDeferredLightManager* GetDeferredLightManager(void);

#endif // !DEFERRED_LIGHT_MANAGER_H
