#pragma once
#include "CharacterBase.h"

class Controller;

enum class Weapon;

class Player :
    public CharacterBase
{
public:
    Player();
    ~Player();
    void Init(std::string name, VECTOR pos, VECTOR dir);
    void Update();
    void ApplyUpdating();

    void InitTitleAnimation();

    void SetWeapon(int num);

    void SetChaseTarget(VECTOR targetPos);
    void OnDamage(int damage);

    void SetKnockBack(float knockBack);

    bool SetDamagePoint(float damagePoint, bool isStan, bool isPowKnock);

    bool GetIsHitAttack(Collider& colOther);

    void SetPosAndDir(VECTOR pos, VECTOR dir);

    /// <summary>
    /// パラメータ強化の反映
    /// </summary>
    /// <param name="paramNum">0:HP　1:Atk　2:Def　3:Agi</param>
    void SetUpgrade(int paramNum);

private:

    /// <summary>
    /// Stateの変更（変更なしの場合は素通り）
    /// </summary>
    /// <param name="state">変更先のState</param>
    void ChangeState(State state);

    /// <summary>
    /// 所持武器変更（初期は武器無し）
    /// </summary>
    /// <param name="weaponKind"></param>
    void ChangeWeapon(Weapon weaponKind);

    /// <summary>
    /// 移動
    /// </summary>
    void MoveUpdate();

    /// <summary>
    /// ジャンプ
    /// </summary>
    void JumpUpdate();

    /// <summary>
    /// 攻撃
    /// </summary>
    void AttackUpdate();

    /// <summary>
    /// ノックバック
    /// </summary>
    void KnockBackUpdate();

    /// <summary>
    /// 回避
    /// </summary>
    void DodgeUpdate();

	/// <summary>
	/// スタン
	/// </summary>
    void StanUpdate();

    /// <summary>
    /// 移動の反映
    /// </summary>
    void ApplyMoving();


    /// <summary>
    /// ボタンの先行入力
    /// </summary>
    void PrecedInputButton();

    int m_handle;

    bool m_isTarget = false;

    Button m_inputButton = Button::NONE;

    // 攻撃回数コンボ用
    int m_attackCount = 0;
    // 最大コンボ数
    int m_attackCountMax = 0;
    
    int m_jumpTime = 0;

	bool m_isAirAttack = true;

	int startPos = DOWN;

    VECTOR m_lastWeaponPos[2] = { VZero() };

	// 空中攻撃時の強化フラグ
	bool m_isSkyBaff = false;

    // ジャンプ時のｙ座標
	float m_jumpPosY = 0;
};

