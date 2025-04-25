#pragma once
#include "WeaponBase.h"
class Sword :
    public WeaponBase
{
public:
    Sword();
    ~Sword();

    void WeaponUpdate();

    void ColUpdate(float scale, int halfTime);

    void SetShot(int& shotHandle, VECTOR shotDir, float shotSize, int shotNum);

    void SetPos(VECTOR ownerPos){}

    void SetModel(VECTOR pos, VECTOR dir);

private:

};

