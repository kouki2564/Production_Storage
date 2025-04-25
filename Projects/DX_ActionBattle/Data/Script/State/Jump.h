#pragma once
#include "StateBase.h"
class Jump :
    public StateBase
{
public:
    Jump()
    {
		state = State::JUMP;
		isOnDamage = true;
		isMove = true;
		isMoveDir = true;
		isKnockBack = false;
		isAttack = true;
		isDodge = false;
		isJump = true;
    }

	~Jump(){}
};

