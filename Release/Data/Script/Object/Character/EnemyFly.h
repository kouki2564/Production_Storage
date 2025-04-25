#pragma once
#include "EnemyBase.h"

class EnemyFly :
    public EnemyBase
{
public:
    EnemyFly();
    ~EnemyFly();

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
    void KnockBackUpdate();
	void StanUpdate();

    std::vector<EnemyShot> m_magicShot;

    std::vector<int> m_shotEfcNum;

    // こやつだけモデルと座標がずれるため
	float m_modelHeight = 0.0f;

    // 打ち出す瞬間に決定される攻撃座標
	VECTOR m_determinedTargetPos = VZero();

	// 浮き上がる範囲
    float m_floatingRange = 0.0f;
	bool m_isFloating = false;

};

