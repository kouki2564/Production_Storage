#pragma once
#include "StateBase.h"
class Death :
    public StateBase
{
public:
	Death()
	{
		state = State::DEATH;
		isOnDamage = false;
		isMove = false;
		isMoveDir = false;
		isKnockBack = false;
		isAttack = false;
		isDodge = false;
		isJump = false;
	}

	~Death() {}
};

