#pragma once
#include "Collider.h"
#include <string>
#include <vector>

enum class Weapon
{
	NONE,
	SWORD,
	AXE,
	MAGIC
};

class Shot;

class WeaponBase
{
public:
	WeaponBase(){}
	virtual ~WeaponBase(){}

	virtual void WeaponUpdate() = 0;

	/// <summary>
	/// 当たり判定の拡大・縮小
	/// </summary>
	/// <param name="scale">拡大値</param>
	/// <param name="halfTime">拡大時間</param>
	virtual void ColUpdate(float scale, int halfTime) = 0;

	void ResetColScale() { m_hitScale = 1; }

	// 武器の座標登録（ほぼ魔法用）
	virtual void SetPos(VECTOR ownerPos) = 0;

	/// <summary>
	/// 弾のセット
	/// </summary>
	/// <param name="targetPos">弾のターゲット座標</param>
	/// <param name="shotNum">打ち出す弾の数</param>
	virtual void SetShot(int& shotHandle, VECTOR shotDir, float shotSize, int shotNum) = 0;

	// ターゲット座標の登録
	void SetTargetPos(VECTOR targetPos) { m_targetPos = targetPos; }

	// 軌道の当たり判定の解除
	virtual void OffTrajectoryCollision();

	virtual void OffIsCollider();

	// 所持ではなく独立でモデルを表示させる用
	virtual void SetModel(VECTOR pos, VECTOR dir) = 0;

	int handle = -1;
	VECTOR rot = VZero();
	VECTOR scale = VZero();

	// 当たり判定
	std::vector<Collider> colliders;
	// Collider collider;

	Weapon kind = Weapon::NONE;
	std::string name = "NONE";

	bool isAttack = false;

protected:
	// 当たり判定の拡大先の座標
	VECTOR m_fixedVec = VZero();
	// 当たり判定の拡大値
	float m_hitScale = 1;

	VECTOR m_targetPos = VZero();

	// 弾クラス
	std::vector<Shot*> m_shots;
};

