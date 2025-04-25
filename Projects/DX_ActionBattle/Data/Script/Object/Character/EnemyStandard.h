#pragma once
#include "EnemyBase.h"
class EnemyStandard :
    public EnemyBase
{
public:
    EnemyStandard();
    ~EnemyStandard();

    void Init(std::string name, VECTOR pos, VECTOR dir);

    void Update();

    void ApplyUpdating();

    void InitTitleAnimation();

    void SetWeapon(int num) {}

    void SetChaseTarget(VECTOR targetPos) { m_targetPos = targetPos; }

    bool GetIsHitAttack(Collider& colOther);

    bool SetDamagePoint(float damagePoint, bool isStan, bool isPowKnock);

    void SetUpgrade(int paramNum) {}

private:
    void MoveUpdate();
    void AttackUpdate();
    void StanUpdate();

    /// <summary>
    /// ノックバック
    /// </summary>
    void KnockBackUpdate();
};

