#include "Animation.h"
#include <cassert>

Animation::Animation() :
	m_modelHandle(-1),
	m_animaFrame(0),
	m_animaRate(0),
	m_animaSpeed(10),
	m_nowAttach(-1),
	m_nextAttach(-1)
{
}

Animation::~Animation()
{
}

void Animation::InitAnimation(int modelHandle, int animaNum, float speed, float frame)
{
	m_modelHandle = modelHandle;
	m_nextAttach = MV1AttachAnim(m_modelHandle, animaNum);
	if (m_nowAttach == -1)
	{
		m_nowAttach = m_nextAttach;
	}
	m_animaSpeed = speed;
	m_animaFrame = frame;
}

void Animation::SetAnimation(int animaNum, float speed)
{
	if (MV1GetAttachAnim(m_modelHandle, m_nextAttach) != animaNum)
	{
		if (m_nowAttach == m_nextAttach)
			m_nextAttach = MV1AttachAnim(m_modelHandle, animaNum);
	}
	m_animaSpeed = speed;
}

void Animation::TransAnimation()
{
	assert(m_modelHandle != -1);
	// アニメーション変更時
	if (m_nowAttach != m_nextAttach)
	{
		AnimaRate();
	}
	// アニメーション更新
	else
	{
		Animation();
	}
}

void Animation::PlayAnimation()
{
	// m_nextAttach = MV1AttachAnim(modelHandle, AnimaFlag, -1);;
	// アニメーション継続時
	if (m_nowAttach == m_nextAttach)
	{
		// 現在使用中のアニメーションの終了フレーム
		float TotalTime = MV1GetAttachAnimTotalTime(m_modelHandle, m_nowAttach);

		// アニメフレーム進行
		m_animaFrame += m_animaSpeed * 0.1f;

		// アニメフレーム更新
		MV1SetAttachAnimTime(m_modelHandle, m_nowAttach, m_animaFrame);

		// アニメーションが1周したとき
		if (m_animaFrame  > TotalTime)
		{
			m_animaFrame = 0.0f;
		}
	}
	// アニメーション遷移時
	else
	{
		AnimaRate();
	}
}

void Animation::AnimaRate()
{
	if (m_animaRate < 1.0f)
	{
		// アニメーション遷移
		m_animaRate += 0.1f;
		MV1SetAttachAnimBlendRate(m_modelHandle, m_nowAttach, 1.0f - m_animaRate);
		MV1SetAttachAnimBlendRate(m_modelHandle, m_nextAttach, m_animaRate);
		MV1DrawModel(m_modelHandle);
	}
	else
	{
		// 直前のアニメーションインデックスを消去
		MV1DetachAnim(m_modelHandle, m_nowAttach);
		// 次のインデックスを付与
		m_nowAttach = m_nextAttach;
		m_animaRate = 0;
		m_animaFrame = 0;
	}
}
