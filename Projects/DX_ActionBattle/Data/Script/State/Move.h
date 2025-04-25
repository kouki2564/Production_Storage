#pragma once
#include "StateBase.h"
class Move :
    public StateBase
{
public:
	Move()
	{
		state = State::MOVE;
		isOnDamage = true;
		isMove = true;
		isMoveDir = true;
		isKnockBack = true;
		isAttack = true;
		isDodge = true;
		isJump = true; 
	}

	~Move(){}
};

