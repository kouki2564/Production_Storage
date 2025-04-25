#include "Sword.h"
#include "HandleManager.h"
#include "EffectManager.h"
#include "DrawingManager.h"

namespace
{
	float kHitRange = 1.5f;
}

Sword::Sword()
{ 
	kind = Weapon::SWORD;
	name = "Sword";
	handle = MV1DuplicateModel(HandleManager::Instance().GetModelHandle(ModelName::SWORD));
	rot = VGet(0, 0, -0.6f * DX_PI_F);
	scale = VGet(40.0f, 80.0f, 80.0f);
	auto pos0 = MV1GetFramePosition(handle, 0);
	auto pos1 = MV1GetFramePosition(handle, 1);
	colliders.push_back(Collider());
	colliders[0].SetCapsule(pos0, pos1, -1, kHitRange, false);
}

Sword::~Sword()
{
	MV1DeleteModel(handle);
}

void Sword::WeaponUpdate()
{
	// 当たり判定の座標更新
	colliders[0].pos[0] = MV1GetFramePosition(handle, 0);
	colliders[0].pos[1] = MV1GetFramePosition(handle, 1);

	// 拡大先の座標の特定
	auto scaleVec = VSub(colliders[0].pos[1], colliders[0].pos[0]);
	m_fixedVec = VScale(scaleVec, m_hitScale);
	colliders[0].pos[1] = VAdd(colliders[0].pos[0], m_fixedVec);

#ifdef _DEBUG
	// カプセル範囲のデバッグ表示
	DrawCapsule3D(colliders[0].pos[0], colliders[0].pos[1], kHitRange, 32, 0xff00ff, 0xffffff, false);

#endif // _DEBUG
}

void Sword::ColUpdate(float scale, int halfTime)
{
	// 拡大時
	/*if (scale < 1)
	{
		auto oneFrameFixScale = (1 - scale) / halfTime;
		if (m_hitScale > scale)
		{
			m_hitScale -= oneFrameFixScale;
		}
	}*/
	// 縮小時
	if(scale > 1)
	{
		auto oneFrameFixScale = scale / halfTime;
		if (m_hitScale < scale)
		{
			m_hitScale += oneFrameFixScale;
		}
	}
	if (!colliders[0].isChackOther)
	{
		m_hitScale = 1.0f;
	}
}

void Sword::SetShot(int& shotHandle, VECTOR shotDir, float shotSize, int shotNum)
{
}

void Sword::SetModel(VECTOR pos, VECTOR dir)
{
	auto titleScale = VScale(scale, 0.5f);
	DrawingManager::Instance().RegisterModel(name, handle, pos, dir, titleScale);
}
