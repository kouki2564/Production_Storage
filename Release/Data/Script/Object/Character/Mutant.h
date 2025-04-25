#pragma once
#include "CharacterBase.h"

class Hand;

class Mutant :
    public CharacterBase
{
public:
    Mutant();
    ~Mutant();


	/// <summary>
	/// 初期化処理
	/// </summary>
	void Init(std::string name, VECTOR pos, VECTOR dir);

	/// <summary>
	/// キャラ周り全体の更新
	/// </summary>
	void Update();


	/// <summary>
	/// 最終的な更新内容の反映
	/// </summary>
	void ApplyUpdating();

	void InitTitleAnimation();

	void SetWeapon(int num) {}

	void SetChaseTarget(VECTOR targetPos);

	bool SetDamagePoint(float damagePoint, bool isStan, bool isPowKnock);

	bool GetIsHitAttack(Collider& colOther);

	void SetPosAndDir(VECTOR pos, VECTOR dir);

	void SetUpgrade(int paramNum) {}

private:
	/// <summary>
	/// 移動
	/// </summary>
	void MoveUpdate();

	/// <summary>
	/// 攻撃
	/// </summary>
	void AttackUpdate();

	void StanUpdate();

	void KnockBackUpdate();

	/// <summary>
	/// Stateの変更（変更なしの場合は素通り）
	/// </summary>
	/// <param name="state">変更先のState</param>
	void ChangeState(State state);

	/* 各攻撃処理 */

	// パンチ
	void Punch();

	// ブレス
	void Breath();

	// ジャンプ攻撃
	void JumpAttack();

	void DrawHpBar(VECTOR pos);

	int m_barHandle[2];

	// 攻撃回数
	int m_attackCount = 0;

	// ジャンプ回数
	int m_jumpCount = 0;

	VECTOR m_jumpPos = VZero();

	bool m_isAction = false;

	int m_attackNum = 0;

	bool m_isJump = true;

	int m_knockTimer = 0;

	// 攻撃手段(プレイヤーでいうWeapon)
	std::map<std::string, Collider> m_hitCols;
	std::vector<int> m_frameIndex;
};

