#pragma once
#include "StateBase.h"
class Stan :
    public StateBase
{
public:
	Stan()
	{
		state = State::STAN;
		isOnDamage = true;
		isMove = false;
		isMoveDir = false;
		isKnockBack = true;
		isAttack = false;
		isDodge = false;
		isJump = false;
	}
	~Stan()
	{

	}
};

