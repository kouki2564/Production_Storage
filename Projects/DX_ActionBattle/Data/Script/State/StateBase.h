#pragma once

enum class State
{
	MOVE,
	ATTACK,
	DODGE,
	ADJUST,
	KNOCK,
	JUMP,
	STAN,
	DEATH
};

class StateBase
{
public:
	StateBase(){}
	~StateBase(){}

	State GetState() { return state; }

	bool GetIsOnDamage() { return isOnDamage; }
	bool GetIsMove() { return isMove; }
	bool GetIsMoveDir() { return isMoveDir; }
	bool GetIsKnockBack() { return isKnockBack; }
	bool GetIsAttack() { return isAttack; }
	bool GetIsDodge() { return isDodge; }
	bool GetIsJump() { return isJump; }

protected:

	/* 各状態移行の許可項目 */
	// ダメージをうけるか
	bool isOnDamage = false;
	// 動けるか
	bool isMove = false;
	// 動作方向を変えられるか
	bool isMoveDir = false;
	// ノックバックを受けるか
	bool isKnockBack = false;
	// 攻撃に移れるか
	bool isAttack = false;
	// 回避できるか
	bool isDodge = false;
	// ジャンプできるか
	bool isJump = false;

	State state = State::MOVE;

};

