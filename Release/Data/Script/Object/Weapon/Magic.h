#pragma once
#include "WeaponBase.h"
class Magic :
    public WeaponBase
{
public:
    Magic();
    ~Magic();

    void WeaponUpdate();

    void ColUpdate(float scale, int halfTime);

    void SetShot(int& shotHandle, VECTOR shotDir, float shotSize, int shotNum);

    void SetPos(VECTOR ownerPos) { m_spherePos = ownerPos; }

    void OffIsCollider() override;

    void SetModel(VECTOR pos, VECTOR dir);

private:
    VECTOR m_spherePos;

    int m_shotNum = 0;

};

