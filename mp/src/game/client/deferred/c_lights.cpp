#include "cbase.h"
#include "deferred_light_manager.h"

#include "tier0/memdbgon.h"

class C_Light : public C_BaseEntity
{
public:
	DECLARE_CLASS(C_Light, C_BaseEntity);
	DECLARE_NETWORKCLASS();

	C_Light();

	virtual void Spawn(void);
	virtual void OnDataChanged(DataUpdateType_t type);

private:
	Vector m_vecLightColor;
	float m_flInnerCone;
	float m_flOuterCone;
	int m_nLightType;

	DeferredLightData_t lightData;
};


IMPLEMENT_CLIENTCLASS_DT(C_Light, DT_Light, CLight)
	RecvPropVector(RECVINFO(m_vecLightColor)),
	RecvPropFloat(RECVINFO(m_flInnerCone)),
	RecvPropFloat(RECVINFO(m_flOuterCone)),
	RecvPropInt(RECVINFO(m_nLightType)),
END_RECV_TABLE()

C_Light::C_Light()
{
	lightData.valid = false;
}

void C_Light::Spawn(void)
{
	BaseClass::Spawn();
}

void C_Light::OnDataChanged(DataUpdateType_t type)
{
	BaseClass::OnDataChanged(type);

	Vector origin = GetAbsOrigin();
	Vector direction;

	AngleVectors(GetAbsAngles(), &direction);

	direction.z *= -1;

	if (m_nLightType < 2)
	{
		lightData.position = origin;
		lightData.color = m_vecLightColor;
		lightData.valid = true;
		lightData.type = m_nLightType;
		lightData.boundsCalced = false;

		if (m_nLightType == 1)
		{
			lightData.direction = direction;
			lightData.innerCone = m_flInnerCone;
			lightData.outerCone = m_flOuterCone;
		}

		GetDeferredLightManager()->m_lightData.AddToTail(&lightData);
	}

	// if (m_nLightType == 0)
	// {
	// 	DevMsg("Light pos: (%f, %f, %f), col: (%f, %f, %f)\n", origin.x, origin.y, origin.z, m_vecLightColor.x, m_vecLightColor.y, m_vecLightColor.z);
	// }
}
