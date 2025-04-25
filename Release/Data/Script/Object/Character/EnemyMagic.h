#pragma once
#include "EnemyBase.h"

class EnemyMagic :
    public EnemyBase
{
public:
    EnemyMagic();
    ~EnemyMagic();

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
    void MagicUpdate();
    void StanUpdate();

    std::vector<EnemyShot> m_magicShot;

    /// <summary>
    /// ノックバック
    /// </summary>
    void KnockBackUpdate();

    int m_shotNum = 0;

    std::vector<int> m_shotEfcNum;
};

