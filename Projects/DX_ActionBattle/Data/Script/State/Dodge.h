#pragma once
#include "StateBase.h"
class Dodge :
    public StateBase
{
public:
	Dodge()
	{
		state = State::DODGE;
		isOnDamage = false;
		isMove = false;
		isMoveDir = false;
		isKnockBack = false;
		isAttack = false;
		isDodge = false;
		isJump = false;
	}
	~Dodge()
	{

	}
};

