#include "EnemyBase.h"
// State関連
#include "Move.h"
#include "Jump.h"
#include "Attack.h"
#include "Knock.h"
#include "Dodge.h"
#include "Stan.h"
#include "Death.h"

namespace
{
	constexpr float kBarWidth = 10.0f;
	constexpr float kBarHeight = 1.0f;
}

EnemyBase::EnemyBase()
{
	m_baseHandle = -1;
	m_barHandle[0] = LoadGraph("Data/Image/HP.png");
	m_barHandle[1] = LoadGraph("Data/Image/Bar.png");
}

EnemyBase::~EnemyBase()
{
	DeleteGraph(m_barHandle[0]);
	DeleteGraph(m_barHandle[1]);
}

void EnemyBase::SetEnemyBaseData(VECTOR pos, int enemyCountNum)
{
	m_name = "Enemy_" + std::to_string(enemyCountNum);
	m_pos = pos;
}

void EnemyBase::SetPosAndDir(VECTOR pos, VECTOR dir)
{
}


void EnemyBase::ChangeState(State state)
{
	auto nowState = m_state->GetState();

	if (nowState != state)
	{
		printf("\n状態の更新\n");
		if (state == State::ATTACK)
		{
			std::shared_ptr<Attack> pState = std::make_shared<Attack>();
			m_state = pState;
#ifdef _DEBUG
			printf("ATTACK\n");
#endif // _DEBUG

		}
		else if (state == State::DODGE)
		{
			std::shared_ptr<Dodge> pState = std::make_shared<Dodge>();
			m_state = pState;
#ifdef _DEBUG
			printf("DODGE\n");
#endif // _DEBUG
		}
		else if (state == State::KNOCK)
		{
			std::shared_ptr<Knock> pState = std::make_shared<Knock>();
			m_state = pState;
#ifdef _DEBUG
			printf("KNOCK\n");
#endif // _DEBUG
		}
		else if (state == State::MOVE)
		{
			std::shared_ptr<Move> pState = std::make_shared<Move>();
			m_state = pState;
#ifdef _DEBUG
			printf("MOVE\n");
#endif // _DEBUG
		}
		else if (state == State::JUMP)
		{
			std::shared_ptr<Jump> pState = std::make_shared<Jump>();
			m_state = pState;
#ifdef _DEBUG
			printf("JUMP\n");
#endif // _DEBUG
		}
		else if (state == State::STAN)
		{
			std::shared_ptr<Stan> pState = std::make_shared<Stan>();
			m_state = pState;
#ifdef _DEBUG
			printf("STAN\n");
#endif // _DEBUG
		}
		else if (state == State::DEATH)
		{
			std::shared_ptr<Death> pState = std::make_shared<Death>();
			m_state = pState;
#ifdef _DEBUG
			printf("DEATH\n");
#endif // _DEBUG
		}
	}
}

void EnemyBase::DrawHpBar(VECTOR pos)
{
	// エネミーの座標の少し上部にHPバーを表示
	auto resPos = pos;
	resPos.x -= kBarWidth / 2;
	resPos.y += 25.0f;
	resPos.z += 10.0f;

	// バー本体
	// DrawBillboard3D(resPos, 0.0f, 0.0f, kBarWidth, 0.0f, m_barHandle[1], TRUE);
	DrawModiBillboard3D(resPos, 0.0f, -kBarHeight,			// 左上
								kBarWidth, -kBarHeight,		// 右上
								kBarWidth, kBarHeight,		// 右下
								0.0f, kBarHeight,			// 左下
								m_barHandle[1], TRUE);

	// 残HPの割合に合わせた表示位置の補正
	float rate = m_parameter.GetHPRate();


	// バー中身（現在HP）
	DrawModiBillboard3D(resPos, 0.1f, -kBarHeight + 0.2f,									// 左上
								kBarWidth * m_parameter.GetHPRate() + 0.1f, -kBarHeight + 0.2f,	// 右上
								kBarWidth * m_parameter.GetHPRate() + 0.1f, kBarHeight - 0.2f,    // 右下
								0.1f, kBarHeight - 0.2f,									// 左下
								m_barHandle[0], TRUE);
}
