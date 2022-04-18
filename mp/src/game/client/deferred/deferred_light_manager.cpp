#include "cbase.h"
#include "debugoverlay_shared.h"
#include "deferred_light_manager.h"

#include "tier0/memdbgon.h"

const static float lightRadius = 39.3701 / sqrt(0.004);
static void CalculateSpotBBox(DeferredLightData_t* light, Vector& mins, Vector& maxs, float radius);

CDeferredLightManager s_DeferredLightManager("DeferredLightManager");

CDeferredLightManager* GetDeferredLightManager(void)
{
	return &s_DeferredLightManager;
}

CDeferredLightManager::CDeferredLightManager(const char* name) : CAutoGameSystem(name)
{
	// m_lightData = CUtlVector<DeferredLightData_t>();
}

void CDeferredLightManager::LevelShutdownPostEntity()
{
	m_lightData.Purge();
}

void CDeferredLightManager::GetLightBounds(float& radius, Vector& mins, Vector& maxs, DeferredLightData_t* light)
{
	radius = light->color.Dot(Vector(1.0f / 3.0f)) * lightRadius;
	radius = min(radius, 1024);

	if (!light->boundsCalced)
	{
		if (light->type == 0)
		{
			light->mins = light->position - Vector(radius);
			light->maxs = light->position + Vector(radius);
		}
		else if (light->type == 1)
		{
			CalculateSpotBBox(light, light->mins, light->maxs, radius);
		}

		light->boundsCalced = true;
	}

	mins = light->mins;
	maxs = light->maxs;
}

// https://stackoverflow.com/a/64706803
static float SolveSpotAxis(float dotAxis, float cosAngle)
{
	if (dotAxis >= cosAngle)
		return 1.0;

	float det = (cosAngle * cosAngle - 1.0f) / (dotAxis * dotAxis - 1.0f);
	if (det >= 0.0f)
	{
		float a = sqrt(det);

		float x0 = (cosAngle - a * dotAxis) * dotAxis + a;
		float x1 = (cosAngle + a * dotAxis) * dotAxis - a;

		return clamp(max(x0, x1), 0.0f, 1.0f);
	}

	return 0.0f;
}

static void CalculateSpotBBox(DeferredLightData_t* light, Vector& mins, Vector& maxs, float radius)
{
	mins = Vector(
		min(0.0f, -SolveSpotAxis(-light->direction.x, light->outerCone)),
		min(0.0f, -SolveSpotAxis(-light->direction.y, light->outerCone)),
		min(0.0f, -SolveSpotAxis(-light->direction.z, light->outerCone))
	) * radius + light->position;

	maxs = Vector(
		max(0.0f, SolveSpotAxis(light->direction.x, light->outerCone)),
		max(0.0f, SolveSpotAxis(light->direction.y, light->outerCone)),
		max(0.0f, SolveSpotAxis(light->direction.z, light->outerCone))
	) * radius + light->position;
}
