#include "aimbot.h"

// Default aimbot settings
bool Settings::Aimbot::enabled = false;

double hyp;

void CalculateAngle (Vector& src, Vector& dst, QAngle& angles)
{
	// Angle deltas
	double delta[3] = { (src[0]-dst[0]), (src[1]-dst[1]), (src[2]-dst[2]) };

	// Hypotenuse
	hyp = sqrt(delta[0] * delta[0] + delta[1] * delta[1]);

	angles[0] = (float) (atan(delta[2]/hyp) * 57.295779513082f);
	angles[1] = (float) (atan(delta[1]/delta[0]) * 57.295779513082f);
	angles[2] = 0.0f;	// <=== NEVER EVER  EVER EVER EVER CHANGE THIS VALUE
						//					... unless you teh VAC ban ...

	if (delta[0] >= 0.0) angles[1] += 180.0f;

	// Safeguards
	if (angles[1] > 180) angles[1] -= 360;
	if (angles[1] < -180) angles[1] += 360;
	if (angles[0] > 89) angles[0] = 89;
	if (angles[0] < -89) angles[0] = -89;
}

CBaseEntity* GetClosestEnemy ()
{
	C_BasePlayer* localPlayer = (C_BasePlayer*)entitylist->GetClientEntity(engine->GetLocalPlayer());
	CBaseEntity* closestEntity = NULL;
	float dist = 10000000.0f;

	if (!localPlayer)
		return NULL;

	for (int i = 0; i < 64; ++i)
	{
		CBaseEntity* entity = entitylist->GetClientEntity(i);

		if (!entity || entity == (CBaseEntity*)localPlayer ||
				entity->GetDormant() || entity->GetLifeState() != LIFE_ALIVE || entity->GetHealth() <= 0)
			continue;

		C_BasePlayer* localplayer = reinterpret_cast<C_BasePlayer*>(entitylist->GetClientEntity(engine->GetLocalPlayer()));

		if
		(
			   !entity
			|| entity == (CBaseEntity*)localPlayer
			|| entity->m_bDormant
			|| entity->GetLifeState() != 0
			|| entity->GetHealth() <= 0
			|| entity->GetTeam() == localPlayer->GetTeam()
		)
			continue;

		float e_dist = localplayer->m_vecOrigin.DistToSqr (entity->m_vecOrigin);

		if (e_dist < dist)
		{
			closestEntity = entity;
			dist = e_dist;
		}
	}

	return closestEntity;
}


void Aimbot::Calculate ()
{
	CBaseEntity* entity = GetClosestEnemy ();

	if (entity == NULL)
		return;

	C_BasePlayer* localplayer = reinterpret_cast<C_BasePlayer*>(entitylist->GetClientEntity(engine->GetLocalPlayer()));

	Vector e_vecHead = entity->m_vecOrigin + entity->m_vecViewOffset;
	Vector p_vecHead = localplayer->m_vecOrigin + localplayer->m_vecViewOffset;

	QAngle angle;
	CalculateAngle (p_vecHead, e_vecHead, angle);

	engine->SetViewAngles (angle);
}