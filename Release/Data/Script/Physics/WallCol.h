#pragma once
#include "CollisionBase.h"
class WallCol :
    public CollisionBase
{
public:
    // 中心からの各軸幅
    float width;
    float height;
    float depth;
   
};

