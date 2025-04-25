#include "Player.h"
// State関連
#include "Move.h"
#include "Jump.h"
#include "Attack.h"
#include "Knock.h"
#include "Dodge.h"
#include "Death.h"
// 武器
#include "WeaponBase.h"
#include "Hand.h"
#include "Sword.h"
#include "Axe.h"
#include "Magic.h"
// その他
#include "Controller.h"
#include "ChackCollision.h"

#include "HandleManager.h"
#include "SoundManager.h"
#include "EffectManager.h"

namespace
{
	constexpr int kSwordAttackMax = 3;
	constexpr int kAxeAttackMax = 1;
	constexpr int kMagicAttackMax = 1;

	constexpr float kTargetRange = 100;

	// 攻撃の時間
	constexpr int kSwordWaitTime = 30;
	constexpr int kLastSwordWaitTime = 60;
	constexpr int kAirSwordWaitTime = 55;
	constexpr float kSwordMoveSpeed = 1.0f;

	constexpr int kAxeWaitTime =90;
	constexpr int kLastAxeWaitTime = 90;
	constexpr float kAxeMoveSpeed = 0.75f;

	constexpr int kMagicWaitTime = 190;
	constexpr int kLastMagicWaitTime = 190;
	constexpr float kMagicMoveSpeed = 1.2f;

	constexpr float kJumpAttackHeight = 0.0f;

	// 攻撃の当たり判定開始時間
	constexpr int kSwordHitStartTime = 15;
	constexpr int kSwordLastHitStartTime = 30;
	constexpr int kSwordAirHitStartTime = 20;

	constexpr int kAxeHitStartTime = 30;

	constexpr int kAxeLastHitStartTime = 30;

	// スタン時間
	constexpr int kStanTime = 250;

	constexpr int kDodgeWaitTime = 50;

	constexpr int kJumpTime = 50;
	constexpr float kJumpHeight = 40;

	constexpr int kKnockTime = 60;
	constexpr float kKnockPow = 1.0f;

	// タイトルのアニメーション時間
	constexpr int kTitleAnimationTime = 80;


	constexpr int kDeathTime = 10;
}


Player::Player()
{
	m_dir = VZero();
	m_cameraDir = VZero();
	std::shared_ptr<Move> pState = std::make_shared<Move>();
	m_state = pState;
	std::shared_ptr<Hand> pWeapon = std::make_shared<Hand>();
	m_weapon = pWeapon;

	// パラメータ設定(マジックナンバー)
	m_parameter.SetPalameter(250, 10, 10, 10);
	EffectManager::Instance().RegisterEffect("HitP", HandleManager::Instance().GetEffectHandle(EffectName::HIT), VGet(0, 0, 0), VGet(0, 0, 0));
	EffectManager::Instance().RegisterEffect("StanP", HandleManager::Instance().GetEffectHandle(EffectName::STAN), VGet(0, 0, 0), VGet(0, 0, 0));
	EffectManager::Instance().RegisterEffect("PlayerPos", HandleManager::Instance().GetEffectHandle(EffectName::PLAYERPOS), VGet(0, 0, 0), VGet(0, 0, 0));
}

Player::~Player()
{
	EffectManager::Instance().DeleteEffect("HitP");
	EffectManager::Instance().DeleteEffect("StanP");
	EffectManager::Instance().DeleteEffect("PlayerPos");
	MV1DeleteModel(m_handle);
}

void Player::Init(std::string name, VECTOR pos, VECTOR dir)
{
	// m_state = std::make_shared<Move>();
	m_handle = MV1DuplicateModel(HandleManager::Instance().GetModelHandle(ModelName::PLAYER));
	m_name = name;
	m_pos = pos;
	m_dir = dir;
	auto frameName0 = MV1SearchFrame(m_handle, "Mesh");
	auto pos0 = VAdd(m_pos, MV1GetFramePosition(m_handle, frameName0));
	auto frameName1 = MV1SearchFrame(m_handle, "mixamorig:Head");
	auto pos1 = VGet(pos0.x, MV1GetFramePosition(m_handle, frameName1).y * 15, pos0.z);
	auto scale = VGet(15.0f, 15.0f, 15.0f);
	DrawingManager::Instance().RegisterModel(name, m_handle, pos0, m_dir, scale);

	EffectManager::Instance().UpdateEffectData("PlayerPos", m_pos, VZero());
	EffectManager::Instance().PlayEffect("PlayerPos", m_pos, VZero());

	m_collider.SetCapsule(pos0, pos1, 2, 3, true);

	// マジックナンバー
	// DrawingManager::Instance().CallAnimation(m_name, "Armature|Idle", 1.0f); 
	m_collider.isOnDamage = true;
}

void Player::Update()
{
	MoveUpdate();
	JumpUpdate();
	AttackUpdate();
	KnockBackUpdate();
	StanUpdate();
	DodgeUpdate();
	WaitTimeCount();
	// 行動から半分の時間経過後から前入力開始
	if (m_waitTimer >= m_waitTimeMax * 0.5f)
	{
		PrecedInputButton();
	}
	else
	{
		m_inputButton = Button::NONE;
	}

	if (m_state->GetState() != State::JUMP)
	{
		// とりあえずここに配置
		// ジャンプ時以外の跳ね上がりを防止
		m_jumpTime = kJumpTime;
	}

	// 魔法球の座標の仮実装
	if (m_weapon->kind == Weapon::MAGIC)
	{
		auto dirRight = VScale(VNorm(VCross(m_dir, VGet(0, -1, 0))), 8);
		auto up = VGet(0, 8, 0);
		auto pos = VAdd(VAdd(m_pos,dirRight), up);
		m_weapon->SetPos(pos);
	}

	m_weapon->WeaponUpdate();

	// 武器変更(攻撃中はできない)
	if (Controller::Instance().GetInputDown(Button::STICK_L) && !m_isTutorial && m_state->GetState() != State::ATTACK)
	{
		auto nowWeapon = m_weapon->kind;
		if (nowWeapon == Weapon::NONE)
		{
			ChangeWeapon(Weapon::SWORD);
		}
		else if (nowWeapon == Weapon::SWORD)
		{
			ChangeWeapon(Weapon::AXE);
		}
		else if (nowWeapon == Weapon::AXE)
		{
			ChangeWeapon(Weapon::MAGIC);
		}
		else if (nowWeapon == Weapon::MAGIC)
		{
			ChangeWeapon(Weapon::SWORD);
		}
	}

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

void Player::ApplyUpdating()
{
#ifdef _DEBUG

	// 各数値読み取り
	if (Controller::Instance().GetInputDown(Button::STICK_R))
	{
		printf("\nPlayer動作状況表示\n");
		printf("pos   : %f, %f, %f\n", m_pos.x, m_pos.y, m_pos.z);
		printf("dir   : %f, %f, %f\n", m_dir.x, m_dir.y, m_dir.z);
		auto vec = m_physics.GetMove();
		printf("move  : %f, %f, %f\n", vec.x, vec.y, vec.z);
		vec = m_physics.GetPush();
		printf("push  : %f, %f, %f\n", vec.x, vec.y, vec.z);
		vec = m_physics.GetKnock();
		printf("knock : %f, %f, %f\n", vec.x, vec.y, vec.z);
		vec = m_physics.GetJump();
		printf("jump  : %f, %f, %f\n", vec.x, vec.y, vec.z);
		auto pow = m_physics.GetGravity();
		printf("grav  : %f\n", pow);
		vec = m_physics.GetFloorPushVec();
		printf("floor : %f, %f, %f\n", vec.x, vec.y, vec.z);
	}


	DrawCapsule3D(m_collider.pos[0], m_collider.pos[1], 5, 8, 0x00ff00, 0xffffff, false);
	DrawLine3D(m_pos, VAdd(m_pos, VScale(m_dir, 15)), 0xff0000);

	DrawFormatString(1000, 30, 0xffff00, "プレイヤーHP： %d", m_parameter.GetPalameter(0));
#endif // _DEBUG

	ApplyMoving();

	auto effectPos = m_pos;
	effectPos.y = 0.0f;
	EffectManager::Instance().UpdateEffectData("PlayerPos", effectPos, VZero());

	if (m_state->GetState() != State::JUMP)
	{
		
		auto shadowPos = m_pos;
		// shadowPos.y = 0;
		auto shadowPosUnder = shadowPos;
		shadowPos.y -= -1.0f;
		// DrawCone3D(shadowPosUnder, shadowPos, 3, 8, 0x000000, 0x000000, true);
	}

	DrawingManager::Instance().UpdateModelData(m_name, m_pos, m_dir);
	DrawingManager::Instance().UpdateWeaponModelData(m_weapon->name);
}

void Player::InitTitleAnimation()
{
	DrawingManager::Instance().CallAnimation(m_name, "Armature|Sword_Idle", kTitleAnimationTime);
}

void Player::SetWeapon(int num)
{
	if (num == 0)
	{
		ChangeWeapon(Weapon::SWORD);
	}
	else if (num == 1)
	{
		ChangeWeapon(Weapon::AXE);
	}
	else if (num == 2)
	{
		ChangeWeapon(Weapon::MAGIC);
	}
}

void Player::SetChaseTarget(VECTOR targetPos)
{ 
	auto targetVec = VSub(targetPos, m_pos);
	targetVec.y = 0;
	if (VSize(targetPos) != 0)
	{
		// ターゲットが一定範囲内にいるか
		if (VSize(targetVec) < kTargetRange)
		{
			m_targetPos = targetPos;
			m_isTarget = true;
		}
		// そうでない場合はプレイヤーの方向に進む
		else
		{
			m_targetPos = VAdd(m_collider.centerPos, VScale(m_dir, 80));
			m_isTarget = false;
		}
	}
	else
	{
		m_targetPos = VAdd(m_collider.centerPos, VScale(m_dir, 80));
		m_isTarget = false;
	}
}

void Player::OnDamage(int damage)
{
}

void Player::SetKnockBack(float knockBack)
{
}

bool Player::SetDamagePoint(float damagePoint, bool isStan, bool isPowKnock)
{
	if (m_state->GetIsOnDamage())
	{
		// 防御値分の計算
		int resDamagePoint = static_cast<int>(damagePoint - (m_parameter.GetPalameter(2) * 0.1f));

		// 最低限のダメージは入れてクリア不可能対策
		if (resDamagePoint <= 1) resDamagePoint = 1;
		// 一定以上のダメージの場合はノックバックに移行
		else
		{
			if (m_state->GetIsKnockBack())
			{
				ChangeState(State::KNOCK);
				m_waitTimeMax = kKnockTime;
				m_waitTimer = 0;
				m_weapon->OffIsCollider();
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
		// チュートリアル時のダメージ固定
		if (m_isTutorial)
		{
			resDamagePoint = 1;
		}
		// 反映
		m_parameter.SetDamage(resDamagePoint);
		SoundManager::Instance().OnePlay("Hit");

		EffectManager::Instance().PlayEffect("HitP", m_collider.centerPos, m_dir);
		

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

		// 一応HPのマイナス化防止とやられた判定をここでとる
		if (m_parameter.GetPalameter(0) <= 0)
		{
			ChangeState(State::DEATH);
			DrawingManager::Instance().CallAnimation(m_name, "Armature|Axe_Ready", kDeathTime);
			m_waitTimeMax = kDeathTime;
			m_waitTimer = 0;
			// 当たり判定の消去
			m_collider.isOnDamage = false;
			// 死んだあとには攻撃判定を消す
		}

#ifdef _DEBUG

		printf("\n%d のダメージ！\nプレイヤー残HP：%d\n", resDamagePoint, m_parameter.GetPalameter(0));
		if (m_parameter.GetPalameter(0) <= 0)
		{
			printf("ヤラレチャッタ...\n");
		}

#endif // _DEBUG
		return true;
	}
	else
	{
		return false;
	}
}

bool Player::GetIsHitAttack(Collider& colOther)
{
	if (colOther.isOnDamage)
	{
		std::shared_ptr<ChackCollision> chacker = std::weak_ptr<ChackCollision>().lock();
		bool isHit = false;
		m_damagePoint = 0;
		for (int i = 0; i < m_weapon->colliders.size(); i++)
		{
			isHit = chacker->GetIsCollision(m_weapon->colliders[i], colOther);
			if (isHit)
			{
				m_weapon->colliders[i].isChackOther = false;

				/* 与えるダメージの計算処理 */

				// 連撃数をもとに基礎攻撃値を設定
				float tempPoint = m_parameter.GetPalameter(1);
				// 武器ごとに攻撃倍率を付与
				if (m_weapon->kind == Weapon::SWORD)
				{
					m_weapon->OffTrajectoryCollision();
					tempPoint *= m_attackCount * 0.8f;
					m_damagePoint += tempPoint * 1.5f;
				}
				else if (m_weapon->kind == Weapon::AXE)
				{
					m_weapon->OffTrajectoryCollision();
					m_damagePoint += tempPoint * 5.0f;
				}
				else if (m_weapon->kind == Weapon::MAGIC)
				{
					m_damagePoint += tempPoint * 5.5f;
				}
				else
				{
					m_damagePoint += 0;
				}
				// 空中時のダメージ増加
				if (m_isSkyBaff)
				{
					m_damagePoint *= 1.35f;
					m_isSkyBaff = false;
				}
#ifdef _DEBUG
				printf("\nHit\n");
				printf("Damage： % f\n", m_damagePoint);
#endif // _DEBUG

				break;
			}
		}
		return isHit;
	}
	else
	{
		return false;
	}
}

void Player::SetPosAndDir(VECTOR pos, VECTOR dir)
{
	m_updateVec = VZero();

	m_pos = pos;
	m_dir = dir;
	
	auto frameName0 = MV1SearchFrame(m_handle, "Mesh");
	auto pos0 = VAdd(m_pos, MV1GetFramePosition(m_handle, frameName0));
	auto frameName1 = MV1SearchFrame(m_handle, "mixamorig:Head");
	auto pos1 = VGet(pos0.x, MV1GetFramePosition(m_handle, frameName1).y * 15 + 5, pos0.z);

	DrawingManager::Instance().UpdateModelData(m_name, pos0, m_dir);
	m_collider.SetCapsule(pos0, pos1, 2, 3, true);


	DrawingManager::Instance().UpdateWeaponModelData(m_weapon->name);
}

void Player::SetUpgrade(int paramNum)
{
	if (paramNum == 0)
	{
		m_parameter.UpGradeParameter(paramNum, 50);
	}
	else if (paramNum == 1)
	{
		m_parameter.UpGradeParameter(paramNum, 5);
	}
	else if (paramNum == 2)
	{
		m_parameter.UpGradeParameter(paramNum, 5);
	}
	else if (paramNum == 3)
	{
		m_parameter.UpGradeParameter(paramNum, 5);
	}
}

void Player::ChangeState(State state)
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
			m_physics.DirControll(m_cameraDir);
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

void Player::ChangeWeapon(Weapon weaponKind)
{
	auto nowWeapon = m_weapon->kind;
	if (nowWeapon != weaponKind)
	{
		printf("\n武器の変更\n");
		if (weaponKind == Weapon::NONE)
		{
			std::shared_ptr<Hand> pWeapon = std::make_shared<Hand>();
			m_weapon = pWeapon;
			printf("NONE\n");
		}
		else if (weaponKind == Weapon::SWORD)
		{
			std::shared_ptr<Sword> pWeapon = std::make_shared<Sword>();
			m_weapon = pWeapon;
			DrawingManager::Instance().RegisterWeaponModel(m_weapon->name, m_weapon->handle, m_weapon->scale, m_weapon->rot, m_name, "mixamorig:RightHandIndex4");
			DrawingManager::Instance().SetIsShadowModel(m_weapon->name, false);
			m_isGiveStanFrag = false;
			m_isPowerKnock = false;
			printf("SWORD\n");
		}
		else if (weaponKind == Weapon::AXE)
		{
			std::shared_ptr<Axe> pWeapon = std::make_shared<Axe>();
			m_weapon = pWeapon;
			DrawingManager::Instance().RegisterWeaponModel(m_weapon->name, m_weapon->handle, m_weapon->scale, m_weapon->rot, m_name, "mixamorig:RightHandIndex4");
			DrawingManager::Instance().SetIsShadowModel(m_weapon->name, false);
			m_isGiveStanFrag = true;
			m_isPowerKnock = true;
			printf("AXE\n");
		}
		else if (weaponKind == Weapon::MAGIC)
		{
			std::shared_ptr<Magic> pWeapon = std::make_shared<Magic>();
			m_weapon = pWeapon;
			m_isGiveStanFrag = false;
			m_isPowerKnock = true;
			printf("MAGIC\n");
			if (m_weapon->kind == Weapon::MAGIC)
			{
				auto dirRight = VScale(VNorm(VCross(m_dir, VGet(0, -1, 0))), 8);
				auto up = VGet(0, 8, 0);
				auto pos = VAdd(VAdd(m_pos, dirRight), up);
				m_weapon->SetPos(pos);
			}
		}
		// 座標更新
		m_weapon->WeaponUpdate();
		DrawingManager::Instance().UpdateWeaponModelData(m_weapon->name);
	}
}

void Player::MoveUpdate()
{
	if (m_state->GetIsMove())
	{
		// コントローラ移動値の更新
		switch (m_weapon->kind)
		{
		case Weapon::NONE:
			m_physics.MoveControll(m_cameraDir, m_parameter.GetPalameter(3) * 0.1f);
			break;
		case Weapon::SWORD:
			m_physics.MoveControll(m_cameraDir, m_parameter.GetPalameter(3) * 0.1f * kSwordMoveSpeed);
			break;
		case Weapon::AXE:
			m_physics.MoveControll(m_cameraDir, m_parameter.GetPalameter(3) * 0.1f * kAxeMoveSpeed);
			break;
		case Weapon::MAGIC:
			m_physics.MoveControll(m_cameraDir, m_parameter.GetPalameter(3) * 0.1f * kMagicMoveSpeed);
			break;
		default:
			break;
		}
		// 移動状態
		auto moveValue = VSize(m_physics.GetMove());
		// ChangeState(State::MOVE);
		if (m_state->GetState() == State::MOVE)
		{
			if (moveValue != 0)
			{
				DrawingManager::Instance().CallAnimation(m_name, "Armature|Run", 40 * m_parameter.GetPalameter(3) * 0.1f / moveValue);
			}
			else
			{
				auto weapon = m_weapon->kind;
				// 後回し　アニメーション遷移で絶妙にブレンドされたまま変なポーズをとっている対策
				if (weapon == Weapon::NONE)
				{
					DrawingManager::Instance().CallAnimation(m_name, "Armature|Idle", 1);
				}
				else if (weapon == Weapon::SWORD)
				{
					DrawingManager::Instance().CallAnimation(m_name, "Armature|Sword_Idle", 1);
				}
				else if (weapon == Weapon::AXE)
				{
					DrawingManager::Instance().CallAnimation(m_name, "Armature|Axe_Idle", 1);
				}
				else if (weapon == Weapon::MAGIC)
				{
					DrawingManager::Instance().CallAnimation(m_name, "Armature|Magic_Idle", 1);
				}
			}
		}
		//else
		//{
		//	ChangeState(State::JUMP);
		//	//////////////////
		//	DrawingManager::Instance().CallTransAnimation(m_name, "Armature|Jump_Idle", 10);
		//	////////////////////
		//}
	}
}

void Player::JumpUpdate()
{
	if (m_state->GetIsJump())
	{
		if (m_inputButton == Button::A && m_physics.GetIsJump())
		{
			ChangeState(State::JUMP);
			m_jumpTime = 0;
			int halfTime = kJumpTime * 0.5f;

			m_jumpPosY = m_pos.y;

			if (m_physics.GetIsGround())
			{
				DrawingManager::Instance().CallAnimation(m_name, "Armature|Idle", 1);
				DrawingManager::Instance().CallTransAnimation(m_name, "Armature|Jump_Up", halfTime);
			}
			else
			{
				DrawingManager::Instance().CallAnimation(m_name, "Armature|Idle", 1);
				DrawingManager::Instance().CallTransAnimation(m_name, "Armature|Jump_Sec", halfTime);
			}
			DrawingManager::Instance().CallTransAnimation(m_name, "Armature|Jump_Idle", 1);

			m_physics.SetJumpFrag();
			//マジックナンバー
			// m_physics.SetJumpVec(VGet(0, 10.0f, 0));
		}

		if (m_state->GetState() == State::JUMP)
		{
			m_physics.SetJumpVec(kJumpHeight, m_jumpTime, kJumpTime);
			if (m_jumpTime < kJumpTime)
			{
				m_jumpTime++;
			}

			if (m_physics.GetIsToGround())
			{
				std::string name = "Armature|Jump_Idle";
				auto animationName = DrawingManager::Instance().GetPlayingAnimationName(m_name);

				/// この辺が問題
				if (m_jumpTime != 0)
				{
					DrawingManager::Instance().CallAnimation(m_name, "Armature|Jump_Land", 5);
					// DrawingManager::Instance().CallTransAnimation(m_name, "Armature|Idle", 1);
					ChangeState(State::MOVE);
					Controller::Instance().ResetLastInputButton();
				}
			}
			m_isSkyBaff = true;
			if (m_attackCount < 2)
				m_attackCount = 2;
		}
	}
	// 空中攻撃バフを切っておく
	if (m_state->GetState() != State::JUMP && m_attackCount != 3)
	{
		m_isSkyBaff = false;
	}
}

void Player::AttackUpdate()
{
	// 事前処理
	// 最大攻撃回数の設定
	if (m_weapon->kind == Weapon::NONE)
	{
		m_attackCountMax = 0;
	}
	else if (m_weapon->kind == Weapon::SWORD)
	{
		m_attackCountMax = kSwordAttackMax;
	}
	else if (m_weapon->kind == Weapon::AXE)
	{
		m_attackCountMax = kAxeAttackMax;
	}
	else if (m_weapon->kind == Weapon::MAGIC)
	{
		m_attackCountMax = kMagicAttackMax;
	}

	// 入力処理
	if (m_state->GetIsAttack())
	{
		
		if (m_attackCount < m_attackCountMax)
		{
			// ウェイト０（待機終了）
			if (m_waitTimeMax == 0)
			{
				m_weapon->ResetColScale();
				// 先行入力の確認
				if (m_inputButton == Button::X)
				{
					if (m_physics.GetIsGround() || (!m_physics.GetIsGround() && (m_pos.y - m_jumpPosY) > kJumpAttackHeight))
					{
						// 先行入力をリセット
						Controller::Instance().ResetLastInputButton();
						// 攻撃回数更新
						m_attackCount++;
						// 方向の決定
						// 近くに敵がいるならその敵の方向を向く
						if (m_isTarget)
						{
							auto vec = VSub(m_targetPos, m_pos);
							vec.y = 0;
							auto dir = VNorm(vec);
							m_physics.SetDir(dir);
						}
						m_dir = m_physics.GetDir();
						ChangeState(State::ATTACK);

						// ターゲット座標の特定
						if (m_isTarget)
						{
							m_weapon->SetTargetPos(m_targetPos);
						}

						// 魔法攻撃の座標処理
						if (m_weapon->kind == Weapon::MAGIC)
						{
							auto handle = 0;
							m_weapon->SetTargetPos(m_targetPos);
							m_weapon->SetShot(handle, m_dir, 3, m_attackCount * 1);
						}

						m_waitTimer = 0;
						// 空中時に体勢を整える
						/*if (!m_physics.GetIsGround())
						{
							DrawingManager::Instance().CallAnimation(m_name, "Armature|Idle", 1);
						}*/
						// 空中であれば滞空
						if (!m_physics.GetIsGround())
						{
							if (m_attackCount < m_attackCountMax)
								m_physics.Hover(false);
							else
								m_physics.Hover(true);
						}
					}
				}
				// 入力が違えば終了
				else
				{
					// 空中ではリセットしない
					if (m_physics.GetIsGround())
					{
						m_attackCount = 0;
						if (m_weapon->kind != Weapon::MAGIC)
						{
							for (auto& t : m_weapon->colliders)
							{
								t.isChackOther = false;
							}
						}
						ChangeState(State::MOVE);
					}
					else
					{
						ChangeState(State::JUMP);
					}
					Controller::Instance().ResetLastInputButton();
				}
			}
		}
		// 攻撃が3段目まで到達
		else
		{
			// ウェイト０（待機終了）でアイドル、移動状態に移行
			if (m_waitTimeMax == 0)
			{
				// 空中ではリセットしない
				if (m_physics.GetIsGround())
				{
					m_attackCount = 0;
					if (m_weapon->kind != Weapon::MAGIC)
					{
						for (auto& t : m_weapon->colliders)
						{
							t.isChackOther = false;
						}
					}
					ChangeState(State::MOVE);
					DrawingManager::Instance().CallAnimation(m_name, "Armature|Idle", kDodgeWaitTime);
				}
				else
				{
					ChangeState(State::JUMP);
				}
				Controller::Instance().ResetLastInputButton();
			}
		}
	}
	// 攻撃の処理
	if (m_state->GetState() == State::ATTACK)
	{

		// アニメーションや動作の処理
		switch (m_weapon->kind)
		{
		case Weapon::SWORD:
			// 攻撃の当たり判定の開始
			// １，２段目
			if (m_attackCount < 3 && m_waitTimer == kSwordHitStartTime)
			{
				for (auto& t : m_weapon->colliders)
				{
					t.isChackOther = true;
				}
			}
			// ３段目、空中攻撃
			else
			{
				if ((m_waitTimeMax == kLastSwordWaitTime && m_waitTimer == kSwordLastHitStartTime) ||
					(m_waitTimeMax == kAirSwordWaitTime && m_waitTimer == kSwordAirHitStartTime))
				{
					for (auto& t : m_weapon->colliders)
					{
						t.isChackOther = true;
					}
				}
			}


			m_weapon->ColUpdate(2.0f, kSwordWaitTime * 0.5f);
			if (m_attackCount == 1)
			{
				DrawingManager::Instance().CallAnimation(m_name, "Armature|Sword_1", kSwordWaitTime);
				m_waitTimeMax = kSwordWaitTime;
			}
			else if (m_attackCount == 2)
			{
				DrawingManager::Instance().CallAnimation(m_name, "Armature|Sword_2", kSwordWaitTime);
				m_waitTimeMax = kSwordWaitTime;
			}
			else
			{
				if (m_physics.GetIsGround())
				{
					DrawingManager::Instance().CallAnimation(m_name, "Armature|Sword_3", kLastSwordWaitTime);
					m_waitTimeMax = kLastSwordWaitTime;
				}
				else
				{
					DrawingManager::Instance().CallAnimation(m_name, "Armature|Sword_3", kAirSwordWaitTime);
					m_waitTimeMax = kAirSwordWaitTime;
				}
			}

			if (m_weapon->colliders[0].isChackOther)
			{
				if (m_weapon->kind == Weapon::SWORD || m_weapon->kind == Weapon::AXE)
				{
					m_weapon->OffTrajectoryCollision();
				}

				// 頂点
				DrawCapsule3D(m_lastWeaponPos[1], m_weapon->colliders[0].pos[1], 0.5f, 3, GetColor(123, 255, 255), GetColor(255, 255, 255), true);
				// 導線
				for (int i = 0; i < 30; i++)
				{
					float rate = i / 30.0f;
					auto posStart = VAdd(m_lastWeaponPos[0], VScale(VSub(m_lastWeaponPos[1], m_lastWeaponPos[0]), rate));
					auto posEnd = VAdd(m_weapon->colliders[0].pos[0], VScale(VSub(m_weapon->colliders[0].pos[1], m_weapon->colliders[0].pos[0]), rate));
					Collider col;
					col.SetCapsule(posStart, posEnd, -1, 0.1f, m_weapon->colliders[0].isOnDamage);
					m_weapon->colliders.push_back(col);
					DrawCapsule3D(posStart, posEnd, 0.01f, 3, GetColor(123, 255, 255), GetColor(255, 255, 255), false);
				}
			}
			m_lastWeaponPos[0] = m_weapon->colliders[0].pos[0];
			m_lastWeaponPos[1] = m_weapon->colliders[0].pos[1];
			break;
		case Weapon::AXE:

			// 攻撃の当たり判定の開始
			if ((m_attackCount < 3 && m_waitTimer == kAxeHitStartTime) ||
				(m_attackCount == 3 && m_waitTimer == kAxeLastHitStartTime))
			{
				for (auto& t : m_weapon->colliders)
				{
					t.isChackOther = true;
				}
			}

			m_weapon->ColUpdate(2.0f, kAxeWaitTime * 0.5f);
			// アニメーションの処理
			if (m_attackCount == 1)
			{

				DrawingManager::Instance().CallAnimation(m_name, "Armature|Axe_1", kAxeWaitTime);
				m_waitTimeMax = kAxeWaitTime;
			}

			// 導線表示
			if (m_weapon->colliders[0].isChackOther)
			{
				// DrawLine3D(m_lastWeaponPos, m_weapon->colliders[0].pos[1], GetColor(123,255,255));
				// 頂点
				DrawCapsule3D(m_lastWeaponPos[1], m_weapon->colliders[0].pos[1], 0.5f, 3, GetColor(123, 255, 255), GetColor(255, 255, 255), true);
				// 導線
				for (int i = 0; i < 30; i++)
				{
					float rate = i / 30.0f;
					auto posStart = VAdd(m_lastWeaponPos[0], VScale(VSub(m_lastWeaponPos[1], m_lastWeaponPos[0]), rate));
					auto posEnd = VAdd(m_weapon->colliders[0].pos[0], VScale(VSub(m_weapon->colliders[0].pos[1], m_weapon->colliders[0].pos[0]), rate));
					DrawCapsule3D(posStart, posEnd, 0.01f, 3, GetColor(123, 255, 255), GetColor(255, 255, 255), false);
				}
			}
			m_lastWeaponPos[0] = m_weapon->colliders[0].pos[0];
			m_lastWeaponPos[1] = m_weapon->colliders[0].pos[1];
			break;
		case Weapon::MAGIC:

			if (m_attackCount == 1)
			{
				DrawingManager::Instance().CallAnimation(m_name, "Armature|Magic_3", kMagicWaitTime);
				m_waitTimeMax = kMagicWaitTime;
			}
			break;
		default:
			break;
		}
	}
	else
	{
		// State::Attack以外の時の後処理
		if (m_attackCount != 0 && m_physics.GetIsGround())
		{
			m_attackCount = 0;
			m_weapon->ResetColScale();
			if (m_weapon->kind != Weapon::MAGIC)
			{
				for (auto& t : m_weapon->colliders)
				{
					t.isChackOther = false;
				}
			}
		}
	}
}

void Player::KnockBackUpdate()
{
	if (m_state->GetState() == State::KNOCK)
	{
		DrawingManager::Instance().CallAnimation(m_name, "Armature|KnockBuck", kKnockTime);
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
				Controller::Instance().ResetLastInputButton();
				DrawingManager::Instance().CallAnimation(m_name, "Armature|Idle", kStanTime / 10);
			}
			else
			{
				// 通常の移動状態に
			    m_collider.isOnDamage = true;
				ChangeState(State::MOVE);
				Controller::Instance().ResetLastInputButton();
				DrawingManager::Instance().CallAnimation(m_name, "Armature|Idle", 120);
			}
			/*m_collider.isOnDamage = true;
			ChangeState(State::MOVE);
			Controller::Instance().ResetLastInputButton();
			DrawingManager::Instance().CallAnimation(m_name, "Armature|Idle", 1);*/
		}
	}
}

void Player::DodgeUpdate()
{
	// 回避処理
	if (m_state->GetState() == State::DODGE)
	{
		// 回避動作の登録
		// マジックナンバー
		m_physics.Dodge(m_parameter.GetPalameter(3) * 0.3f);
		DrawingManager::Instance().CallAnimation(m_name, "Armature|Rolling", kDodgeWaitTime);
		// 回避終了
		if (m_waitTimer == 0)
		{
			ChangeState(State::MOVE);
			Controller::Instance().ResetLastInputButton();
			DrawingManager::Instance().CallAnimation(m_name, "Armature|Idle", 1);
		}
	}
	// 入力処理
	if (m_state->GetIsDodge())
	{
		if (m_waitTimer == 0)
		{
			if (m_inputButton == Button::B)
			{
				// 先行入力をリセットして回避開始
				Controller::Instance().ResetLastInputButton();
				ChangeState(State::DODGE);
				m_waitTimeMax = kDodgeWaitTime;
			}
		}
	}
	
}

void Player::StanUpdate()
{
	if (m_state->GetState() == State::STAN)
	{
		// スタン終了
		if (m_waitTimer == kStanTime)
		{
			// 通常の移動状態に
			ChangeState(State::MOVE);
			Controller::Instance().ResetLastInputButton();
			DrawingManager::Instance().CallAnimation(m_name, "Armature|Idle", 1);
		}
	}
}

void Player::ApplyMoving()
{
	m_dir = m_physics.GetDir();
	m_updateVec = m_physics.GetUpdateVec();
	// auto vec = m_physics.GetUpdateVec();
	// 座標反映
	m_pos = VAdd(m_pos, m_updateVec);
	m_collider.ColliderUpdate(m_updateVec);
}

void Player::PrecedInputButton()
{
	// auto state = m_state->GetState();

	// 各Stateでボタンを使い分け
	if (m_state->GetIsAttack())
	{
		Controller::Instance().GetInputDown(Button::X);
	}
	if (m_state->GetIsDodge())
	{
		Controller::Instance().GetInputDown(Button::B);
	}
	if (m_state->GetIsJump())
	{
		Controller::Instance().GetInputDown(Button::A);
	}

	// 入力したボタンの反映
	m_inputButton = Controller::Instance().GetLastInputButton();
}
