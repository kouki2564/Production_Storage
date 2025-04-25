#pragma once
#include "DxLib.h"

enum class CollisionForm
{
	SPHERE,
	CAPSULE,
	WALL
};

class CollisionBase
{
public:
	bool IsHitCollision(CollisionBase other);

	// CollisionForm GetCollisionForm() { return  };
	// Collisionの中心座標
	VECTOR pos;
	// 当たって通過するかどうか
	bool isCollision;
	// 当たり判定をとる優先度値（-1:貫通物　0:不変物　1:重い物　2:プレイヤー　3:軽い物）
	int prefer;
};

