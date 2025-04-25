#include "EnemyMagic.h"
#include "Quaternion.h"
#include "ChackCollision.h"

#include "HandleManager.h"
#include "SoundManager.h"
#include "EffectManager.h"

// State関連
#include "Move.h"
#include "Jump.h"
#include "Attack.h"
#include "Knock.h"
#include "Dodge.h"
#include "Death.h"

namespace
{
	// モデルデータ情報
	constexpr float kRadius = 10.0f;

	constexpr float kScale = 15.0f;

	// アイドル時間
	constexpr int kIdleTime = 250;

	// スタン時間
	constexpr int kStanTime = 250;

	// 移動速度
	constexpr float kMovePow = 0.4f;

	constexpr float kShotRadius = 3.0f;

	// 魔法弾数
	constexpr int kShotNum = 3;

	constexpr int kShotMaxNum = 15;

	// 魔法速度
	constexpr float kShotSpeed = 0.3f;

	// 魔法残存時間
	constexpr int kShotTime = 240;

	// 魔法攻撃時間
	constexpr int kSpellingTime = 80;

	// 逃走開始距離
	constexpr float kEscapeRange = 50.0f;

	// 攻撃開始距離
	constexpr float kAttackRange = 100.0f;

	// タイトルのアニメーション時間
	constexpr int kTitleAnimationTime = 100;

	// 各ステータス（変動なしのためconstexpr）
	// HP
	constexpr int kMaxHP = 100;
	// 攻撃
	constexpr int kAtk = 10;
	// 防御
	constexpr int kDef = 10;
	// 素早さ
	constexpr int kAgi = 10;

	constexpr int kKnockTime = 25;
	constexpr float kKnockPow = 0.1f;

	constexpr int kStanCoolTime = 180;

	constexpr int kDeathTime = 60;

	constexpr int kDamagePopTime = 30;
}

EnemyMagic::EnemyMagic()
{
}

EnemyMagic::~EnemyMagic()
{
	EffectManager::Instance().DeleteEffect("Hit_" + m_name);
	EffectManager::Instance().DeleteEffect("Stan_" + m_name);
	EffectManager::Instance().DeleteEffect("Pos_" + m_name);
	for (int i = 0; i < kShotMaxNum; i++)
	{
		EffectManager::Instance().DeleteEffect("ShotM" + m_name + "_" + std::to_string(i));
	}

	MV1DeleteModel(m_handle);

	for (int i = 0; i < m_magicShot.size(); i++)
	{
		DrawingManager::Instance().DeleteModel("ShotM" + m_name + "_" + std::to_string(m_magicShot[i].shotID));
		EffectManager::Instance().StopEffect("ShotM" + m_name + "_" + std::to_string(m_magicShot[i].shotID));
	}
	
	m_magicShot.clear();
}

void EnemyMagic::Init(std::string name, VECTOR pos, VECTOR dir)
{
	m_handle = MV1DuplicateModel(HandleManager::Instance().GetModelHandle(ModelName::ENEMY3));
	m_name = name;
	m_dir = dir;
	m_pos = pos;
	auto frameIndex0 = MV1SearchFrame(m_handle, "Armature");
	auto pos0 = VAdd(m_pos, MV1GetFramePosition(m_handle, frameIndex0));
	auto frameIndex1 = MV1SearchFrame(m_handle, "mixamorig10:Head");
	// マジックナンバー
	auto pos1 = VGet(pos0.x, MV1GetFramePosition(m_handle, frameIndex1).y * kScale - kRadius * 0.5f, pos0.z);
	// マジックナンバー
	auto scale = VGet(kScale, kScale, kScale);
	DrawingManager::Instance().RegisterModel(m_name, m_handle, m_pos, m_dir, scale);
	m_collider.SetCapsule(pos0, pos1, 2, kRadius, true);
	// マジックナンバー
	DrawingManager::Instance().CallAnimation(m_name, "Armature|Idle", 90);

	EffectManager::Instance().RegisterEffect("Hit_" + m_name, HandleManager::Instance().GetEffectHandle(EffectName::HIT), m_pos, m_dir);
	EffectManager::Instance().RegisterEffect("Stan_" + m_name, HandleManager::Instance().GetEffectHandle(EffectName::STAN), m_pos, m_dir);
	auto effectPos = VGet(m_pos.x, 0.0f, m_pos.z);
	EffectManager::Instance().RegisterEffect("Pos_" + m_name, HandleManager::Instance().GetEffectHandle(EffectName::ENEMYPOS), effectPos, m_dir);
	EffectManager::Instance().PlayEffect("Pos_" + m_name, effectPos, m_dir);

	for (int i = 0; i < kShotMaxNum; i++)
	{
		EffectManager::Instance().RegisterEffect("ShotM" + m_name + "_" + std::to_string(i), HandleManager::Instance().GetEffectHandle(EffectName::ENEMYSHOT), VGet(0, 0, 0), VGet(0, 0, 0));
	}

	//当たり判定設定
	m_frameIndex = MV1SearchFrame(m_handle, "mixamorig10:RightHand");
	auto poss = MV1GetFramePosition(m_handle, m_frameIndex);
	// マジックナンバー
	m_hitCol.SetSphere(pos, -1, 0, true);
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

void EnemyMagic::Update()
{
	MoveUpdate();
	AttackUpdate();
	MagicUpdate();
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
			EffectManager::Instance().StopEffect("Stan_" + m_name);
			EffectManager::Instance().StopEffect("Pos_" + m_name);
		}
	}
}

void EnemyMagic::ApplyUpdating()
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
	if (Controller::Instance().GetInputDown(Button::STICK_R))
	{
		printf("\nEnemy動作状況表示\n");
		auto temp = m_physics.GetMove();
		printf("move  : %f, %f, %f\n", temp.x, temp.y, temp.z);
		temp = m_physics.GetPush();
		printf("push  : %f, %f, %f\n", temp.x, temp.y, temp.z);
		temp = m_physics.GetKnock();
		printf("knock : %f, %f, %f\n", temp.x, temp.y, temp.z);
	}

#endif // _DEBUG

	DrawingManager::Instance().UpdateModelData(m_name, m_pos, m_dir);
}

void EnemyMagic::InitTitleAnimation()
{
	DrawingManager::Instance().CallAnimation(m_name, "Armature|Idle", kTitleAnimationTime);
}

bool EnemyMagic::GetIsHitAttack(Collider& colOther)
{
	// 当たり判定処理
	if (colOther.isOnDamage)
	{
		bool isHit = false;
		// ダメージ計算
		m_damagePoint = 0;
		// 基礎攻撃値を設定
		float tempPoint = m_parameter.GetPalameter(1) * 1.5f;

		for (int i = 0; i < m_magicShot.size(); i++)
		{
			if (ChackCollision::Instance().GetIsCollision(m_magicShot[i].shotCol, colOther))
			{
				isHit = true;
#ifdef _DEBUG
				printf("\nHit\n");
#endif // _DEBUG
				m_damagePoint += tempPoint * 1.0f;
				m_hitCol.isChackOther = false;

				// 消滅させる
				DrawingManager::Instance().DeleteModel("ShotM" + m_name + "_" + std::to_string(m_magicShot[i].shotID));
				EffectManager::Instance().StopEffect("ShotM" + m_name + "_" + std::to_string(m_magicShot[i].shotID));
				m_magicShot.erase(m_magicShot.begin() + i);
				i--;
			}
		}

		return isHit;
	}
	else
	{
		return false;
	}
}

bool EnemyMagic::SetDamagePoint(float damagePoint, bool isStan, bool isPowKnock)
{
	// 防御値分の計算
	int resDamagePoint = static_cast<int>(damagePoint - (m_parameter.GetPalameter(2) * 0.1f));

	// 最低限のダメージは入れてクリア不可能対策
	if (resDamagePoint <= 0) resDamagePoint = 1;
	else
	{
		if (m_state->GetIsKnockBack())
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
	}
	SoundManager::Instance().OnePlay("Hit");

	// ダメージ表示
		// 10以下で拡大値1.5
	if (resDamagePoint < 10)
	{
		PanelManager::Instance().SetNumber(resDamagePoint, m_pos, 0.5f, kDamagePopTime, m_cameraDir);
	}
	// 10以上で拡大値2.0
	else if (resDamagePoint < 30)
	{
		PanelManager::Instance().SetNumber(resDamagePoint, m_pos, 0.8f, kDamagePopTime, m_cameraDir);
	}
	// 30以上で拡大値2.5
	else if (resDamagePoint < 50)
	{
		PanelManager::Instance().SetNumber(resDamagePoint, m_pos, 1.0f, kDamagePopTime, m_cameraDir);
	}
	// 50以上で拡大値3.0
	else if (resDamagePoint < 80)
	{
		PanelManager::Instance().SetNumber(resDamagePoint, m_pos, 1.2f, kDamagePopTime, m_cameraDir);
	}
	// 80以上で拡大値3.5
	else
	{
		PanelManager::Instance().SetNumber(resDamagePoint, m_pos, 1.5f, kDamagePopTime, m_cameraDir);
	}

	// 反映
	m_parameter.SetDamage(resDamagePoint);

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

	EffectManager::Instance().PlayEffect("Hit_" + m_name, m_collider.centerPos, m_dir);

	return true;

#ifdef _DEBUG

	printf("\n%d のダメージ！\n松佳 エイリ  残HP：%d\n", resDamagePoint, m_parameter.GetPalameter(0));
	if (m_parameter.GetPalameter(0) <= 0)
	{
		printf("討伐っ！\n");
	}

#endif // _DEBUG
}

void EnemyMagic::MoveUpdate()
{
	auto vec = VSub(m_pos, m_targetPos);
	vec.y = 0;
	// プレイヤー接近時逃走
	if (VSize(vec) < kEscapeRange && m_state->GetIsMove())
	{
		DrawingManager::Instance().CallAnimation(m_name, "Armature|Walk", 60);
		m_physics.Move(VNorm(vec), kMovePow);
	}
	// プレイヤー攻撃範囲内
	else if (VSize(vec) < kAttackRange && m_state->GetIsAttack())
	{

		if (m_state->GetState() != State::ATTACK)
		{
			m_physics.SetDir(VNorm(VSub(VZero(), vec)));
			ChangeState(State::ATTACK);
			m_waitTimeMax = kSpellingTime;
			m_waitTimer = 0;
		}
	}
	// それ以外はアイドル
	else if(m_state->GetState() == State::MOVE)
	{
		DrawingManager::Instance().CallAnimation(m_name, "Armature|Idle", 120);
	}
}

void EnemyMagic::AttackUpdate()
{
	if (m_state->GetState() == State::ATTACK)
	{
		DrawingManager::Instance().CallAnimation(m_name, "Armature|Magic", kSpellingTime);

		// 魔法弾生成
		if (m_waitTimer == static_cast<int>(m_waitTimeMax * 0.35f))
		{
			Quaternion q;
			for (int i = 0; i < kShotNum; i++)
			{
				EnemyShot shotTemp;
				shotTemp.shotCol.SetSphere(m_collider.centerPos, -1, kShotRadius, true);
				shotTemp.shotCol.isChackOther = true;
				shotTemp.shotCount = 0;
				shotTemp.baseID = m_id;
				shotTemp.shotID = m_shotNum;
				m_shotNum++;
				if (m_shotNum >= kShotMaxNum)
				{
					m_shotNum = 0;
				}

				// 方向設定
				auto dir = VSub(m_targetPos, m_collider.centerPos);
				dir.y = 0;
				dir = VNorm(dir);
				// 30度ずつずらす
				auto angle = (i - 1) * (DX_PI_F / 3);
				auto upVec = VGet(0, 1, 0);
				auto zeroVec = VZero();
				q.SetMove(angle, upVec, zeroVec);
				dir = q.Move(zeroVec, dir);
				shotTemp.shotDir = dir;
				// 弾の設定反映
				DrawingManager::Instance().RegisterOtherModel("ShotM" + m_name + "_" + std::to_string(shotTemp.shotID), shotTemp.shotCol.pos[0], ModelForm::SPHERE, 0xff0030, 0xffffff, kShotRadius, true);
				EffectManager::Instance().PlayEffect("ShotM" + m_name + "_" + std::to_string(shotTemp.shotID), shotTemp.shotCol.pos[0], dir);
				m_magicShot.push_back(shotTemp);
			}
		}
		// 攻撃終了
		if (m_waitTimer == m_waitTimeMax)
		{
			ChangeState(State::MOVE);
		}
	}
}

void EnemyMagic::MagicUpdate()
{
	for (int i = 0; i < m_magicShot.size(); i++)
	{
		m_magicShot[i].shotCount++;
		m_magicShot[i].shotCol.pos[0] = VAdd(m_magicShot[i].shotCol.pos[0], VScale(m_magicShot[i].shotDir, kShotSpeed));
		DrawingManager::Instance().UpdateModelData("ShotM" + m_name + "_" + std::to_string(m_magicShot[i].shotID), m_magicShot[i].shotCol.pos[0], VZero());
		EffectManager::Instance().UpdateEffectData("ShotM" + m_name + "_" + std::to_string(m_magicShot[i].shotID), m_magicShot[i].shotCol.pos[0], m_magicShot[i].shotDir);

		if (m_magicShot[i].shotCount >= kShotTime)
		{
			// 消滅条件時、消して数値補正
			DrawingManager::Instance().DeleteModel("ShotM" + m_name + "_" + std::to_string(m_magicShot[i].shotID));
			EffectManager::Instance().StopEffect("ShotM" + m_name + "_" + std::to_string(m_magicShot[i].shotID));
			m_magicShot.erase(m_magicShot.begin() + i);
			i--;
		}
	}
}

void EnemyMagic::StanUpdate()
{
	// スタンエフェクトの更新
	auto stanPos = VGet(m_collider.pos[1].x, m_collider.pos[1].y + 10.0f, m_collider.pos[1].z);
	EffectManager::Instance().UpdateEffectData("Stan_" + m_name, stanPos, m_dir);
	if (m_state->GetState() == State::STAN)
	{
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

void EnemyMagic::KnockBackUpdate()
{
	if (m_state->GetState() == State::KNOCK)
	{
		DrawingManager::Instance().CallAnimation(m_name, "Armature|KnockBack", kKnockTime);
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
