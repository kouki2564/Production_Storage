#include "CharacterBase.h"
#include "Move.h"

namespace
{
	constexpr int kStanCoolTime = 360;
}

CharacterBase::CharacterBase()
{
}

void CharacterBase::WaitTimeCount()
{
	if (m_waitTimeMax != 0 && m_waitTimer < m_waitTimeMax)
	{
		m_waitTimer++;
	}
	else if(m_state->GetState() == State::DEATH)
	{
		m_waitTimer = m_waitTimeMax;
	}
	else
	{
		m_waitTimeMax = 0;
		m_waitTimer = 0;
	}

	if (m_stanLock)
	{
		if (m_stanCoolTimer < kStanCoolTime)
		{
			m_stanCoolTimer++;
		}
		else
		{
			m_stanLock = false;
			m_stanCoolTimer = 0;
		}
	}

}
//void CharacterBase::Update()
//{
//	// 移動（パラメータ参照して引数に入れる）
//	m_physic.MoveControll(1.0f);
//
//	// 座標更新
//	m_pos = VAdd(m_pos, m_physic.GetUpdateVec());
//}

// モデルクラスに持ってく
//void CharacterBase::Rotation()
//{
//	// モデル回転
//	MV1SetRotationXYZ(m_handle, VGet(0, atan2(-m_dir.x, -m_dir.z), 0));
//}
//
//void CharacterBase::WeaponMove(int attachFrame)
//{
//	if (m_handle != -1 && m_WeaponHandle != -1)
//	{
//		VECTOR AttachSuruFrame1Position;
//		MATRIX TransMat;
//		MATRIX AttachSareruFrame10Mat;
//		MATRIX MixMatrix;
//
//		// アタッチするモデルの MV1SetMatrix の設定を無効化する
//		MV1SetMatrix(m_WeaponHandle, MGetIdent());
//
//		// アタッチするモデルのフレームの座標を取得する
//		AttachSuruFrame1Position = MV1GetFramePosition(m_WeaponHandle, 0);
//
//		// アタッチするモデルをフレームの座標を原点にするための平行移動行列を作成
//		TransMat = MGetTranslate(VScale(AttachSuruFrame1Position, -1.0f));
//
//		// アタッチされるモデルのフレームの行列を取得
//		AttachSareruFrame10Mat = MV1GetFrameLocalWorldMatrix(m_handle, attachFrame);
//
//		// アタッチするモデルをフレームの座標を原点にするための平行移動行列と、
//		// アタッチされるモデルのフレームの行列を合成
//		MixMatrix = MMult(MGetRotAxis(VGet(0, 1, 0), 1.0f * DX_PI_F),	// 角度調整
//			MMult(MGetScale(VGet(0.005f, 0.005f, 0.005f)),		// スケール値調整
//				MMult(TransMat, AttachSareruFrame10Mat)));			// 移動量設定
//		// 合成した行列をアタッチするモデルにセット
//		MV1SetMatrix(m_WeaponHandle, MixMatrix);
//	}
//}