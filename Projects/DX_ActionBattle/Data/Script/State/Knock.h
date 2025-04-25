#pragma once
#include "StateBase.h"
class Knock :
    public StateBase
{
public:
    Knock()
    {
		state = State::KNOCK;
		isOnDamage = false;
		isMove = false;
		isMoveDir = false;
		isKnockBack = false;
		isAttack = false;
		isDodge = false;
		isJump = false;
    }

	~Knock()
	{

	}
};

