#include "EnemyTank.h"
#include "ChackCollision.h"

// State関連
#include "Move.h"
#include "Jump.h"
#include "Attack.h"
#include "Knock.h"
#include "Dodge.h"
#include "Death.h"

#include "HandleManager.h"
#include "SoundManager.h"
#include "EffectManager.h"

namespace
{
	// モデルデータ情報
	constexpr float kRadius = 10.0f;

	constexpr float kScale = 15.0f;

	// アイドル時間
	constexpr int kIdleTime = 250;

	// スタン時間
	constexpr int kStanTime = 250;

	// ノックバック時間
	constexpr int kKnockTime = 25;
	constexpr float kKnockPow = 0.0f;

	// 移動速度
	constexpr float kMovePow = 0.4f;

	// パンチ攻撃時間
	constexpr int kPunchTime = 250;

	// 追跡開始距離
	constexpr float kTargetRange = 100.0f;

	// 攻撃開始距離
	constexpr float kAttackRange = 20.0f;

	// タイトルのアニメーション時間
	constexpr int kTitleAnimationTime = 60;

	// 各ステータス（変動なしのためconstexpr）
	// HP
	constexpr int kMaxHP = 200;
	// 攻撃
	constexpr int kAtk = 10;
	// 防御
	constexpr int kDef = 10;
	// 素早さ
	constexpr int kAgi = 10;

	constexpr int kDeathTime = 60;

	constexpr int kStanCoolTime = 180;

}

EnemyTank::EnemyTank()
{
}

EnemyTank::~EnemyTank()
{
	EffectManager::Instance().DeleteEffect("Hit_" + m_name);
	EffectManager::Instance().DeleteEffect("Stan_" + m_name);
	EffectManager::Instance().DeleteEffect("Pos_" + m_name);
	MV1DeleteModel(m_handle);
	m_attackCol.clear();
}

void EnemyTank::Init(std::string name, VECTOR pos, VECTOR dir)
{
	m_handle = MV1DuplicateModel(HandleManager::Instance().GetModelHandle(ModelName::ENEMY2));
	m_name = name;
	m_pos = pos;
	m_dir = dir;
	auto frameIndex0 = MV1SearchFrame(m_handle, "Armature");
	auto pos0 = VAdd(m_pos, MV1GetFramePosition(m_handle, frameIndex0));
	auto frameIndex1 = MV1SearchFrame(m_handle, "mixamorig:Head");
	// マジックナンバー
	auto pos1 = VGet(pos0.x, MV1GetFramePosition(m_handle, frameIndex1).y * kScale - kRadius * 0.5f, pos0.z);
	// マジックナンバー
	auto scale = VGet(kScale, kScale, kScale);
	DrawingManager::Instance().RegisterModel(m_name, m_handle, m_pos, m_dir, scale);
	m_collider.SetCapsule(pos0, pos1, 3, kRadius, true);
	// マジックナンバー
	DrawingManager::Instance().CallAnimation(m_name, "Armature|Idle", 120);

	EffectManager::Instance().RegisterEffect("Hit_" + m_name, HandleManager::Instance().GetEffectHandle(EffectName::HIT), m_pos, m_dir);
	EffectManager::Instance().RegisterEffect("Stan_" + m_name, HandleManager::Instance().GetEffectHandle(EffectName::STAN), m_pos, m_dir);
	auto effectPos = VGet(m_pos.x, 0.0f, m_pos.z);
	EffectManager::Instance().RegisterEffect("Pos_" + m_name, HandleManager::Instance().GetEffectHandle(EffectName::ENEMYPOS), effectPos, m_dir);
	EffectManager::Instance().PlayEffect("Pos_" + m_name, effectPos, m_dir);

	//当たり判定設定
	m_frameIndex = MV1SearchFrame(m_handle, "mixamorig:RightHand");
	// マジックナンバー
	m_hitCol.SetSphere(MV1GetFramePosition(m_handle, m_frameIndex), -1, 8, true);
	m_hitCol.isChackOther = false;


	// パラメータ設定
	m_parameter.SetPalameter(kMaxHP, kAtk, kDef, kAgi);
	m_collider.isOnDamage = true;


	std::shared_ptr<Move> pState = std::make_shared<Move>();
	m_state = pState;
	m_waitTimeMax = kIdleTime;
	m_waitTimer = 0;
	m_collider.isOnDamage = true;
}

void EnemyTank::Update()
{
	MoveUpdate();
	AttackUpdate();
	KnockUpdate();
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
			EffectManager::Instance().StopEffect("Stan_" + m_name);
			EffectManager::Instance().StopEffect("Pos_" + m_name);
		}
	}
}

void EnemyTank::ApplyUpdating()
{
	m_dir = m_physics.GetDir();
	auto moveVec = m_physics.GetUpdateVec();
	// 座標反映
	m_pos = VAdd(m_pos, moveVec);
	m_collider.ColliderUpdate(moveVec);

	auto handle = DrawingManager::Instance().GetModelHandle(m_name);
	m_hitCol.pos[0] = MV1GetFramePosition(handle, m_frameIndex);

	auto effectPos = VGet(m_pos.x, 0.0f, m_pos.z);
	EffectManager::Instance().UpdateEffectData("Pos_" + m_name, effectPos, m_dir);

#ifdef _DEBUG
	DrawCapsule3D(m_collider.pos[0], m_collider.pos[1], kRadius, 8, 0xff0000, 0xffffff, false);
	if (m_hitCol.isChackOther)
	{
		DrawSphere3D(m_hitCol.pos[0], 8, 8, 0xffff00, 0xffffff, false);
	}
	else
	{
		DrawSphere3D(m_hitCol.pos[0], 8, 8, 0xff0000, 0xffffff, false);
	}

#endif // _DEBUG

	DrawingManager::Instance().UpdateModelData(m_name, m_pos, m_dir);

}

void EnemyTank::InitTitleAnimation()
{
	DrawingManager::Instance().CallAnimation(m_name, "Armature|Idle", kTitleAnimationTime);
}

bool EnemyTank::GetIsHitAttack(Collider& colOther)
{
	// 当たり判定処理
	if (colOther.isOnDamage)
	{
		bool isHit = ChackCollision::Instance().GetIsCollision(m_hitCol, colOther);

		// ダメージ計算
		m_damagePoint = 0;
		// 基礎攻撃値を設定
		float tempPoint = m_parameter.GetPalameter(1) * 1.5f;

		if (isHit)
		{
#ifdef _DEBUG
			printf("\nHit\n");
#endif // _DEBUG
			m_damagePoint += tempPoint * 2.0f;
			m_hitCol.isChackOther = false;
		}

		return isHit;
	}
	else
	{
		return false;
	}
}

bool EnemyTank::SetDamagePoint(float damagePoint, bool isStan, bool isPowKnock)
{
	// 防御値分の計算
	int resDamagePoint = static_cast<int>(damagePoint - (m_parameter.GetPalameter(2) * 0.1f));

	// 最低限のダメージは入れてクリア不可能対策
	if (resDamagePoint <= 0) resDamagePoint = 1;

	// 反映
	m_parameter.SetDamage(resDamagePoint);
	SoundManager::Instance().OnePlay("Hit");

	// スタン時ノックバックも付与
	if (m_state->GetIsKnockBack() && isPowKnock)
	{
		EffectManager::Instance().StopEffect("Stan_" + m_name);
		ChangeState(State::KNOCK);
		m_waitTimeMax = kKnockTime;
		m_waitTimer = 0;
		if (m_state->GetState() != State::STAN && !m_stanLock)
		{
			m_isTakeStanFrag = isStan;
			if (isStan)
			{
				m_stanLock = true;
			}
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

	EffectManager::Instance().PlayEffect("Hit_" + m_name, m_collider.centerPos, m_dir);
	
	// 一応HPのマイナス化防止とやられた判定をここでとる
	if (m_parameter.GetPalameter(0) <= 0)
	{
		ChangeState(State::DEATH);
		DrawingManager::Instance().CallAnimation(m_name, "Armature|Die", kDeathTime);
		m_waitTimeMax = kDeathTime;
		m_waitTimer = 0;
		// 当たり判定の消去
		m_collider.isOnDamage = false;
		// 死んだあとには攻撃判定を消す
		m_hitCol.isChackOther = false;
	}

	return true;

#ifdef _DEBUG

	printf("\n%d のダメージ！\n郷田 重雄  残HP：%d\n", resDamagePoint, m_parameter.GetPalameter(0));
	if (m_parameter.GetPalameter(0) <= 0)
	{
		printf("討伐っ！\n");
	}

#endif // _DEBUG
}

void EnemyTank::MoveUpdate()
{
	if (m_state->GetState() == State::MOVE)
	{
		auto vec = VSub(m_targetPos, m_pos);
		vec.y = 0;
		if (VSize(vec) > kTargetRange)
		{
			DrawingManager::Instance().CallAnimation(m_name, "Armature|Idle", 120);
		}
		else if (VSize(vec) > kAttackRange)
		{
			DrawingManager::Instance().CallAnimation(m_name, "Armature|Walk", 120);
			m_physics.Move(VNorm(vec), kMovePow);
		}
		else
		{
			if (m_state->GetIsAttack())
			{
				m_physics.SetDir(VNorm(vec));
				ChangeState(State::ATTACK);
				m_waitTimeMax = kPunchTime;
				m_waitTimer = 0;
			}
		}
	}
}

void EnemyTank::AttackUpdate()
{
	if (m_state->GetState() == State::ATTACK)
	{
		DrawingManager::Instance().CallAnimation(m_name, "Armature|Punch", kPunchTime);

		if (m_waitTimer == static_cast<int>(m_waitTimeMax * 0.3f))
		{
			m_hitCol.isChackOther = true;
		}
		else if (m_waitTimer == static_cast<int>(m_waitTimeMax * 0.4f))
		{
			m_hitCol.isChackOther = false;
		}
		// 攻撃終了
		if (m_waitTimer == m_waitTimeMax)
		{
			ChangeState(State::MOVE);
		}
	}
}

void EnemyTank::KnockUpdate()
{
	if (m_state->GetState() == State::KNOCK)
	{
		DrawingManager::Instance().CallAnimation(m_name, "Armature|Idle", kKnockTime);
		m_collider.isOnDamage = false;
		auto vec = VScale(VSub(VZero(), m_dir), kKnockPow);
		m_physics.SetKnockVec(vec);
		// ノックバック終了
		if (m_waitTimer == kKnockTime)
		{
			m_collider.isOnDamage = true;
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
			else
			{
				// 通常の移動状態に
				ChangeState(State::MOVE);
				m_waitTimeMax = kIdleTime;
				m_waitTimer = 0;
				DrawingManager::Instance().CallAnimation(m_name, "Armature|Idle", 120);
			}
		}
	}
}

void EnemyTank::StanUpdate()
{
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
