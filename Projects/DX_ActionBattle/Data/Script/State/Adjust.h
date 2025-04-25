#pragma once
#include "StateBase.h"
class Adjust :
    public StateBase
{
public:
	Adjust()
	{
		state = State::ADJUST;
		isOnDamage = false;
		isMove = false;
		isMoveDir = false;
		isKnockBack = false;
		isAttack = false;
		isDodge = false;
		isJump = false;
	}
	~Adjust()
	{

	}
};

