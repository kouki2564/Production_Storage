#include "Shot.h"
#include "Quaternion.h"
#include "EffectManager.h"
#include "DrawingManager.h"
#include "EffekseerForDXLib.h"

namespace
{
	constexpr float kShotSpeed = 0.09f;
	constexpr float kShotDirMovePow = 5.0f;
	constexpr float kFirstShotDirMovePow = 100.0f;
	constexpr float kHitStartTimeRate = 0.6f;
	constexpr float kShotAlpha = 128.0f;
}

void Shot::SetUp(int id, int handle, VECTOR startPos, VECTOR targetPos, int time, VECTOR baseDir, VECTOR shotDir, float shotSize)
{
	m_id = id;
	m_handle = handle;
	m_targetPos = targetPos;
	m_shotDir = shotDir;
	m_maxTime = time;
	m_pos = startPos;
	m_startPos = startPos;
	m_baseDir = baseDir;
	m_shotSize = shotSize;
	DrawingManager::Instance().RegisterOtherModel("Pshot_" + std::to_string(m_id), m_pos, ModelForm::SPHERE, 0x8080ff, 0xffffff, m_shotSize, true);
	auto name = "Pshot_" + std::to_string(m_id);
	EffectManager::Instance().MoveEffectPos(name, m_pos, m_shotDir);
	EffectManager::Instance().PlayEffect(name, m_pos, m_shotDir);
}

void Shot::Update()
{
	// 進行方向の確定
	auto straightVec = VSub(m_targetPos, m_pos);
	auto straightDir = VNorm(straightVec);
	m_shotDir = VNorm(VAdd(VScale(m_shotDir, kShotDirMovePow), straightDir));
	if (m_time > m_maxTime * kHitStartTimeRate)
	{
		m_isHitStart = true;
	}

	//Quaternion rot;
	//auto zero = VZero();
	//auto up = VGet(0, 1, 0);
	////	36度/フレーム
	//auto angle = DX_PI_F / 5;
	//// 上方向射出時
	//if (m_shotDir.y > 0.5f)
	//{
	//	// 発射方向の回転
	//	rot.SetMove(angle, up, zero);
	//}
	//// それ以外
	//else
	//{
	//	// 発射方向の回転
	//	rot.SetMove(angle, m_baseDir, zero);
	//}
	//m_shotDir = VNorm(rot.Move(zero, m_shotDir));
	//auto vec = VScale(m_shotDir, kFirstShotSpeed);

	//rot.SetMove(angle, m_baseDir, vec);
	//m_pos = rot.Move(m_startPos, m_pos);

#ifdef _DEBUG

	DrawLine3D(m_startPos, VAdd(m_startPos, VScale(m_baseDir, 1000)), 0x0000ff);
	DrawLine3D(m_pos, VAdd(m_pos, VScale(m_shotDir, 1000)), 0x0000ff);

#endif // _DEBUG


	// 弾実体の描画準備
	if (!m_isHitStart)
	{
		m_shotSize *= 0.99f;
		m_pos = VAdd(m_pos, VScale(m_shotDir, kShotSpeed * m_shotSize * m_shotSize));
	}
	else
	{
		m_shotSize *= 1.04f;
	}
	DrawingManager::Instance().UpdateModelData("Pshot_" + std::to_string(m_id), m_pos, m_shotDir, VGet(m_shotSize,m_shotSize,m_shotSize));
	// エフェクトの描画
	auto name = "Pshot_" + std::to_string(m_id);
	EffectManager::Instance().MoveEffectPos(name, m_pos, m_shotDir);


	// 生存時間管理
	if (m_time < m_maxTime)
	{
		m_time++;
	}
	else
	{
		m_isApp = false;
	}

}

void Shot::UpdateTargetPos(VECTOR targetPos)
{
	m_targetPos = targetPos;

}

void Shot::DeleteModel()
{ 
	m_isApp = false;
	DrawingManager::Instance().DeleteModel("Pshot_" + std::to_string(m_id));
	EffectManager::Instance().DeleteEffect("Pshot_" + std::to_string(m_id));
}
