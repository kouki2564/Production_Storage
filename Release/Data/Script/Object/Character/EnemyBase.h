#pragma once
#include "CharacterBase.h"

enum class EnemyKind
{
    STANDARD,
    TANK,
    MAGIC,
	FLY
};

// 弾飛ばす敵用
struct EnemyShot
{
    int baseID;
    int shotID;
    Collider shotCol;
    int shotCount;
    VECTOR shotDir;
};

class EnemyBase :
    public CharacterBase
{
public:
    EnemyBase();
    ~EnemyBase();

    virtual void Init(std::string name, VECTOR pos, VECTOR dir) = 0;

    virtual void Update() = 0;

    virtual void ApplyUpdating() = 0;

    virtual void SetWeapon(int num) = 0;

    virtual void SetChaseTarget(VECTOR targetPos) = 0;

    virtual bool GetIsHitAttack(Collider& colOther) = 0;

    virtual bool SetDamagePoint(float damagePoint, bool isStan, bool isPowKnock) = 0;

    void SetEnemyBaseData(VECTOR pos, int enemyCountNum);

    void SetPosAndDir(VECTOR pos, VECTOR dir);

    virtual void SetUpgrade(int paramNum) = 0;

protected:
    /// <summary>
    /// 移動
    /// </summary>
    virtual void MoveUpdate() = 0;

    /// <summary>
    /// 攻撃
    /// </summary>
    virtual void AttackUpdate() = 0;

    /// <summary>
    /// Stateの変更（変更なしの場合は素通り）
    /// </summary>
    /// <param name="state">変更先のState</param>
    void ChangeState(State state);

    void DrawHpBar(VECTOR pos);

    int m_barHandle[2];

    int m_baseHandle = -1;

    float m_targetRange = 0;

    float m_AttackRange = 0;

    int m_frameIndex = 0;
    Collider m_hitCol;
};

