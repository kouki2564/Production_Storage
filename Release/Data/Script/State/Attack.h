#pragma once
#include "StateBase.h"
class Attack :
    public StateBase
{
public:
	Attack()
	{
		state = State::ATTACK;
		isOnDamage = true;
		isMove = false;
		isMoveDir = false;
		isKnockBack = true;
		isAttack = true;
		isDodge = true;
		isJump = false;
	}
	~Attack()
	{

	}
};

