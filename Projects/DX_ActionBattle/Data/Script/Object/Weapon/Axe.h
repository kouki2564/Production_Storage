#pragma once
#include "WeaponBase.h"
class Axe :
    public WeaponBase
{
public:
    Axe();
    ~Axe();

    void WeaponUpdate();

    void ColUpdate(float scale, int halfTime);

    void SetShot(int& shotHandle, VECTOR shotDir, float shotSize, int shotNum);

    void SetPos(VECTOR ownerPos){}

    void SetModel(VECTOR pos, VECTOR dir);

private:
};

