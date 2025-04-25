#include "Mutant.h"
// State関連
#include "Move.h"
#include "Jump.h"
#include "Attack.h"
#include "Knock.h"
#include "Dodge.h"
#include "Death.h"


#include "Hand.h"
#include "ChackCollision.h"

#include "HandleManager.h"
#include "SoundManager.h"
#include "EffectManager.h"

namespace
{
	// モデルデータ情報
	constexpr float kRadius = 30.0f;

	// アイドル時間
	constexpr int kIdleTime = 200;

	// スタン時間
	constexpr int kStanTime = 250;

	// 追跡最大時間
	constexpr int kChaseTime = 500;

	// 移動速度
	constexpr float kMovePow = 0.4f;

	// 攻撃ロール最大回数
	constexpr int kAttackCountMax = 6;

	// パンチ攻撃時間
	constexpr int kPunchTime = 180;

	// ブレス攻撃時間
	constexpr int kBreathTime = 400;

	// ジャンプ前予備動作時間
	constexpr int kPreJumpTime = 240;

	// ジャンプ攻撃時間
	constexpr int kJumpAttackTime = 800;

	// 強攻撃のジャンプ回数
	constexpr int kMaxJumpCount = 3;

	// 疲れるまでの強攻撃回数
	constexpr int kToFatigueCount = 3;

	// 疲労による停止時間
	constexpr int kFatigueTime = 5000;

	constexpr int kStanCoolTime = 700;

	constexpr int kKnockTime = 25;

	constexpr int kDeathTime = 60;

	// タイトルのアニメーション時間
	constexpr int kTitleAnimationTime = 30;

	constexpr float kFrameBarWidth = 50.0f;
	constexpr float kFrameBarHeight = 3.0f;
	
	constexpr float kLifeBarWidth = 46.0f;
	constexpr float kLifeBarHeight = 1.8f;
}

enum AttackKind
{
	NONE,
	PUNCH,
	BREATH,
	PREJUMP,
	JUMP
};

Mutant::Mutant()
{
	m_barHandle[0] = HandleManager::Instance().GetImageHandle(ImageName::BARRED);
	m_barHandle[1] = HandleManager::Instance().GetImageHandle(ImageName::BARBACK);
	EffectManager::Instance().RegisterEffect("HitB", HandleManager::Instance().GetEffectHandle(EffectName::HIT), VGet(0, 0, 0), VGet(0, 0, 0));
	EffectManager::Instance().RegisterEffect("StanB", HandleManager::Instance().GetEffectHandle(EffectName::STAN), VGet(0, 0, 0), VGet(0, 0, 0));
}

Mutant::~Mutant()
{
	EffectManager::Instance().DeleteEffect("HitB");
	EffectManager::Instance().DeleteEffect("StanB");
}

void Mutant::Init(std::string name, VECTOR pos, VECTOR dir)
{

	// モデルデータ設定
	m_handle = MV1DuplicateModel(HandleManager::Instance().GetModelHandle(ModelName::MUTANT));
	m_name = name;
	m_pos = pos;
	m_dir = dir;
	auto frameIndex0 = MV1SearchFrame(m_handle, "Mesh");
	auto pos0 = VAdd(m_pos, MV1GetFramePosition(m_handle, frameIndex0));
	auto frameIndex1 = MV1SearchFrame(m_handle, "mixamorig:Head");
	auto pos1 = VGet(pos0.x, MV1GetFramePosition(m_handle, frameIndex1).y * 40.0f - kRadius * 0.5f, pos0.z);
	auto scale = VGet(40.0f, 40.0f, 40.0f);
	DrawingManager::Instance().RegisterModel(name, m_handle, m_pos, m_dir, scale);
	m_collider.SetCapsule(pos0, pos1, 1, kRadius, true);
	// マジックナンバー
	DrawingManager::Instance().CallAnimation(m_name, "Armature|Idle", 60);

	//当たり判定設定
	Collider hand;
	m_frameIndex.push_back(MV1SearchFrame(m_handle, "mixamorig:RightHand"));
	m_frameIndex.push_back(MV1SearchFrame(m_handle, "mixamorig:LeftHand"));
	hand.SetSphere(MV1GetFramePosition(m_handle, m_frameIndex[0]), -1, 25, true);
	m_hitCols.insert(std::make_pair("RightHand", hand));
	m_hitCols["RightHand"].isChackOther = false;
	pos = MV1GetFramePosition(m_handle, m_frameIndex[1]);
	hand.SetSphere(MV1GetFramePosition(m_handle, m_frameIndex[1]), -1, 25, true);
	m_hitCols.insert(std::make_pair("LeftHand", hand));
	m_hitCols["LeftHand"].isChackOther = false;

	// パラメータ設定(マジックナンバー)
	if (m_isTutorial)
	{
		m_parameter.SetPalameter(500, 1, 10, 10);
	}
	else
	{
		m_parameter.SetPalameter(1500, 40, 30, 10);
	}

	std::shared_ptr<Move> pState = std::make_shared<Move>();
	m_state = pState;
	m_waitTimeMax = kIdleTime;
	m_waitTimer = 0;
	m_isAction = false;
	m_collider.isOnDamage = true;
}

void Mutant::Update()
{
	MoveUpdate();
	AttackUpdate(); 
	KnockBackUpdate();
	StanUpdate();
	WaitTimeCount();
	DrawHpBar(m_pos);

	// 死んだあとの処理
	if (m_state->GetState() == State::DEATH)
	{
		if (m_waitTimer == m_waitTimeMax)
		{
			// 削除可能にする
			m_isDelete = true;
		}
	}
}

void Mutant::ApplyUpdating()
{
	m_dir = m_physics.GetDir();
	auto moveVec = m_physics.GetUpdateVec();
	// 座標反映
	m_pos = VAdd(m_pos, moveVec);
	m_collider.ColliderUpdate(moveVec);

	auto handle = DrawingManager::Instance().GetModelHandle(m_name);
	m_hitCols["RightHand"].pos[0] = MV1GetFramePosition(handle, m_frameIndex[0]);
	m_hitCols["LeftHand"].pos[0] = MV1GetFramePosition(handle, m_frameIndex[1]);

#ifdef _DEBUG
	if (m_collider.isOnDamage)
	{
		DrawCapsule3D(m_collider.pos[0], m_collider.pos[1], kRadius, 8, 0xff0000, 0xffffff, false);
	}
	else
	{
		DrawCapsule3D(m_collider.pos[0], m_collider.pos[1], kRadius, 8, 0xffff00, 0xffffff, false);
	}
	if (m_hitCols["LeftHand"].isChackOther)
	{
		DrawSphere3D(m_hitCols["LeftHand"].pos[0], 25, 8, 0xffff00, 0xffffff, false);
	}
	else
	{
		DrawSphere3D(m_hitCols["LeftHand"].pos[0], 25, 8, 0xff0000, 0xffffff, false);
	}
	if (m_hitCols["RightHand"].isChackOther)
	{
		DrawSphere3D(m_hitCols["RightHand"].pos[0], 25, 8, 0xffff00, 0xffffff, false);
	}
	else
	{
		DrawSphere3D(m_hitCols["RightHand"].pos[0], 25, 8, 0xff0000, 0xffffff, false);
	}

	DrawFormatString(1000, 10, 0xffff00, "ボスHP： %d", m_parameter.GetPalameter(0));
#endif // _DEBUG

	DrawingManager::Instance().UpdateModelData(m_name, m_pos, m_dir);
	// DrawingManager::Instance().UpdateWeaponModelData(m_weapon->name);

}

void Mutant::InitTitleAnimation()
{
	DrawingManager::Instance().CallAnimation(m_name, "Armature|Breath", kTitleAnimationTime);
}

void Mutant::SetChaseTarget(VECTOR targetPos)
{
	m_targetPos = targetPos;
}

bool Mutant::SetDamagePoint(float damagePoint, bool isStan, bool isPowKnock)
{
	// 防御値分の計算
	int resDamagePoint = static_cast<int>(damagePoint - (m_parameter.GetPalameter(2) * 0.1f));

	// 最低限のダメージは入れてクリア不可能対策
	if (resDamagePoint <= 0) resDamagePoint = 1;

	// 反映
	if (m_collider.isOnDamage)
	{
		m_collider.isOnDamage = false;
		m_parameter.SetDamage(resDamagePoint);
		SoundManager::Instance().OnePlay("Hit");
	}

	// スタン時ノックバックも付与
	if (m_state->GetIsKnockBack() && isPowKnock && m_knockTimer == kKnockTime)
	{
		m_knockTimer = 0;
		if (m_state->GetState() != State::STAN)
		{
			if (!m_stanLock)
			{
				m_isTakeStanFrag = isStan;
				if (isStan)
				{
					m_stanLock = true;
				}
			}
		}
		else
		{
			EffectManager::Instance().StopEffect("Stan_" + m_name);
			ChangeState(State::MOVE);
		}
	}

	// ダメージ表示
		// 10以下で拡大値1.5
	if (resDamagePoint < 10)
	{
		PanelManager::Instance().SetNumber(resDamagePoint, m_pos, 0.5f, 30, m_cameraDir);
	}
	// 10以上で拡大値2.0
	else if (resDamagePoint < 30)
	{
		PanelManager::Instance().SetNumber(resDamagePoint, m_pos, 0.8f, 30, m_cameraDir);
	}
	// 30以上で拡大値2.5
	else if (resDamagePoint < 50)
	{
		PanelManager::Instance().SetNumber(resDamagePoint, m_pos, 1.0f, 30, m_cameraDir);
	}
	// 50以上で拡大値3.0
	else if (resDamagePoint < 80)
	{
		PanelManager::Instance().SetNumber(resDamagePoint, m_pos, 1.2f, 30, m_cameraDir);
	}
	// 80以上で拡大値3.5
	else
	{
		PanelManager::Instance().SetNumber(resDamagePoint, m_pos, 1.5f, 30, m_cameraDir);
	}

	EffectManager::Instance().PlayEffect("HitB", m_collider.centerPos, m_dir);

	// 一応HPのマイナス化防止とやられた判定をここでとる
	if (m_parameter.GetPalameter(0) <= 0)
	{
		ChangeState(State::DEATH);
		DrawingManager::Instance().CallAnimation(m_name, "Armature|Die", kDeathTime);
		m_waitTimeMax = kDeathTime;
		m_waitTimer = 0;
		// 当たり判定の消去
		m_collider.isOnDamage = false;
		m_isTakeStanFrag = isStan;
	}

	return true;

#ifdef _DEBUG

	printf("\n%d のダメージ！\nボス残HP：%d\n", resDamagePoint, m_parameter.GetPalameter(0));
	if (m_parameter.GetPalameter(0) <= 0)
	{
		printf("討伐っ！\n");
	}

#endif // _DEBUG
}

bool Mutant::GetIsHitAttack(Collider& colOther)
{
	// 当たり判定処理
	if (colOther.isOnDamage)
	{
		bool isHitR = ChackCollision::Instance().GetIsCollision(m_hitCols["RightHand"], colOther);
		bool isHitL = ChackCollision::Instance().GetIsCollision(m_hitCols["LeftHand"], colOther);
		bool isHitBody = ChackCollision::Instance().GetIsCollision(m_collider, colOther);
		bool isResHit = false;

		// ダメージ計算
		m_damagePoint = 0;
		// 基礎攻撃値を設定
		float tempPoint = m_parameter.GetPalameter(1) * 1.5f;


		// 攻撃手段ごとに倍率設定して反映
		if (m_attackNum == AttackKind::PUNCH)
		{
			if (isHitL)
			{
#ifdef _DEBUG
				printf("\nHit\n");
#endif // _DEBUG
				m_damagePoint += tempPoint * 1.0f;
				m_hitCols["LeftHand"].isChackOther = false; 
				isResHit = true;
			}
			if (isHitR)
			{
#ifdef _DEBUG
				printf("\nHit\n");
#endif // _DEBUG
				m_damagePoint += tempPoint * 0.75f;
				m_hitCols["RightHand"].isChackOther = false;
				isResHit = true;
			}
		}
		else if (m_attackNum == AttackKind::BREATH)
		{

		}
		else if (m_attackNum == AttackKind::JUMP)
		{
			if (isHitBody && !m_collider.isOnDamage)
			{
#ifdef _DEBUG
				printf("\nHit\n");
#endif // _DEBUG
				m_damagePoint += tempPoint * 2.5f;
				m_hitCols["RightHand"].isChackOther = false;
				m_hitCols["LeftHand"].isChackOther = false;
				m_collider.isOnDamage = true;
				m_collider.isChackOther = true;
				isResHit = true;
			}
			else if (isHitL || isHitR) 
			{
#ifdef _DEBUG
				printf("\nHit\n");
#endif // _DEBUG
				m_damagePoint += tempPoint * 2.0f;
				m_hitCols["RightHand"].isChackOther = false;
				m_hitCols["LeftHand"].isChackOther = false;
				isResHit = true;
			}
		}

		return isResHit;
	}
	else
	{
		return false;
	}
}

void Mutant::SetPosAndDir(VECTOR pos, VECTOR dir)
{
}

void Mutant::MoveUpdate()
{
	if (m_state->GetIsMove())
	{
		if (m_waitTimer >= m_waitTimeMax)
		{
			if (m_isAction)
			{
				m_isAction = false;
				m_waitTimeMax = kIdleTime;
				m_waitTimer = 0;
			}
			else
			{
				m_isAction = true;
				m_waitTimeMax = kChaseTime;
				m_waitTimer = 0;
			}
		}
		
		if (m_state->GetState() == State::MOVE)
		{

			auto moveValue = 0.0f;
			// 移動するかどうか
			if (m_isAction)
			{
				// ターゲット方向の設定
				auto dir = VSub(m_targetPos, m_pos);
				dir.y = 0;
				dir = VNorm(dir);
				// 移動値の更新
				m_physics.Move(dir, kMovePow);
				// 移動状態
				moveValue = VSize(m_physics.GetMove());

			}

			// アニメーション
			if (moveValue != 0)
			{
				// マジックナンバー
				DrawingManager::Instance().CallAnimation(m_name, "Armature|Walk", 40 / moveValue);
			}
			else
			{
				DrawingManager::Instance().CallAnimation(m_name, "Armature|Idle", 160);
			}
		}
	}
}

void Mutant::AttackUpdate()
{
	if (m_state->GetIsAttack() && m_isAction)
	{
		// プレイヤーまでの距離
		auto toTargetVec = VSub(m_targetPos, m_pos);

		/* 攻撃選択 */
		// 攻撃ロール
		// 0：パンチ
		// 1：パンチ
		// 2：パンチ or ジャンプ攻撃
		// 3：パンチ
		// 4：パンチ
		// 5：ジャンプ攻撃

		if (m_attackCount == 2 || m_attackCount == 5)
		{
			// 体力半分以下で行動変化
			if (m_attackCount == 5 || m_parameter.GetHPRate() < 0.5f)
			{
				// ジャンプ攻撃
				if (m_attackNum == AttackKind::NONE)
				{
					ChangeState(State::ATTACK);
					m_attackNum = AttackKind::PREJUMP;

					m_waitTimeMax = kPreJumpTime;
					m_waitTimer = 0;
					m_attackCount++;
					if (m_attackCount == kAttackCountMax)
					{
						m_attackCount = 0;
					}
				}
			}
			else
			{
				// パンチ攻撃
				if (m_attackNum == AttackKind::NONE && VSize(toTargetVec) <= 50)
				{
					ChangeState(State::ATTACK);
					m_attackNum = AttackKind::PUNCH;

					m_waitTimeMax = kPunchTime;
					m_waitTimer = 0;
					m_attackCount++;
				}
			}
			
		}
		else
		{
			// パンチ攻撃
			if (m_attackNum == AttackKind::NONE && VSize(toTargetVec) <= 50)
			{
				ChangeState(State::ATTACK);
				m_attackNum = AttackKind::PUNCH;

				m_waitTimeMax = kPunchTime;
				m_waitTimer = 0;
				m_attackCount++;
			}
		}
		
	}
	// 攻撃の処理
	if (m_state->GetState() == State::ATTACK)
	{
		Punch();
		Breath();
		JumpAttack();
		

		// 攻撃終了処理
		if (m_waitTimer == 0 && (m_waitTimeMax != kPunchTime && m_waitTimeMax != kBreathTime && m_waitTimeMax != kPreJumpTime && m_waitTimeMax != kJumpAttackTime))
		{
			m_isGiveStanFrag = false;
			m_attackNum = AttackKind::NONE;
			ChangeState(State::MOVE);
			m_isAction = false;
			m_waitTimeMax = kIdleTime;
			m_jumpPos = VZero();
			m_hitCols["RightHand"].isChackOther = false;
			m_hitCols["LeftsHand"].isChackOther = false;
		}
	}
	else
	{
		// State::Attack以外の時の後処理
		/*if (m_attackCount != 0 && m_physics.GetIsGround())
		{
			m_attackCount = 0;
			m_weapon->ResetColScale();
		}*/
	}
}

void Mutant::StanUpdate()
{
	if (m_isTakeStanFrag)
	{
		// スタンフラグが立っていたら
		m_isTakeStanFrag = false;
		// そのまま移動状態に
		ChangeState(State::STAN);
		auto stanPos = VGet(m_collider.pos[1].x, m_collider.pos[1].y + 10.0f, m_collider.pos[1].z);
		EffectManager::Instance().PlayEffect("Stan_" + m_name, stanPos, m_dir);
		m_waitTimeMax = kStanTime;
		m_waitTimer = 0;
		DrawingManager::Instance().CallAnimation(m_name, "Armature|Idle", kStanTime / 10);
	}

	if (m_state->GetState() == State::STAN)
	{
		// スタンエフェクトの更新
		auto stanPos = VGet(m_collider.pos[1].x, m_collider.pos[1].y + 10.0f, m_collider.pos[1].z);
		EffectManager::Instance().UpdateEffectData("Stan_" + m_name, stanPos, m_dir);
		// スタン終了
		if (m_waitTimer == kStanTime)
		{
			EffectManager::Instance().StopEffect("Stan_" + m_name);
			// 通常の移動状態に
			ChangeState(State::MOVE);
			m_waitTimeMax = kIdleTime;
			m_waitTimer = 0;
			DrawingManager::Instance().CallAnimation(m_name, "Armature|Idle", 120);
		}
	}
}

void Mutant::KnockBackUpdate()
{
	// ノックバック終了
	if (m_knockTimer == kKnockTime)
	{
		m_collider.isOnDamage = true;
	}
	else
	{
		m_collider.isOnDamage = false;
		m_knockTimer++;
	}
}

void Mutant::ChangeState(State state)
{
	auto nowState = m_state->GetState();

	if (nowState != state)
	{
		printf("\n状態の更新\n");
		if (state == State::ATTACK)
		{
			std::shared_ptr<Attack> pState = std::make_shared<Attack>();
			m_state = pState;
			printf("ATTACK\n");
		}
		else if (state == State::DODGE)
		{
			std::shared_ptr<Dodge> pState = std::make_shared<Dodge>();
			m_state = pState;
			printf("DODGE\n");
		}
		else if (state == State::KNOCK)
		{
			std::shared_ptr<Knock> pState = std::make_shared<Knock>();
			m_state = pState;
			printf("KNOCK\n");
		}
		else if (state == State::MOVE)
		{
			std::shared_ptr<Move> pState = std::make_shared<Move>();
			m_state = pState;
			printf("MOVE\n");
		}
		else if (state == State::JUMP)
		{
			std::shared_ptr<Jump> pState = std::make_shared<Jump>();
			m_state = pState;
			printf("JUMP\n");
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

void Mutant::Punch()
{
	if (m_attackNum == AttackKind::PUNCH)
	{
		// 左パンチ判定開始
		if (m_waitTimer == static_cast<int>(kPunchTime * 0.2f))
		{
			m_hitCols["LeftHand"].isChackOther = true;
		}
		// 右パンチ開始
		if (m_waitTimer == static_cast<int>(kPunchTime * 0.6f))
		{
			m_hitCols["RightHand"].isChackOther = true;
		}
		// 左パンチ終了
		if (m_waitTimer == static_cast<int>(kPunchTime * 0.5f))
		{
			m_hitCols["LeftHand"].isChackOther = false;
		}
		// 右パンチ終了
		if (m_waitTimer == static_cast<int>(kPunchTime * 0.9f))
		{
			m_hitCols["RightHand"].isChackOther = false;
		}
		// 主にアニメーション管理
		if (m_waitTimer < static_cast<int>(kPunchTime * 0.4f))
		{
			DrawingManager::Instance().CallAnimation(m_name, "Armature|LeftPunch", kPunchTime * 0.5f + 10);
		}
		else
		{
			DrawingManager::Instance().CallTransAnimation(m_name, "Armature|RightPunch", kPunchTime * 0.5f);
		}
	}
}

void Mutant::Breath()
{
	if (m_attackNum == AttackKind::BREATH)
	{
		DrawingManager::Instance().CallAnimation(m_name, "Armature|Breath", kBreathTime);
	}
}

void Mutant::JumpAttack()
{
	if (m_attackNum == AttackKind::PREJUMP)
	{
		DrawingManager::Instance().CallAnimation(m_name, "Armature|Otakebi", kPreJumpTime);
		DrawingManager::Instance().CallTransAnimation(m_name, "Armature|JumpAttack", (kJumpAttackTime / kMaxJumpCount));
		if (m_waitTimeMax != kPreJumpTime)
		{
			if (m_waitTimer == 0)
			{
				m_isGiveStanFrag = true;
				m_attackNum = AttackKind::JUMP;
				m_waitTimeMax = kJumpAttackTime;
			}
		}
	}
	else if (m_attackNum == AttackKind::JUMP)
	{
		auto jumpAttackOneTime = kJumpAttackTime / kMaxJumpCount;
		// ジャンプ中の移動
		if (m_waitTimer == static_cast<int>(jumpAttackOneTime * 0.2f))
		{
			m_jumpPos = m_pos;
		}
		else if (m_waitTimer == static_cast<int>(jumpAttackOneTime * 1.2f))
		{
			m_jumpPos = m_targetPos;
		}
		else if (m_waitTimer == static_cast<int>(jumpAttackOneTime * 2.2f))
		{
			m_jumpPos = m_targetPos;
		}
		// 空中時の当たり判定（外すかも）
		if ((m_waitTimer > static_cast<int>(jumpAttackOneTime * 0.2f) && m_waitTimer <= static_cast<int>(jumpAttackOneTime * 0.5f)) ||
			(m_waitTimer > static_cast<int>(jumpAttackOneTime * 1.2f) && m_waitTimer <= static_cast<int>(jumpAttackOneTime * 1.5)) ||
			(m_waitTimer > static_cast<int>(jumpAttackOneTime * 2.2f) && m_waitTimer <= static_cast<int>(jumpAttackOneTime * 2.5f)))
		{
			m_physics.SetGravity(0, VGet(0, -1, 0));
			auto vec = VSub(m_jumpPos, m_pos);
			m_physics.Move(vec, 0.1f);
			m_collider.isChackOther = false;
			m_collider.isOnDamage = false;
		}
		else
		{
			m_collider.isChackOther = true;
		}
		// 拳の判定開始
		if (m_waitTimer == static_cast<int>(jumpAttackOneTime * 0.3f) ||
			m_waitTimer == static_cast<int>(jumpAttackOneTime * 1.3f) ||
			m_waitTimer == static_cast<int>(jumpAttackOneTime * 2.3f))
		{
			m_hitCols["LeftHand"].isChackOther = true;
			m_hitCols["RightHand"].isChackOther = true;
		}
		// 拳の判定終了
		if (m_waitTimer == static_cast<int>(jumpAttackOneTime * 0.6f) ||
			m_waitTimer == static_cast<int>(jumpAttackOneTime * 1.6f) ||
			m_waitTimer == static_cast<int>(jumpAttackOneTime * 2.6f))
		{
			m_collider.isOnDamage = true;
			m_hitCols["LeftHand"].isChackOther = false;
			m_hitCols["RightHand"].isChackOther = false;
		}
		DrawingManager::Instance().CallAnimation(m_name, "Armature|JumpAttack", (kJumpAttackTime / kMaxJumpCount));
	}
}


void Mutant::DrawHpBar(VECTOR pos)
{
	// エネミーの座標の少し上部にHPバーを表示
	auto resPos = pos;
	resPos.y += 80.0f;

	// バー本体
	// DrawBillboard3D(resPos, 0.0f, 0.0f, kBarWidth, 0.0f, m_barHandle[1], TRUE);
	DrawModiBillboard3D(resPos, - kFrameBarWidth / 2	, - kFrameBarHeight,	// 左上
								kFrameBarWidth / 2		, - kFrameBarHeight,	// 右上
								kFrameBarWidth / 2		, kFrameBarHeight,		// 右下
								- kFrameBarWidth / 2	, kFrameBarHeight,		// 左下
								m_barHandle[1], TRUE);

	// 残HPの割合に合わせた表示位置の補正
	float rate = m_parameter.GetHPRate();


	// バー中身（現在HP）
	DrawModiBillboard3D(resPos, - kLifeBarWidth / 2								, -kLifeBarHeight,		// 左上
								- kLifeBarWidth / 2 + (kLifeBarWidth) * rate	, -kLifeBarHeight,		// 右上
								- kLifeBarWidth / 2 + (kLifeBarWidth) * rate	, kLifeBarHeight,		// 右下
								- kLifeBarWidth / 2								, kLifeBarHeight,		// 左下
								m_barHandle[0], TRUE);
}