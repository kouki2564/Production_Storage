#include "DebugScene.h"
#include "Camera.h"
// プレイヤー
#include "Player.h"
// ボス（ミュータント）
#include "Mutant.h"
// 通常敵情報
#include "EnemyBase.h"
#include "EnemyStandard.h"
#include "EnemyMagic.h"
#include "EnemyTank.h"

#include "Stage.h"
#include "ChackCollision.h"

namespace
{
	// constexpr int kSetEnemyNum = 3;
}

DebugScene::DebugScene()
{
	
}

DebugScene::~DebugScene()
{
	// ステージの削除
	for (auto& t : m_stage)
	{
		t.second.reset();
	}
	m_stage.clear();

	// オブジェクトを全て削除
	// プレイヤーの描画モデルの削除
	DrawingManager::Instance().DeleteModel("Player");
	// 削除前に中身の解放
	for (auto& t : m_objects)
	{
		t.second.reset();
	}

	m_objects.clear();
	std::map<std::string, std::shared_ptr<ObjectBase>>(m_objects).swap(m_objects);
}

void DebugScene::InitScene()
{
	m_camera.Init(VGet(0, 30, -50));

	auto stageData = std::make_shared<Stage>();
	stageData->SetStageHandle(MV1LoadModel("Data/Model/DebugStage.mv1"));
	m_stage.insert(std::make_pair("Stage", stageData));
	// m_stage["Stage"]->InitStage("Stage", VGet(0, 0, 0), VGet(0, 0, 1));

	// プレイヤー情報
	m_objects.insert(std::make_pair("Player", std::make_shared<Player>()));
	// m_objects["Player"]->Init("Player", VGet(0, 10, 0), VGet(0, 0, 1));

	// ボス（ミュータント）情報
	m_objects.insert(std::make_pair("Mutant", std::make_shared<Mutant>()));
	// m_objects["Mutant"]->Init("Mutant", VGet(0, 10, 200), VGet(0, 0, -1));

	m_camera.ChangeViewMode(ViewMode::QUARTER,m_objects["Player"]->GetPos(), m_objects["Player"]->GetDir());
	m_camera.SetTargetPos(m_objects["Player"]->GetPos(), m_objects["Mutant"]->GetCollider().pos[1]);

	m_light.Init(m_camera.GetPos(), m_camera.GetTargetPos());
}

void DebugScene::UpdateScene()
{
	// 各オブジェクトの入力や行動の登録
	ObjectsUpdate();

	// 行動結果による当たり判定を測定
	CollisionUpdate();

	// 当たり判定を加味した上での最終行動結果の登録
	for (auto& t : m_objects)
	{
		t.second->ApplyUpdating();
	}

	// カメラの更新
	CameraUpdate();

	// 描画
	DrawingManager::Instance().Draw();
}

void DebugScene::InitImage()
{
}

void DebugScene::ObjectsUpdate()
{
	/* インプット処理、各オブジェクトの行動決定パート */
	for (auto& t1 : m_objects)
	{
		// プレイヤーのとき
		if (t1.first == "Player")
		{
			auto pos = VZero();
			/// 基準距離
			float enemyRange = 1000.0f;

			// 最接近の敵を検索
			for (auto& t2 : m_objects)
			{
				auto vec = VSub(t2.second->GetCollider().pos[0], t1.second->GetCollider().pos[0]);
				if (t2.first != "Player" && VSize(vec) <= enemyRange)
				{
					enemyRange = VSize(vec);
					pos = t2.second->GetCollider().pos[0];
				}
			}
			t1.second->SetChaseTarget(pos);
		}
		// プレイヤー以外の時
		else
		{
			t1.second->SetChaseTarget(m_objects["Player"]->GetPos());
		}
		t1.second->Update();
	}
}



void DebugScene::CollisionUpdate()
{
	/* 当たり判定のチェック、各数値決定パート */

	// コライダーを獲得して
	// コライダー同士による押し出し量の決定
	for (auto& t1 : m_objects)
	{
		// 押し出されるオブジェクト
		auto colA = t1.second->GetCollider();
		for (auto& t2 : m_objects)
		{
			// 比較対象が同じものでないことの確認
			if (t1 != t2)
			{
				// 押し出すオブジェクト
				auto colB = t2.second->GetCollider();

				// ダメージ判定処理
				// t1のオブジェクトにt2のコライダー情報を渡して
				// t1の中でHIT処理してもらう
				if (t1.second->GetIsHitAttack(colB))
				{
					// t1から取得したダメージをt2に付与
					// 敵同士の攻撃の当たり判定はなし
					if (t1.first == "Player" || (t1.first != "Player" && t2.first == "Player"))
					{
						auto damage = t1.second->GetDamagePoint();
						if (t2.second->SetDamagePoint(damage, t1.second->GetIsStanFrag(), t1.second->GetIsPowerKnock()))
						{
							if (t1.first != "Player")
							{
								m_camera.SetVibration(10);
							}
						}
					}
					// 当たった際のエフェクトやらなんとやらの処理
					// m_pSoundManager->OnePlay("SE_Boss_Hit");
					// m_hitStopCount = m_pPlayer->GetDamage() / 100;
					// m_pEffectManager->PlayEffect("Hit", m_pBoss->GetPos(), m_pBoss->GetDir());
				}

				// 押し出し処理
				auto VecZero = VZero();
				auto pushVec = VZero();
				if (colB.prefer > 0)
				{
					pushVec = ChackCollision::Instance().GetPushVec(colA, colB, VecZero);
				}
				t1.second->SetPushVec(pushVec);
			}
		}

		// マップとの判定
		for (auto& t2 : m_stage)
		{
			colA = t1.second->GetCollider();
			auto colB = t2.second->GetCollider();
			auto colAPreMoveVec = t1.second->GetSemiUpdateVec();
			auto pushVec = ChackCollision::Instance().GetPushVec(colA, colB, colAPreMoveVec);

			t1.second->SetFloorPushVec(pushVec);

			// 次のフレームで床にあたる判定の獲得
			t1.second->SetIsToGround(ChackCollision::Instance().GetIsToGround(colA, colB, colAPreMoveVec));

			// 押し出しがない（マップに接していない）ときに
			// 次フレームでの接触を確認する
		}
	}

	
}

void DebugScene::CameraUpdate()
{
	// カメラ関係
	if (Controller::Instance().GetInputDown(Button::RB))
	{
		m_camera.ChangeIsTarget();
	}

	if (Controller::Instance().GetInputDown(Button::LB))
	{
		if (m_camera.GetViewMode() == ViewMode::UP)
		{
			m_camera.ChangeViewMode(ViewMode::QUARTER, m_objects["Player"]->GetPos(), m_objects["Player"]->GetDir());
		}
		else if (m_camera.GetViewMode() == ViewMode::QUARTER)
		{
			m_camera.ChangeViewMode(ViewMode::TPS, m_objects["Player"]->GetPos(), m_objects["Player"]->GetDir());
		}
		else if (m_camera.GetViewMode() == ViewMode::TPS)
		{
			m_camera.ChangeViewMode(ViewMode::UP, m_objects["Player"]->GetPos(), m_objects["Player"]->GetDir());
		}

#ifdef _DEBUG

		// コンソール表示
		if (m_camera.GetViewMode() == ViewMode::UP)
		{
			// DrawFormatStringF(GameData::kScreenWidth * 0.5f, 10, 0xffffff, "UP");
			printf("\nChangeCameraMode：UP\n");
		}
		else if (m_camera.GetViewMode() == ViewMode::QUARTER)
		{
			// DrawFormatStringF(GameData::kScreenWidth * 0.5f, 10, 0xffffff, "QUARTER");
			printf("\nChangeCameraMode：QUARTER\n");
		}
		else if (m_camera.GetViewMode() == ViewMode::TPS)
		{
			// DrawFormatStringF(GameData::kScreenWidth * 0.5f, 10, 0xffffff, "TPS");
			printf("\nChangeCameraMode：TPS\n");
		}

#endif // _DEBUG

	}
	m_camera.SetTargetPos(m_objects["Player"]->GetPos(), m_objects["Mutant"]->GetCollider().pos[1]);
	m_objects["Player"]->SetCameraDir(m_camera.GetCameraDir());


	m_camera.Update();
}
