#include "Physics.h"
#include "WallCol.h"
#include "SphereCol.h"
#include "CapsuleCol.h"

namespace
{
	constexpr float kMaxGravity = 2.0f;
	constexpr float kAddGravity = 0.3f;
	constexpr float kSetJumpPow = 7.5f;
	constexpr float kHoverGravity = - 25.0f;
	constexpr float kWeekHoverGravity = - 15.0f;
	constexpr float kJumpReductRate = 0.9f;
	constexpr float kMinJumpForce = -5.0f;
	constexpr float kSubJumpForce = 2.0f;

}

VECTOR Physics::GetUpdateVec()
{
	float gravity;
	if (m_gravityForce < 1 )
	{
		gravity = 0;
	}
	else
	{
		gravity = m_gravityForce;
	}

	if (VSize(m_pushVec) != 0)
	{
		m_pushVec = VZero();
	}

	// マップ床による押し出しの確認
	ChackIsGround();

	// 最終的な移動ベクトルの確定
	m_responsVec = VAdd(m_responsVec, m_floorPushVec);

	m_floorPushVec = VZero();

	if (VSize(m_moveVec) != 0)
	{
		m_moveVec = VZero();
	}

	return m_responsVec;
}

VECTOR Physics::GetSemiUpdateVec()
{
	m_responsVec = VZero();

	// ノックバック量の軽減
	if (VSize(m_knockVec) > 0)
	{
		m_knockVec = VScale(m_knockVec, m_reductRate);
	}
	else if (VSize(m_knockVec) < 0.1f)
	{
		m_knockVec = VZero();
	}
	// ジャンプ量の反映
	if (!m_isGround)
	{
		m_jumpVec.y = m_jumpForce;
	}
	else
	{
		m_jumpVec = VZero();
		m_jumpForce = 0;
	}
	// 重力ベクトルの更新
	if (m_gravityForce < kMaxGravity)
	{
		m_gravityForce += kAddGravity;
	}
	float gravity;
	if (m_gravityForce < 1 || m_jumpForce != 0)
	{
		gravity = 0;
	}
	else
	{
		gravity = m_gravityForce;
	}

	if (VSize(m_pushVec) != 0 && VSize(m_moveVec) != 0)
	{
		m_moveVec.x *= 0.5f;
		m_moveVec.z *= 0.5f;
	}

	// 最終的な移動ベクトルの確定
	m_responsVec = VAdd(m_moveVec, VAdd(m_pushVec, VAdd(m_knockVec, VAdd(m_jumpVec, VScale(m_gravityDir, gravity)))));


	

	return m_responsVec;
}

void Physics::Move(VECTOR targetDir, float movePow)
{
	m_moveVec = VScale(targetDir, movePow); 
	if (m_moveVec.x != 0 || m_moveVec.z != 0)
	{
		m_dir = VNorm(m_moveVec);
		m_dir.y = 0;
	}
}

void Physics::MoveControll(VECTOR flontDir, float movePow)
{
	// 正規化した前方ベクトルをもとに
	// 右向きベクトルの作成
	auto dirRight = VGet(-flontDir.z, 0, flontDir.x);

	// コントローラのスティック入力値を獲得
	auto input = Controller::Instance().GetStick(Stick::STICK_L);
	// 入力に応じて、移動量を確定
	auto inputVec = VGet((flontDir.x * (-input.y) + dirRight.x * (-input.x)),
					0,
					(flontDir.z * (-input.y) + dirRight.z * (-input.x)));
	if (inputVec.x != 0 || inputVec.z != 0)
	{
		m_dir = VNorm(inputVec);
		m_dir.y = 0;
	}

	// 入力時はそのまま反映
	if (VSize(inputVec) != 0)
	{
		m_moveVec = VScale(inputVec, movePow);
	}
	// 未入力時は移動量減衰
	else
	{
		// if(m_moveVec)
		m_moveVec = VScale(m_moveVec, m_reductRate);
		if (VSize(m_moveVec) <= 0.1f)
		{
			m_moveVec = VZero();
		}
	}
}

void Physics::DirControll(VECTOR flontDir)
{
	// 正規化した前方ベクトルをもとに
	// 右向きベクトルの作成
	auto dirRight = VGet(-flontDir.z, 0, flontDir.x);

	// コントローラのスティック入力値を獲得
	auto input = Controller::Instance().GetStick(Stick::STICK_L);
	// 入力に応じて、移動量を確定
	auto inputVec = VGet((flontDir.x * (-input.y) + dirRight.x * (-input.x)),
					0,
					(flontDir.z * (-input.y) + dirRight.z * (-input.x)));
	if (inputVec.x != 0 || inputVec.z != 0)
	{
		m_dir = VNorm(inputVec);
		m_dir.y = 0;
	}
}


void Physics::SetPushVec(VECTOR pushVec)
{ 
	m_pushVec = VAdd(m_pushVec, pushVec);
}

void Physics::SetIsGround(bool isGround)
{
	if (isGround)
	{
		m_isGround = true;
		// 地面に触れているときはジャンプの計算をしないように
		m_isMathJump = false;
	}	
	else
	{
		m_isGround = false;
	}
}
void Physics::SetIsToGround(bool isToGround)
{
	// 次フレームで
	if (isToGround)
	{
		// m_gravityForce = 0;
		m_isToGround = true;
	}
	else
	{
		m_isGround = false;
		m_isToGround = false;
	}
}
void Physics::SetJumpVec(float height, int time, int maxTime)
{
	if (m_isMathJump)
	{
		// 2次関数曲線軌道
		int halfTime = static_cast<int>(maxTime * 0.5f);
		auto slope = -height / (halfTime * halfTime);
		// ジャンプ軌道のみの仮想座標として落とし込む
		auto jumpPosY = slope * ((time - halfTime) * (time - halfTime)) + height;
		auto lastJumpPosY = slope * (((time - 1) - halfTime) * ((time - 1) - halfTime)) + height;

		// ジャンプ初期フレーム
		if (time == 0)
		{
			m_jumpForce = 0;
		}
		// 途中フレーム
		else if(time < maxTime)
		{
			// 直前との座標差をジャンプ量として反映
			m_jumpForce = jumpPosY - lastJumpPosY;
		}
		// 最終フレーム
		else
		{
			m_jumpForce = 0;
			m_isMathJump = false;
		}
	}
	else
	{
		m_jumpForce = 0;
	}
}
void Physics::SetJumpFrag()
{
	if (m_isGround)
	{
		m_isGround = false;
		m_gravityForce = kHoverGravity;
		m_isMathJump = true;
	}
	else if (m_isAirJump)
	{
		m_isAirJump = false;
		m_gravityForce = kHoverGravity;
		m_isMathJump = true;
	}
}

void Physics::Hover(bool isLast)
{
	if (isLast)
	{
		m_gravityForce = kWeekHoverGravity;
		m_jumpVec = VZero();
		m_jumpForce = 0;
	}
	else
	{
		m_gravityForce = kHoverGravity;
		m_jumpVec = VZero();
		m_jumpForce = 0;
	}
	m_jumpVec = VZero();
}

void Physics::Dodge(float dodgePow)
{
	m_moveVec = VZero();
	m_knockVec = VScale(m_dir, dodgePow);
}

void Physics::Attack(float movePow)
{
	m_moveVec = VZero();
	m_knockVec = VScale(m_dir, movePow);
}

void Physics::ChackIsGround()
{
	auto AbsY = abs(m_floorPushVec.y);
	// 微小な値をゼロ化
	if (AbsY < 0.001f)
	{
		AbsY = 0;
	}

	// ゼロでなければ地面と接触している
	if (AbsY != 0)
	{
		m_isGround = true;
		m_isAirJump = true;
	}
	// ゼロなら離れている
	else
	{
		m_isGround = false;
	}
}

void Physics::Jump()
{
}
