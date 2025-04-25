#pragma once
#include "CollisionBase.h"
class CapsuleCol :
    public CollisionBase
{
public:
    // カプセルの球同士の幅
    float height;
    // カプセル球の半径
    float radius;
};

