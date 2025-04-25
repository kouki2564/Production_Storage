#pragma once
#include "ObjectBase.h"
#include "Physics.h"
#include "StateBase.h"

class WeaponBase;


class CharacterBase :
	public ObjectBase
{
public:
	CharacterBase();
	virtual ~CharacterBase() {}

	virtual void Init(std::string name, VECTOR pos, VECTOR dir) = 0;

	virtual void Update() = 0;

	virtual void ApplyUpdating() = 0;

	// タイトル画面で動かすアニメーションの準備
	virtual void InitTitleAnimation() = 0;

	virtual void SetWeapon(int num) = 0;

	virtual void SetChaseTarget(VECTOR targetPos) = 0;

	virtual bool GetIsHitAttack(Collider& colOther) = 0;

	virtual bool SetDamagePoint(float damagePoint, bool isStan, bool isPowKnock) = 0;

	virtual void SetPosAndDir(VECTOR pos, VECTOR dir) = 0;

	virtual void SetUpgrade(int paramNum) = 0;


	/// <summary>
	/// 移動量の反映
	/// </summary>
	void ReflectMoving() { m_pos = VAdd(m_pos, m_physics.GetUpdateVec()); }

protected:

	/// <summary>
	/// 入力待機時間のカウント
	/// </summary>
	void WaitTimeCount();

	/* 実装予定クラス,変数*/

	// 物理動作クラスはObjectBaseに実装

	// モデル管理、描画クラス

	// Stateクラス
	std::shared_ptr<StateBase> m_state;

	// SE,エフェクトはstaticクラス使う予定（どこで使うかは未定）
	// ダメージ, パラメータ関連は別のManagerクラスを作るか...?


	/* 実装しなさそう、どっかでまとめそうなクラス,変数 */

	// アニメーションクラス（多分モデル管理に持ってく）
	// Animation m_animation;

	// キャラクターが手に持つ武器関連（モデル管理に持ってく）
	int m_weaponHandle;
	int m_weaponAttachFrameNum;

	// ダメージフラグ
	bool m_isDealDamage;

	// 待機時間
	int m_waitTimer = 0;
	// 待機時間最大値
	int m_waitTimeMax = 0;

	bool m_stanLock = false;
	int m_stanCoolTimer = 0;

	// ターゲット座標
	VECTOR m_targetPos = VZero();

	// 武器
	std::shared_ptr<WeaponBase> m_weapon;
};

