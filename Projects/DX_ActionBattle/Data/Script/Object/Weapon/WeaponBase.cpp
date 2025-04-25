#include "WeaponBase.h"

void WeaponBase::OffTrajectoryCollision()
{
	for (int i = 1; i < colliders.size(); i++)
	{
		colliders.erase(colliders.begin() + i);
		i--;
	}
}

void WeaponBase::OffIsCollider()
{
	for (auto& t : colliders)
	{
		t.isChackOther = false;
	}
}
