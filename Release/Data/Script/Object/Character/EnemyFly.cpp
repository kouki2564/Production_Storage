#include "EnemyFly.h"
#include "Quaternion.h"
#include "ChackCollision.h"

#include "SoundManager.h"
#include "EffectManager.h"
#include "HandleManager.h"

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
	constexpr float kRadius = 8.0f;

	constexpr float kScale = 10.0f;

	// アイドル時間
	constexpr int kIdleTime = 90;

	// スタン時間
	constexpr int kStanTime = 250;

	// 移動速度
	constexpr float kFloatingPow = 0.01f;

	// 弾の半径
	constexpr float kShotRadius = 3.0f;

	// 魔法弾数
	constexpr int kShotNum = 3;

	// 魔法速度
	constexpr float kShotSpeed = 1.5f;

	// 魔法残存時間
	constexpr int kShotLimitTime = 200;

	// 魔法攻撃時間
	constexpr int kSpellingTime = 45;

	// 攻撃タイミング
	constexpr int kShotTime = 15;

	// 攻撃待機時間
	constexpr int kAttackCoolTime = 80;

	// 攻撃開始距離
	constexpr float kAttackRange = 100.0f;

	// タイトルのアニメーション時間
	constexpr int kTitleAnimationTime = 60;

	// 各ステータス（変動なしのためconstexpr）
	// HP
	constexpr int kMaxHP = 75;
	// 攻撃
	constexpr int kAtk = 10;
	// 防御
	constexpr int kDef = 30;
	// 素早さ
	constexpr int kAgi = 10;

	constexpr int kKnockTime = 25;
	constexpr float kKnockPow = 0.1f;
	
	constexpr int kStanCoolTime = 180;

	constexpr int kDeathTime = 60;

	constexpr int kDamagePopTime = 30;
}

EnemyFly::EnemyFly()
{
	for (int i = 0; i < 10; i++)
	{
		// EffectManager::Instance().RegisterEffect("ShotF_" + std::to_string(i), LoadEffekseerEffect("Data/Effect/EnemyShot.efkefc", 1.0f), VGet(0, 0, 0), VGet(0, 0, 0));
	}
	// 重力設定
	m_physics.SetGravity(0.0f, VZero());
	// 着弾点のエフェクトがほしいところ
}

EnemyFly::~EnemyFly()
{
	EffectManager::Instance().DeleteEffect("Hit_" + m_name);
	EffectManager::Instance().DeleteEffect("Stan_" + m_name);
	EffectManager::Instance().DeleteEffect("Pos_" + m_name);
	MV1DeleteModel(m_handle);
	for (int i = 0; i < m_magicShot.size(); i++)
	{
		DrawingManager::Instance().DeleteModel("ShotF_" + m_name + "_" + std::to_string(m_magicShot[i].shotID));
		EffectManager::Instance().StopEffect("ShotF_" + m_name + "_" + std::to_string(m_magicShot[i].shotID));
	}
	m_magicShot.clear();
}

void EnemyFly::Init(std::string name, VECTOR pos, VECTOR dir)
{
	// m_handle = MV1DuplicateModel(handle);
	m_handle = MV1DuplicateModel(HandleManager::Instance().GetModelHandle(ModelName::ENEMY4));
	m_name = name;
	m_dir = dir;
	m_pos = pos;
	m_floatingRange = pos.y;
	m_determinedTargetPos = VAdd(m_pos, m_dir);
	auto frameIndex0 = MV1SearchFrame(m_handle, "Skeleton_Mage_Hat");
	auto frameIndex1 = MV1SearchFrame(m_handle, "root");
	auto scale = VGet(kScale, kScale, kScale);
	m_modelHeight = (MV1GetFramePosition(m_handle, frameIndex0).y - MV1GetFramePosition(m_handle, frameIndex1).y) * kScale;
	auto modelPos = VGet(m_pos.x, m_pos.y - m_modelHeight, m_pos.z);
	DrawingManager::Instance().RegisterModel(m_name, m_handle, modelPos, m_dir, scale);
	//DrawingManager::Instance().RegisterOtherModel(m_name, m_pos, ModelForm::SPHERE, 0xff0000, 0xffffff, kRadius, true);
	m_collider.SetSphere(m_pos, 2, kRadius, true);
	DrawingManager::Instance().CallAnimation(m_name, "2H_Melee_Idle", kIdleTime);

	EffectManager::Instance().RegisterEffect("Hit_" + m_name, HandleManager::Instance().GetEffectHandle(EffectName::HIT), m_pos, m_dir);
	EffectManager::Instance().RegisterEffect("Stan_" + m_name, HandleManager::Instance().GetEffectHandle(EffectName::STAN), m_pos, m_dir);
	auto effectPos = VGet(m_pos.x, 0.0f, m_pos.z);
	EffectManager::Instance().RegisterEffect("Pos_" + m_name, HandleManager::Instance().GetEffectHandle(EffectName::ENEMYPOS), effectPos, m_dir);
	EffectManager::Instance().PlayEffect("Pos_" + m_name, effectPos, m_dir);

	for (int i = 0; i < kShotNum; i++)
	{
		EffectManager::Instance().RegisterEffect("ShotF_" + m_name + "_" + std::to_string(i), HandleManager::Instance().GetEffectHandle(EffectName::FLYSHOT), VGet(0, 0, 0), VGet(0, 0, 0));
	}

	//当たり判定設定
	m_frameIndex = MV1SearchFrame(m_handle, "Skeleton_Mage_Hat");
	// auto poss = MV1GetFramePosition(m_handle, m_frameIndex);
	m_hitCol.SetSphere(m_pos, -1, 0, true);
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

void EnemyFly::Update()
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

void EnemyFly::ApplyUpdating()
{
	m_dir = m_physics.GetDir();
	auto moveVec = m_physics.GetUpdateVec();
	// 座標反映
	m_pos = VAdd(m_pos, moveVec);
	m_collider.ColliderUpdate(moveVec);

	m_hitCol.pos[0] = MV1GetFramePosition(m_handle, m_frameIndex);

	auto effectPos = VGet(m_pos.x, 0.0f, m_pos.z);
	EffectManager::Instance().UpdateEffectData("Pos_" + m_name, effectPos, m_dir);

#ifdef _DEBUG
	DrawSphere3D(m_collider.pos[0], kRadius, 8, 0xff0000, 0xffffff, false);
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

	// 弾の更新
	for (int i = 0; i < m_magicShot.size(); i++)
	{
		// DrawSphere3D(m_magicShot[i].shotCol.pos[0], kShotRadius, 8, 0xffff00, 0xffffff, true);
		DrawingManager::Instance().UpdateModelData("ShotF_" + m_name + "_" + std::to_string(m_magicShot[i].shotID), m_magicShot[i].shotCol.pos[0], VZero());
	}

	auto modelPos = VGet(m_pos.x, m_pos.y - m_modelHeight, m_pos.z);
	DrawingManager::Instance().UpdateModelData(m_name, modelPos, m_dir);
}

void EnemyFly::InitTitleAnimation()
{
	DrawingManager::Instance().CallAnimation(m_name, "Cheer", kTitleAnimationTime);
}

bool EnemyFly::GetIsHitAttack(Collider& colOther)
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
				DrawingManager::Instance().DeleteModel("ShotF_" + m_name + "_" + std::to_string(m_magicShot[i].shotID));
				EffectManager::Instance().StopEffect("ShotF_" + m_name + "_" + std::to_string(m_magicShot[i].shotID));
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

bool EnemyFly::SetDamagePoint(float damagePoint, bool isStan, bool isPowKnock)
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

	// 反映
	m_parameter.SetDamage(resDamagePoint);
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

	// 一応HPのマイナス化防止とやられた判定をここでとる
	if (m_parameter.GetPalameter(0) <= 0)
	{
		ChangeState(State::DEATH);
		DrawingManager::Instance().CallAnimation(m_name, "Death_A", kDeathTime);
		m_waitTimeMax = kDeathTime;
		m_waitTimer = 0;
		// 当たり判定の消去
		m_collider.isOnDamage = false;
		// 死んだあとには攻撃判定を消す
		m_hitCol.isChackOther = false;
	}

	EffectManager::Instance().PlayEffect("Hit_" + m_name, m_collider.pos[0], m_dir);

	return true;

#ifdef _DEBUG

	printf("\n%d のダメージ！\n浮嶋 フロウ  残HP：%d\n", resDamagePoint, m_parameter.GetPalameter(0));
	if (m_parameter.GetPalameter(0) <= 0)
	{
		printf("討伐っ！\n");
	}

#endif // _DEBUG
}

void EnemyFly::MoveUpdate()
{
	// 向きの更新
	auto vec = VSub(m_pos, m_determinedTargetPos);
	vec.y = 0;
	m_physics.SetDir(VNorm(VSub(VZero(), vec)));

	// 浮き沈み
	if (m_pos.y >= m_floatingRange + 3.0f)
	{
		m_isFloating = false;
	}
	else if (m_pos.y <= m_floatingRange - 3.0f)
	{
		m_isFloating = true;
	}

	if (m_isFloating)
	{
		m_physics.Move(VGet(0, (m_floatingRange + 5.0f) - m_pos.y, 0), kFloatingPow);
	}
	else
	{
		m_physics.Move(VGet(0, (m_floatingRange - 5.0f) - m_pos.y, 0), kFloatingPow);
	}

	// アイドル時間の経過で攻撃状態に移行
	if (m_waitTimeMax == kIdleTime && m_waitTimer >= kIdleTime)
	{
		if (m_state->GetIsAttack())
		{
			ChangeState(State::ATTACK);
			m_waitTimeMax = kAttackCoolTime;
			m_waitTimer = 0;
			// 攻撃モーションの開始
			DrawingManager::Instance().CallAnimation(m_name, "2H_Melee_Attack_Spinning", kAttackCoolTime);
		}
	}
	// それ以外はアイドル
	else
	{
		DrawingManager::Instance().CallAnimation(m_name, "2H_Melee_Idle", 120);
	}
}

void EnemyFly::AttackUpdate()
{
	if (m_state->GetState() == State::ATTACK)
	{
		// DrawingManager::Instance().CallAnimation(m_name, "Armature|Magic", kSpellingTime);

		// クールタイム終了次第攻撃タイムに移行
		if (m_waitTimeMax == kAttackCoolTime)
		{
			// 発射先の座標の更新
			if (m_waitTimer <= kAttackCoolTime * 0.85f)
			{
				m_determinedTargetPos = m_targetPos;

			}

			// クールタイム更新
			if (m_waitTimer == m_waitTimeMax)
			{
				m_waitTimeMax = kSpellingTime;
				m_waitTimer = 0;
				// 攻撃モーションの開始
				DrawingManager::Instance().CallAnimation(m_name, "2H_Melee_Attack_Chop", kSpellingTime / 3);
			}
		}

		// 魔法弾を一定フレーム毎に生成
		if (m_waitTimeMax == kSpellingTime)
		{
			if (m_magicShot.size() < kShotNum && m_waitTimer % kShotTime == 0)
			{
				EnemyShot shotTemp;

				// IDの設定
				int id = 0;
				for (int i = 0; i < m_magicShot.size(); i++)
				{
					// IDの重複を避ける
					if (m_magicShot[i].shotID == id)
					{
						id++;
						// 一度0に戻して再度チェック
						i = -1;
					}
				}
				shotTemp.shotID = id;

				shotTemp.shotCol.SetSphere(m_collider.pos[0], -1, kShotRadius, true);
				shotTemp.shotCol.isChackOther = true;
				shotTemp.shotCount = 0;

				// 方向設定
				auto dir = VSub(m_determinedTargetPos, m_collider.pos[0]);
				dir = VNorm(dir);
				shotTemp.shotDir = dir;
				// 弾の設定反映
				DrawingManager::Instance().RegisterOtherModel("ShotF_" + m_name + "_" + std::to_string(shotTemp.shotID), shotTemp.shotCol.pos[0], ModelForm::SPHERE, 0xffff00, 0xffffff, kShotRadius, true);
				EffectManager::Instance().PlayEffect("ShotF_" + m_name + "_" + std::to_string(shotTemp.shotID), shotTemp.shotCol.pos[0], dir);
				m_magicShot.push_back(shotTemp);
			}
			// 攻撃終了
			if (m_waitTimer == m_waitTimeMax)
			{
				ChangeState(State::MOVE);
				m_waitTimeMax = kIdleTime;
				m_waitTimer = 0;
				DrawingManager::Instance().CallAnimation(m_name, "2H_Melee_Idle", kIdleTime);
			}
		}

		// 導線の表示
		auto dir = VNorm(VSub(m_determinedTargetPos, m_collider.pos[0]));
		DrawLine3D(m_collider.pos[0], VAdd(m_determinedTargetPos, VScale(dir, 30.0f)), 0xe00000);
	}
}

void EnemyFly::MagicUpdate()
{
	for (int i = 0; i < m_magicShot.size(); i++)
	{
		m_magicShot[i].shotCount++;
		m_magicShot[i].shotCol.pos[0] = VAdd(m_magicShot[i].shotCol.pos[0], VScale(m_magicShot[i].shotDir, kShotSpeed));
		EffectManager::Instance().UpdateEffectData("ShotF_" + m_name + "_" + std::to_string(m_magicShot[i].shotID), m_magicShot[i].shotCol.pos[0], m_magicShot[i].shotDir);

		if (m_magicShot[i].shotCount >= kShotLimitTime)
		{
			// 消滅条件時、消して数値補正
			DrawingManager::Instance().DeleteModel("ShotF_" + m_name + "_" + std::to_string(m_magicShot[i].shotID));
			EffectManager::Instance().StopEffect("ShotF_" + m_name + "_" + std::to_string(m_magicShot[i].shotID));
			m_magicShot.erase(m_magicShot.begin() + i);
			i--;
		}
	}
}

void EnemyFly::KnockBackUpdate()
{
	if (m_state->GetState() == State::KNOCK)
	{
		DrawingManager::Instance().CallAnimation(m_name, "Jump_Start", kKnockTime);
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
				DrawingManager::Instance().CallAnimation(m_name, "2H_Melee_Idle", kStanTime / 10);
			}
			else
			{
				// 通常の移動状態に
				ChangeState(State::MOVE);
				m_waitTimeMax = kIdleTime;
				m_waitTimer = 0;
			}
			DrawingManager::Instance().CallAnimation(m_name, "2H_Melee_Idle", 120);
		}
#ifdef _DEBUG
		DrawSphere3D(m_collider.pos[0], kRadius+ 0.2f, 8, 0x00ff00, 0xffffff, false);
#endif // _DEBUG

	}
}

void EnemyFly::StanUpdate()
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
			DrawingManager::Instance().CallAnimation(m_name, "2H_Melee_Idle", 120);
		}
	}
}
