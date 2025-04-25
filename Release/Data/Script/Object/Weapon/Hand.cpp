#include "Hand.h"

Hand::Hand()
{
	kind = Weapon::NONE;
}

Hand::~Hand()
{
}

void Hand::WeaponUpdate()
{
}

void Hand::ColUpdate(float scale, int time)
{
}

void Hand::SetShot(int& shotHandle, VECTOR shotDir, float shotSize, int shotNum)
{
}
