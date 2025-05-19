#include "TutorialScene.h"
#include <string>
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
#include "EnemyFly.h"

#include "Stage.h"
#include "ChackCollision.h"

// SceneState
#include "SceneStart.h"
#include "SceneMain.h"
#include "SceneEnd.h"
#include "ScenePause.h"
#include "PauseMenu.h"

#include "HandleManager.h"
#include "EffectManager.h"
#include "SoundManager.h"
#include "PanelManager.h"


namespace
{
	// 敵の最大配置数
	constexpr int kEnemyMaxNum = 3;

	// 表示ステータスの位置
	constexpr float kStatusPosX = GameData::kScreenWidth * 0.155f;
	constexpr float kStatusPosY = GameData::kScreenHeight * 0.13f;

	constexpr float kBarPosX = GameData::kScreenWidth * 0.1714f;
	constexpr float kBarPosY = GameData::kScreenHeight * 0.176f;

	// 表示ステータスのスケール
	constexpr float kStatusScale = 0.85f;

	/* チュートリアル表示関連 */
	// チュートリアルウインドウのＹ軸最大サイズ
	constexpr float kTutorialWindowMaxScaleY = 0.8f;
	// チュートリアルウインドウの展開量
	constexpr float kTutorialWindowDeltaY = kTutorialWindowMaxScaleY / 20;
	// チュートリアルの説明表示の上昇α値
	constexpr int kTutorialDeltaAlpha =  255 / 40;
	// チュートリアルの説明表示位置
	constexpr float kTutorialPosX = GameData::kScreenHalfWidth;
	constexpr float kTutorialPosY = GameData::kScreenHeight * 0.85f;
	// チュートリアルの説明表示のスケール
	constexpr float kTutorialScale = 1.0f;
	// サクセス表示のスケール増加値
	constexpr float kSucsessDeltaScale = 0.01f;
	// サクセス表示のスケール増加率
	constexpr float kSucsessScalePow = 1.2f;
	// サクセス表示のスケール最大値
	constexpr float kSucsessMaxScale = 1.0f;
	// サクセス表示の上昇アルファ値
	constexpr int kSucsessDeltaAlpha = 255 / 30;

	// フェードアウト用
	constexpr int kEndingTime = 30;
	constexpr int kEndingMaxAlpha = 100;
	// ステージの範囲
	constexpr int kStageRange = 140;
	// ステージ移動ライン
	constexpr int kStageMoveLine = 130;
}

TutorialScene::TutorialScene()
{
	SoundManager::Instance().loopPlay("Stage");

	m_sceneState = std::make_unique<SceneStart>();
	m_pPause = std::make_shared<PauseMenu>();

	// ステージ移動フラグをfalseに初期化
	for (int i = 0; i < GameData::kTutorialMaxLine; i++)
	{
		m_isMoveMap[i] = false;
	}
	m_nextPlayerPos = VGet(0, 10, -100);
	m_nextPlayerDir = VGet(0, 0, 1);

	InitImage();

	m_stage.insert(std::make_pair("Stage", std::make_unique<Stage>()));
	m_stage["Stage"]->InitStage("Stage", HandleManager::Instance().GetModelHandle(ModelName::NORMALSTAGE));
	m_stage.insert(std::make_pair("Boss", std::make_unique<Stage>()));
	m_stage["Boss"]->InitStage("Boss", HandleManager::Instance().GetModelHandle(ModelName::BOSSSTAGE));
	m_stage["Boss"]->SetIsUse(false);

	EffectManager::Instance().RegisterEffect("Floor", HandleManager::Instance().GetEffectHandle(EffectName::FLOOR), VZero(), VGet(0, 0, -1));
	EffectManager::Instance().RegisterEffect("Sky", HandleManager::Instance().GetEffectHandle(EffectName::SKY), VZero(), VZero());
	EffectManager::Instance().RegisterEffect("Smoke", HandleManager::Instance().GetEffectHandle(EffectName::SMOKE), VZero(), VZero());

	EffectManager::Instance().PlayEffect("Floor", VZero(), VGet(0, 0, -1));
	EffectManager::Instance().PlayEffect("Sky", VZero(), VGet(0, 0, -1));
	EffectManager::Instance().PlayEffect("Smoke", VZero(), VZero());
}

TutorialScene::~TutorialScene()
{
	PanelManager::Instance().DeleteAllImage();

	m_enemyData.clear();
	m_pPause.reset();
	m_sceneState.reset();

	for (auto& i : m_stage)
	{
		i.second.reset();
		i.second.release();
	}
	m_stage.clear();

	for (auto& i : m_objects)
	{
		i.second.reset();
		i.second.release();
	}
	m_objects.clear();

	EffectManager::Instance().DeleteEffect("Floor");
	EffectManager::Instance().DeleteEffect("Sky");
	EffectManager::Instance().DeleteEffect("Smoke");

	DrawingManager::Instance().DeleteAllModel();
	SoundManager::Instance().Stop("Stage");
	SoundManager::Instance().Stop("Boss");
}

void TutorialScene::InitScene()
{
	// カメラの初期化
	m_camera.Init(VGet(0, 80, -180));

	// ステージ番号の初期化
	m_nowStageNum[0] = 5;
	m_nowStageNum[1] = 0;

	// プレイヤー情報
	m_objects.insert(std::make_pair("Player", std::make_unique<Player>()));
	m_objects["Player"]->Init("Player", m_nextPlayerPos, m_nextPlayerDir);
	m_objects["Player"]->SetWeapon(m_weaponSelect);
	m_objects["Player"]->SetTutorialMode(true);

	// ステージ情報
	InitStageData(m_nowStageNum[0], m_nowStageNum[1]);

	// 敵の準備
	InitEnemyData(m_nowStageNum[0], m_nowStageNum[1]);
	SetEnemyData(m_nowStageNum[0], m_nowStageNum[1]);

	m_camera.ChangeViewMode(ViewMode::QUARTER, m_objects["Player"]->GetPos(), m_objects["Player"]->GetDir());
	// m_camera.SetTargetPos(m_objects["Player"]->GetPos(), m_objects["Mutant"]->GetCollider().pos[1]);
	m_camera.SetIsCameraRotate(false);

	m_light.Init(m_camera.GetPos(), m_camera.GetTargetPos());
}

void TutorialScene::UpdateScene()
{
	// 主操作
	if (m_sceneState->isMove)
	{
		// 勝敗確定時
		if (m_isWinEnd || m_isLoseEnd)
		{
			EndingUpdate();
		}
		// 未確定でゲーム進行時
		else
		{
			// ポーズ画面への移行
			if (Controller::Instance().GetInputDown(Button::START))
			{
				ChangeState(SceneState::PAUSE);

				UpgradeData data;
				data.upgradePoint = m_upgradePoint;
				data.attackLevel = m_objects["Player"]->GetLevel().Atk;
				data.defenseLevel = m_objects["Player"]->GetLevel().Def;
				data.speedLevel = m_objects["Player"]->GetLevel().Agi;
				// ポーズ画面側に現在の強化ポイント情報を渡す
				m_pPause->SetUpgradePoint(data);

				// ポーズ画面展開
				m_pPause->SwitchPause();
			}
			else
			{
				// マップ移動処理
				ChackMoveMap();

				// 各オブジェクトの入力や行動の登録
				ObjectsUpdate();

				// 行動結果による当たり判定を測定
				CollisionUpdate();

				// 当たり判定を加味した上での最終行動結果の登録
				for (auto it = m_objects.begin(); it != m_objects.end(); )
				{
					auto& t = *it;
					t.second->ApplyUpdating();

					// カメラにプレイヤーの移動量の設定
					if (t.first == "Player")
					{
						m_camera.SetTargetMove(t.second->GetResVec());
					}

					if (t.second->GetIsDelete())
					{
						if (t.first == "Player")
						{
							// プレイヤーの削除
							m_isLoseEnd = true;
							++it;
						}
						else if (t.first == "Mutant")
						{
							// ボスの削除
							m_isWinEnd = true;
							++it;
						}
						else
						{
							// 敵の削除
							DrawingManager::Instance().DeleteModel(t.first);
							m_enemyData.pop_back();
							t.second.reset();
							it = m_objects.erase(it);
						}
					}
					else
					{
						++it;
					}
				}

				// 敵の残存数確認
				//if (m_enemyData.size() <= 0)
				//{
				//	// ステージクリア時だけポイント加算
				//	if (!m_isMoveMap[m_nowStageNum[0]][m_nowStageNum[1]] &&
				//		!(m_nowStageNum[0] == 0 && m_nowStageNum[1] == 1))
				//	{
				//		m_isMoveMap[m_nowStageNum[0]][m_nowStageNum[1]] = true;
				//		m_upgradePoint++;
				//		m_tutorialNum = 6;
				//		m_tutorialTimer = 0;
				//		m_isTutorialPop = true;
				//	}
				//}
				
				// チュートリアルの更新
				TutorialUpdate();
			}
		}
	}

	// ポーズ画面
	m_pPause->Update();



	if (m_sceneState->isPause)
	{
		// ポーズ中の処理// ポーズ画面への移行
		if (Controller::Instance().GetInputDown(Button::START))
		{
			m_pPause->SwitchPause();
		}
		// ポーズ画面の終了処理
		if (m_pPause->GetIsEndPause())
		{
			auto data = m_pPause->GetUpgradePoint();
			m_upgradePoint = data.upgradePoint;
			Level level = { data.attackLevel, data.defenseLevel, data.speedLevel };
			m_objects["Player"]->SetLevel(level);
			ChangeState(SceneState::MAIN);
		}
		if (m_pPause->GetIsEndGame())
		{
			m_nextScene = Scene::TITLE;
			m_isMoveScene = true;
		}
		// パラメータの強化値を取得してプレイヤーに渡す
		int upgrade = m_pPause->GetUpgrade();
		if (upgrade != -1)
		{
			// 強化ポイントを減らしてプレイヤーを強化
			m_upgradePoint--;
			m_objects["Player"]->SetUpgrade(upgrade);
		}
	}
	// プレイヤーのHP表示の更新
	PlayerStatusPop();

	// カメラの更新
	CameraUpdate();
	EffectManager::Instance().Update();

	// tennzi
	/*if (Controller::Instance().GetInputDown(Button::LB) &&
		Controller::Instance().GetPOVDown(InputPOV::LEFT))
	{
		m_upgradePoint = 10;
	}*/

	// 描画
	DrawingManager::Instance().Draw();
	EffectManager::Instance().Draw();
	DrawFloor();
	PanelManager::Instance().Draw();
	DrawParameter();
	m_pPause->Draw();
	EndingDraw();

	// ゲーム進行(主にフェード処理)
	if (m_sceneState->isProgress)
	{
		if (m_sceneState->GetState() == SceneState::START)
		{
			//if (/* Init処理の未完了時 */)
			//{
			//	// なんか処理
			//}
			if (FadeIn(5))
			{
				ChangeState(SceneState::MAIN);
			}
		}
		else if (m_sceneState->GetState() == SceneState::END)
		{
			// ステージ更新処理等

			// フェードアウト後
			if (FadeOut(5))
			{
				// チュートリアル文の非表示化(ボスマップはそのまま)
				if (m_nowStageNum[0] != 0)
				{
					m_tutorialWindowScaleY = 0;
					PanelManager::Instance().UpdateImageScale("TutorialWindow", kTutorialScale, m_tutorialWindowScaleY);
					PanelManager::Instance().SetIsDrawImage("Tutorial_" + std::to_string(m_nowStageNum[0]), false);
					m_tutorialAlpha = 0;
					// サクセスの非表示化とスケールの初期化
					PanelManager::Instance().SetIsDrawImage("Tutorial_Success", false);
					m_sucsessScale = 0.0f;
					m_sucsessAlpha = 0;
					PanelManager::Instance().UpdateImageScale("Tutorial_Success", m_sucsessScale, m_sucsessScale);
				}

				// 最終ステージ時
				if (m_isLoseEnd || m_isWinEnd)
				{
					// エンディング移行
					m_nextScene = Scene::GAME;
					m_isMoveScene = true;
					return;
				}
				// それ以外
				else
				{
					// 各方向への部屋移動
					if (m_moveRoomDir == UP)
					{
						m_nextPlayerPos = VGet(0, 0, -120);
						m_nextPlayerDir = VGet(0, 0, 1);
						m_nowStageNum[0]--;
						m_moveRoomDir = -1;
					}
					else if (m_moveRoomDir == DOWN)
					{
						m_nextPlayerPos = VGet(0, 0, 120);
						m_nextPlayerDir = VGet(0, 0, -1);
						m_nowStageNum[0]++;
						m_moveRoomDir = -1;
					}
					else if (m_moveRoomDir == RIGHT)
					{
						m_nextPlayerPos = VGet(-120, 0, 0);
						m_nextPlayerDir = VGet(1, 0, 0);
						m_nowStageNum[1]++;
						m_moveRoomDir = -1;
					}
					else if (m_moveRoomDir == LEFT)
					{
						m_nextPlayerPos = VGet(120, 0, 0);
						m_nextPlayerDir = VGet(-1, 0, 0);
						m_nowStageNum[1]--;
						m_moveRoomDir = -1;
					}

					// ステージデータの削除
					DeleteStatgeData();

					// ステージの更新
					InitStageData(m_nowStageNum[0], m_nowStageNum[1]);

					// プレイヤーの準備
					m_objects["Player"]->SetPosAndDir(m_nextPlayerPos, m_nextPlayerDir);

					// 敵の準備
					if (!m_isMoveMap[m_nowStageNum[0]])
					{
						InitEnemyData(m_nowStageNum[0], m_nowStageNum[1]);
						SetEnemyData(m_nowStageNum[0], m_nowStageNum[1]);
					}

					ChangeState(SceneState::START);
				}
			}
		}
	}
	m_light.Update(m_camera.GetPos(), m_camera.GetTargetPos());
}

void TutorialScene::UpdateStage()
{
}

void TutorialScene::InitImage()
{
	// 表示ステータスの画像登録
	PanelManager::Instance().RegisterImage("BarBack", HandleManager::Instance().GetImageHandle(ImageName::BARBACK), kBarPosX, kBarPosY, kStatusScale, kStatusScale);
	PanelManager::Instance().RegisterImage("BarGreen", HandleManager::Instance().GetImageHandle(ImageName::BARGREEN), kBarPosX, kBarPosY, kStatusScale, kStatusScale);
	PanelManager::Instance().RegisterImage("BarRed", HandleManager::Instance().GetImageHandle(ImageName::BARRED), kBarPosX, kBarPosY, kStatusScale, kStatusScale);
	PanelManager::Instance().RegisterImage("Status", HandleManager::Instance().GetImageHandle(ImageName::STATUS), kStatusPosX, kStatusPosY, kStatusScale, kStatusScale);

	PanelManager::Instance().SetIsDrawImage("BarRed", false);

	// チュートリアルの画像登録
	PanelManager::Instance().RegisterImage("TutorialWindow", HandleManager::Instance().GetImageHandle(ImageName::TUTORIAL_WINDOW), kTutorialPosX, kTutorialPosY, kTutorialScale, m_tutorialWindowScaleY);
	PanelManager::Instance().SetIsDrawImage("TutorialWindow", true);
	PanelManager::Instance().RegisterImage("Tutorial_01", HandleManager::Instance().GetImageHandle(ImageName::TUTORIAL_TARGET), kTutorialPosX, kTutorialPosY, kTutorialScale, kTutorialScale);
	PanelManager::Instance().SetIsDrawImage("Tutorial_01", false);
	PanelManager::Instance().RegisterImage("Tutorial_02", HandleManager::Instance().GetImageHandle(ImageName::TUTORIAL_BOSS), kTutorialPosX, kTutorialPosY, kTutorialScale, kTutorialScale);
	PanelManager::Instance().SetIsDrawImage("Tutorial_02", false);
	PanelManager::Instance().RegisterImage("Tutorial_1", HandleManager::Instance().GetImageHandle(ImageName::TUTORIAL_AVOIDANCE), kTutorialPosX, kTutorialPosY, kTutorialScale, kTutorialScale);
	PanelManager::Instance().SetIsDrawImage("Tutorial_1", false);
	PanelManager::Instance().RegisterImage("Tutorial_2", HandleManager::Instance().GetImageHandle(ImageName::TUTORIAL_JUMPATTACK), kTutorialPosX, kTutorialPosY, kTutorialScale, kTutorialScale);
	PanelManager::Instance().SetIsDrawImage("Tutorial_2", false);
	PanelManager::Instance().RegisterImage("Tutorial_3", HandleManager::Instance().GetImageHandle(ImageName::TUTORIAL_JUMP), kTutorialPosX, kTutorialPosY, kTutorialScale, kTutorialScale);
	PanelManager::Instance().SetIsDrawImage("Tutorial_3", false);
	PanelManager::Instance().RegisterImage("Tutorial_4", HandleManager::Instance().GetImageHandle(ImageName::TUTORIAL_ATTACK), kTutorialPosX, kTutorialPosY, kTutorialScale, kTutorialScale);
	PanelManager::Instance().SetIsDrawImage("Tutorial_4", false);
	PanelManager::Instance().RegisterImage("Tutorial_5", HandleManager::Instance().GetImageHandle(ImageName::TUTORIAL_MOVE), kTutorialPosX, kTutorialPosY, kTutorialScale, kTutorialScale);
	PanelManager::Instance().SetIsDrawImage("Tutorial_5", false);

	// サクセス画像
	PanelManager::Instance().RegisterImage("Tutorial_Success", HandleManager::Instance().GetImageHandle(ImageName::TUTORIAL_SUCCESS), kTutorialPosX, kTutorialPosY, m_sucsessScale, m_sucsessScale);
	PanelManager::Instance().SetIsDrawImage("Tutorial_Success", false);
}

void TutorialScene::ObjectsUpdate()
{
	/* インプット処理、各オブジェクトの行動決定パート */
	for (auto it = m_objects.begin(); it != m_objects.end(); )
	{
		auto& t1 = *it;
		// プレイヤーのとき
		if (t1.first == "Player")
		{
			auto pos = VZero();
			/// 基準距離
			float enemyRange = 100.0f;

			// 最接近の敵を検索
			for (auto its = m_objects.begin(); its != m_objects.end(); ++its)
			{
				auto& t2 = *its;
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
			if (m_nowStageNum[0] != 1)
			{
				t1.second->SetChaseTarget(m_objects["Player"]->GetCollider().centerPos);
			}
			// 回避ステージのみターゲット座標固定
			else
			{
				auto pos = VGet(0, 0, (t1.second->GetPos().z * 0.3f));
				t1.second->SetChaseTarget(pos);
			}
		}

		// 各オブジェクトの行動
		t1.second->Update();

		// 死んだキャラクターの削除
		if (t1.second->GetIsDelete())
		{
			// ゲームの勝敗判定
			if (t1.first == "Player")
			{
				m_isLoseEnd = true;
				++it;
			}
			else if (t1.first == "Mutant")
			{
				m_isWinEnd = true;
				++it;
			}
			else
			{
				// 描画モデルの削除
				DrawingManager::Instance().DeleteModel(t1.first);
				m_enemyData.pop_back();
				it = m_objects.erase(it);
			}
		}
		else
		{
			++it;
		}
	}
}

void TutorialScene::CollisionUpdate()
{
	/* 当たり判定のチェック、各数値決定パート */

	// コライダーを獲得して
	// コライダー同士による押し出し量の決定
	for (auto it1 = m_objects.begin(); it1 != m_objects.end(); )
	{
		auto& t1 = *it1;
		// 押し出されるオブジェクト
		auto colA = t1.second->GetCollider();
		for (auto it2 = m_objects.begin(); it2 != m_objects.end(); ++it2)
		{
			auto& t2 = *it2;
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
						// 回避ステージでの回避成功判定
						else if (m_nowStageNum[0] == 1)
						{
							m_isClearDodge = true;
						}
					}
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
		for (auto it2 = m_stage.begin(); it2 != m_stage.end(); ++it2)
		{
			auto& t2 = *it2;
			// 押し出しがない（マップに接していない）ときに
			// 次フレームでの接触を確認する

			if (m_nowStageNum[0] == 0 && t2.first == "Stage")
			{
				continue;
			}

			// 壁当たり判定(応急処置)
			auto move = VZero();
			// 通常ステージ
			if (t2.first == "Stage")
			{
				auto pos = t1.second->GetPos();
				// 壁
				if (pos.x < -kStageMoveLine)
				{
					move.x = -kStageMoveLine - pos.x;
				}
				if (pos.x > kStageMoveLine)
				{
					move.x = kStageMoveLine - pos.x;
				}
				if (pos.z < -kStageMoveLine)
				{
					move.z = -kStageMoveLine - pos.z;
				}
				if (pos.z > kStageMoveLine)
				{
					move.z = kStageMoveLine - pos.z;
				}
			}
			// ボスステージ
			else
			{
				auto pos = t1.second->GetPos();
				// 壁
				if (pos.x < -160)
				{
					move.x = -160 - pos.x;
				}
				if (pos.x > 160)
				{
					move.x = 160 - pos.x;
				}
				if (pos.z < -200)
				{
					move.z = -200 - pos.z;
				}
				if (pos.z > 200)
				{
					move.z = 200 - pos.z;
				}
			}

			colA = t1.second->GetCollider();
			auto colB = t2.second->GetCollider();
			auto colAPreMoveVec = t1.second->GetSemiUpdateVec();
			auto pushVec = VAdd(ChackCollision::Instance().GetPushVec(colA, colB, colAPreMoveVec), move);
			t1.second->SetFloorPushVec(pushVec);
			// 次のフレームで床にあたる判定の獲得
			t1.second->SetIsToGround(ChackCollision::Instance().GetIsToGround(colA, colB, colAPreMoveVec));
		}


		// 復帰処理
		if (t1.second->GetPos().y < -50)
		{
			t1.second->SetPosAndDir(m_nextPlayerPos, m_nextPlayerDir);
		}

		++it1;
	}
}

void TutorialScene::TutorialUpdate()
{
	/* プレイヤー行動関係 */

	// チュートリアル処理
	if (!m_isMoveMap[m_nowStageNum[0]])
	{
		switch (m_nowStageNum[0])
		{
		case 0:		// ボス戦の間
			if (m_isWinEnd)
			{
				m_isMoveMap[m_nowStageNum[0]] = true;
				// サクセス表示開始
				PanelManager::Instance().SetIsDrawImage("Tutorial_Success", true);
			}
			break;
		case 1:		// 回避の間
			if (m_isClearDodge)
			{
				m_isMoveMap[m_nowStageNum[0]] = true;
				// サクセス表示開始
				PanelManager::Instance().SetIsDrawImage("Tutorial_Success", true);
			}
			break;
		case 2:		// 空中攻撃の間
			if (m_enemyData.size() <= 0)
			{
				m_isMoveMap[m_nowStageNum[0]] = true;
				// サクセス表示開始
				PanelManager::Instance().SetIsDrawImage("Tutorial_Success", true);
			}
			break;
		case 3:		// ジャンプの間
			if (m_objects["Player"]->GetPos().z > 120)
			{
				m_isMoveMap[m_nowStageNum[0]] = true;
				// サクセス表示開始
				PanelManager::Instance().SetIsDrawImage("Tutorial_Success", true);
			}
			break;
		case 4:		// 攻撃の間
			if (m_enemyData.size() <= 0)
			{
				m_isMoveMap[m_nowStageNum[0]] = true;
				// サクセス表示開始
				PanelManager::Instance().SetIsDrawImage("Tutorial_Success", true);
			}
			break;
		case 5:		// 徒歩の間
			if (m_objects["Player"]->GetPos().z > 120)
			{
				m_isMoveMap[m_nowStageNum[0]] = true;
				// サクセス表示開始
				PanelManager::Instance().SetIsDrawImage("Tutorial_Success", true);
			}
			break;
		default:
			break;
		}
	}


	/* Draw関係 */

	// フロア開始時処理
	if (!m_isMoveMap[m_nowStageNum[0]])
	{
		// チュートリアル表示のためのウインドウ表示
		if (m_tutorialWindowScaleY < kTutorialWindowMaxScaleY)
		{
			m_tutorialWindowScaleY += kTutorialWindowDeltaY;
			PanelManager::Instance().UpdateImageScale("TutorialWindow", kTutorialScale, m_tutorialWindowScaleY);
		}
		else if (m_tutorialWindowScaleY > kTutorialWindowMaxScaleY)
		{
			m_tutorialWindowScaleY = kTutorialWindowMaxScaleY;
			PanelManager::Instance().UpdateImageScale("TutorialWindow", kTutorialScale, m_tutorialWindowScaleY);
			if (m_nowStageNum[0] != 0)
				PanelManager::Instance().SetIsDrawImage("Tutorial_" + std::to_string(m_nowStageNum[0]), true);
			else
			{
				PanelManager::Instance().SetIsDrawImage("Tutorial_01", true);
				PanelManager::Instance().SetIsDrawImage("Tutorial_02", false);
			}
		}
		if (m_isClearTarget && PanelManager::Instance().GetIsDrawImage("Tutorial_01"))
		{
			PanelManager::Instance().SetIsDrawImage("Tutorial_01", false);
			PanelManager::Instance().SetIsDrawImage("Tutorial_02", true);
		}

		// チュートリアル本文のアルファ値上昇
		if (m_nowStageNum[0] != 0)
		{
			if (PanelManager::Instance().GetIsDrawImage("Tutorial_" + std::to_string(m_nowStageNum[0])))
			{
				if (m_tutorialAlpha < 255)
				{
					m_tutorialAlpha += kTutorialDeltaAlpha;
					PanelManager::Instance().SetAlpha("Tutorial_" + std::to_string(m_nowStageNum[0]), m_tutorialAlpha);
				}
				else if (m_tutorialAlpha > 255)
				{
					m_tutorialAlpha = 255;
					PanelManager::Instance().SetAlpha("Tutorial_" + std::to_string(m_nowStageNum[0]), m_tutorialAlpha);
				}
			}
		}
		else
		{
			if (PanelManager::Instance().GetIsDrawImage("Tutorial_01"))
			{
				if (m_tutorialAlpha < 255)
				{
					m_tutorialAlpha += kTutorialDeltaAlpha;
					PanelManager::Instance().SetAlpha("Tutorial_01", m_tutorialAlpha);
				}
				else if (m_tutorialAlpha > 255)
				{
					m_tutorialAlpha = 255;
					PanelManager::Instance().SetAlpha("Tutorial_01", m_tutorialAlpha);
				}
			}
			if (PanelManager::Instance().GetIsDrawImage("Tutorial_02"))
			{
				if (m_tutorialAlpha < 255)
				{
					m_tutorialAlpha += kTutorialDeltaAlpha;
					PanelManager::Instance().SetAlpha("Tutorial_02", m_tutorialAlpha);
				}
				else if (m_tutorialAlpha > 255)
				{
					m_tutorialAlpha = 255;
					PanelManager::Instance().SetAlpha("Tutorial_02", m_tutorialAlpha);
				}
			}
		}
	}

	// クリア時のサクセス表示
	if (m_isMoveMap[m_nowStageNum[0]])
	{
		// scaleの更新
		if (m_sucsessScale < kSucsessMaxScale)
		{
			// サクセス表示の拡大
			m_sucsessScale += kSucsessDeltaScale;
			m_sucsessScale *= kSucsessScalePow;
			PanelManager::Instance().UpdateImageScale("Tutorial_Success", m_sucsessScale, m_sucsessScale);
		}
		else if (m_sucsessScale > kSucsessMaxScale)
		{
			m_sucsessScale = kSucsessMaxScale;
			PanelManager::Instance().UpdateImageScale("Tutorial_Success", m_sucsessScale, m_sucsessScale);
		}

		// サクセス表示のアルファ値の更新
		if (m_sucsessAlpha < 255)
		{
			m_sucsessAlpha += kSucsessDeltaAlpha;
			PanelManager::Instance().SetAlpha("Tutorial_Success", m_sucsessAlpha);
		}
		else if (m_sucsessAlpha > 255)
		{
			m_sucsessAlpha = 255;
			PanelManager::Instance().SetAlpha("Tutorial_Success", m_sucsessAlpha);

			/*if (m_nowStageNum[0] == 0)
			{
				if (!m_isWinEnd)
				{
					m_sucsessAlpha = 0;
					m_sucsessScale = 0;
					PanelManager::Instance().SetIsDrawImage("Tutorial_Success", false);
				}
			}*/
		}
	}
}

void TutorialScene::TutorialDraw()
{
}

void TutorialScene::CameraUpdate()
{
	// カメラ関係
	int size = static_cast<int>(m_objects.size());
	m_camera.SetTargetPos(m_objects["Player"]->GetPos(), VZero());

	if (m_nowStageNum[0] == 0)
	{
		if (Controller::Instance().GetInputDown(Button::RB))
		{
			m_camera.ChangeIsTarget();
			m_isClearTarget = true;
		}
		m_camera.SetTargetPos(m_objects["Player"]->GetPos(), m_objects["Mutant"]->GetCollider().pos[1]);
	}

	for (auto& t : m_objects)
	{
		t.second->SetCameraDir(m_camera.GetCameraDir());
	}

	m_camera.Update();
}

void TutorialScene::ChackMoveMap()
{
	// 現在マップの敵が全滅してマップ移動可能かどうかのチェック
	if (m_nowStageNum[0] != 0 &&
		m_isMoveMap[m_nowStageNum[0]])
	{
		auto pos = m_objects["Player"]->GetPos();

		// どの方向に進めるかのチェック
		if (m_nowStageNum[0] > 0)		// 奥
		{
			EffectManager::Instance().StopEffect("SistemWall_Flont");
			// プレイヤーの座標チェック
			if (abs(pos.x) < 15 && pos.z >= kStageMoveLine)
			{
				m_moveRoomDir = UP;
				ChangeState(SceneState::END);
			}
		}
		if (m_nowStageNum[0] < GameData::kTutorialMaxLine - 1)	// 手前
		{
			EffectManager::Instance().StopEffect("SistemWall_Back");
			// プレイヤーの座標チェック
			if (abs(pos.x) < 15 && pos.z <= -kStageMoveLine)
			{
				m_moveRoomDir = DOWN;
				ChangeState(SceneState::END);
			}
		}
	}
}

void TutorialScene::DeleteStatgeData()
{
	// オブジェクトを全て削除
	// プレイヤーの描画モデルの削除
	// DrawingManager::Instance().DeleteModel("Player");

	// オブジェクトの削除
	// m_objects.clear();
	// std::map<std::string, std::shared_ptr<ObjectBase>>().swap(m_objects);
	// プレイヤー以外のオブジェクトの削除
	for (auto it = m_objects.begin(); it != m_objects.end(); )
	{
		auto& t = *it;
		if (t.first != "Player")
		{
			// 描画モデルの削除
			DrawingManager::Instance().DeleteModel(t.first);
			it = m_objects.erase(it);
		}
		else
		{
			++it;
		}
	}


	m_setEnemyNum = 0;
}

void TutorialScene::InitStageData(int stageLine, int stageRow)
{
	// 前フロアで消去した床を復活
	for (auto t : m_invisibleFrame)
	{
		int frame = t * 2;
		DrawingManager::Instance().ChangePopModelFrame("Stage", frame, true);
	}
	// 消去するフレームのリセット
	m_invisibleFrame.clear();
	m_stage["Stage"]->ResetNoUseFrame();

	// ステージの準備
	switch (stageLine)
	{
	case 0:
		SoundManager::Instance().Stop("Stage");
		SoundManager::Instance().loopPlay("Boss");
		m_stage["Stage"]->SetIsUse(false);
		m_stage["Boss"]->SetIsUse(true);
		DrawingManager::Instance().SetIsDrawModel("Boss", true);
		DrawingManager::Instance().SetIsDrawModel("Stage", false);
		// bossmap
		break;
	case 1:
		m_stage["Stage"]->SetIsUse(true);
		m_stage["Boss"]->SetIsUse(false);
		m_invisibleFrame = {    48,47,   45,44,   
							    41,40,   38,37,   
							    34,33,   31,30,   
							    27,26,   24,23,   
							    20,19,   17,16,   
							    13,12,   10, 9,   
							     6, 5,    3, 2,   
		};
		DrawingManager::Instance().SetStageDoor(false , true, false, true);
		DrawingManager::Instance().SetIsDrawModel("Boss", false);
		DrawingManager::Instance().SetIsDrawModel("Stage", true);
		DrawingManager::Instance().SetFrameOpacityRate("Stage", 127, 0.3f);
		break; 
	case 2:
		m_stage["Stage"]->SetIsUse(true);
		m_stage["Boss"]->SetIsUse(false);
		// 床抜け無し
		DrawingManager::Instance().SetStageDoor(false, true, false, true);
		DrawingManager::Instance().SetIsDrawModel("Boss", false);
		DrawingManager::Instance().SetIsDrawModel("Stage", true);
		DrawingManager::Instance().SetFrameOpacityRate("Stage", 127, 0.3f);
		break;
	case 3:
		m_stage["Stage"]->SetIsUse(true);
		m_stage["Boss"]->SetIsUse(false);
		m_invisibleFrame = {
							 42,41,40,39,38,37,36,
							 35,34,33,32,31,30,29,


							 14,13,12,11,10, 9, 8,

		};
		DrawingManager::Instance().SetStageDoor(false, true, false, true);
		DrawingManager::Instance().SetIsDrawModel("Boss", false);
		DrawingManager::Instance().SetIsDrawModel("Stage", true);
		DrawingManager::Instance().SetFrameOpacityRate("Stage", 127, 0.3f);
		break;
	case 4:
		m_stage["Stage"]->SetIsUse(true);
		m_stage["Boss"]->SetIsUse(false);
		// 床抜け無し
		DrawingManager::Instance().SetStageDoor(false, true, false, true);
		DrawingManager::Instance().SetIsDrawModel("Boss", false);
		DrawingManager::Instance().SetIsDrawModel("Stage", true);
		DrawingManager::Instance().SetFrameOpacityRate("Stage", 127, 0.3f);
		break;
	case 5:
		m_stage["Stage"]->SetIsUse(true);
		m_stage["Boss"]->SetIsUse(false);
		m_invisibleFrame = { 49,48,47,   45,44,43,
							 42,               36,
							 35,   33,32,31,   29,
					         28,               22,
							 21,20,19,   17,16,15,
							 14,13,12,   10, 9, 8,
							  7, 6, 5,    3, 2, 1,
		};
		DrawingManager::Instance().SetStageDoor(false, true, false, false);
		DrawingManager::Instance().SetIsDrawModel("Boss", false);
		DrawingManager::Instance().SetIsDrawModel("Stage", true);
		DrawingManager::Instance().SetFrameOpacityRate("Stage", 127, 0.3f);
		break;
	}

	// いらない部分の床を消去
	for (auto t : m_invisibleFrame)
	{
		int frame = t * 2;
		DrawingManager::Instance().ChangePopModelFrame("Stage", frame, false);
		// 判定しないフレーム番号の設定
		m_stage["Stage"]->SetNoUseFrame(frame);			// 床板
		m_stage["Stage"]->SetNoUseFrame(frame + 1);		// 床の中身
	}

	// ステージ本体の準備
	// ボスステージ
	if (stageLine == 0)
	{
		// カメラ更新
		m_camera.ChangeViewMode(ViewMode::TPS, m_objects["Player"]->GetPos(), m_objects["Player"]->GetDir());
		m_camera.SetIsCameraRotate(true);
	}
}

void TutorialScene::InitEnemyData(int stageLine, int stageRow)
{
	EffectManager::Instance().StopEffect("SistemWall_Flont");
	EffectManager::Instance().StopEffect("SistemWall_Back");
	EffectManager::Instance().StopEffect("SistemWall_Left");
	EffectManager::Instance().StopEffect("SistemWall_Right");

	// 前データを削除
	m_enemyData.clear();

	// ボス部屋
	// ステージの準備
	switch (stageLine)
	{
	case 0:
		// bossmap
		break;
	case 1:
		m_enemyData.push_back(EnemyKind::FLY);
		m_enemyData.push_back(EnemyKind::FLY);
		m_enemyData.push_back(EnemyKind::FLY);
		EffectManager::Instance().PlayEffect("SistemWall_Flont", VGet(0, 0, kStageRange), VGet(1, 0, 0));
		EffectManager::Instance().PlayEffect("SistemWall_Back", VGet(0, 0, -kStageRange), VGet(-1, 0, 0));
		break;
	case 2:
		m_enemyData.push_back(EnemyKind::FLY);
		EffectManager::Instance().PlayEffect("SistemWall_Flont", VGet(0, 0, kStageRange), VGet(1, 0, 0));
		EffectManager::Instance().PlayEffect("SistemWall_Back", VGet(0, 0, -kStageRange), VGet(-1, 0, 0));

		break;
	case 3:
		EffectManager::Instance().PlayEffect("SistemWall_Flont", VGet(0, 0, kStageRange), VGet(1, 0, 0));
		EffectManager::Instance().PlayEffect("SistemWall_Back", VGet(0, 0, -kStageRange), VGet(-1, 0, 0));
		break;
	case 4:
		m_enemyData.push_back(EnemyKind::STANDARD);
		EffectManager::Instance().PlayEffect("SistemWall_Flont", VGet(0, 0, kStageRange), VGet(1, 0, 0));
		EffectManager::Instance().PlayEffect("SistemWall_Back", VGet(0, 0, -kStageRange), VGet(-1, 0, 0));
		break;
	case 5:
		EffectManager::Instance().PlayEffect("SistemWall_Flont", VGet(0, 0, kStageRange), VGet(1, 0, 0));
		break;
	}
}

void TutorialScene::SetEnemyData(int stageLine, int stageRow)
{
	auto enemyNum = m_enemyData.size();

	// ボスマップ以外
	if (stageLine != 0)
	{
		// 敵の配置
		for (int i = 0; i < kEnemyMaxNum; i++)
		{
			// エネミーの配置座標
			VECTOR enemyPos;

			switch (stageLine)
			{
			case 0:
				enemyPos = VGet(0, 10, 120);
				break;
			case 1:
				switch (i)
				{
				case 0:
					enemyPos = VGet(-125, 45, 50);
					break;
				case 1:
					enemyPos = VGet(-125, 45, 0);
					break;
				case 2:
					enemyPos = VGet(-125, 45, -50);
					break;
				default:
					enemyPos = VZero();
					break;
				}
				break;
			case 2:
				enemyPos = VGet(0, 45, 50);
				break;
			case 4:
				enemyPos = VGet(0, 5, 50);
				break;
			default:
				enemyPos = VZero();
				break;
			}

			// 配置するエネミーの情報
			if (enemyNum > i)
			{
				m_setEnemyNum++;
				std::string name = "Enemy_" + std::to_string(m_setEnemyNum);
				if (m_enemyData[i] == EnemyKind::STANDARD)
				{
					m_objects.insert(std::make_pair(name, std::make_unique<EnemyStandard>()));
				}
				else if (m_enemyData[i] == EnemyKind::FLY)
				{
					m_objects.insert(std::make_pair(name, std::make_unique<EnemyFly>()));
				}
				m_objects[name]->Init(name, enemyPos, VGet(0, 0, 1));
				m_objects[name]->SetID(m_setEnemyNum);
				// チュートリアル用に行動制限
				// m_objects[name]->SetIsLock();

			}
		}
	}
	// ボスマップ
	else
	{
		// ボスの配置
		m_objects.insert(std::make_pair("Mutant", std::make_unique<Mutant>()));
		m_objects["Mutant"]->SetTutorialMode(true);
		m_objects["Mutant"]->Init("Mutant", VGet(0, 0, 100), VGet(0, 0, 1));
	}
}

void TutorialScene::PlayerStatusPop()
{
	// プレイヤーのHP割合
	auto playerHpRate = m_objects["Player"]->GetParameter().GetHPRate();

	// 座標の読み取り用変数
	float getX = 0.0f;
	float getY = 0.0f;

	// HPバーのスケール調整
	PanelManager::Instance().UpdateImageScale("BarGreen", kStatusScale * playerHpRate, kStatusScale);
	PanelManager::Instance().UpdateImageScale("BarRed", kStatusScale * playerHpRate, kStatusScale);

	// HPバーの位置の取得
	PanelManager::Instance().GetPos("BarGreen", &getX, &getY);

	// HPバーの位置調整
	PanelManager::Instance().UpdateImagePos("BarGreen", kBarPosX - (kBarPosX * (1.0f - playerHpRate) * 0.5f), getY);
	PanelManager::Instance().UpdateImagePos("BarRed", kBarPosX - (kBarPosX * (1.0f - playerHpRate) * 0.5f), getY);

	// HPバーの色の変更
	if (playerHpRate < 0.25f)
	{
		PanelManager::Instance().SetIsDrawImage("BarRed", true);
		PanelManager::Instance().SetIsDrawImage("BarGreen", false);
	}
	else
	{
		PanelManager::Instance().SetIsDrawImage("BarRed", false);
		PanelManager::Instance().SetIsDrawImage("BarGreen", true);
	}
}

// マジックナンバーの塊
void TutorialScene::DrawParameter()
{
	// フォント変更
	HandleManager::Instance().UseFont(FontName::BOLD_20);

	auto color = GetColor(10, 10, 10);
	auto selectColor = GetColor(200, 200, 0);
	auto edgeColor = GetColor(255, 255, 255);

	// プレイヤーのレベル表記
	Level playerLevel = m_objects["Player"]->GetLevel();
	// プレイヤーの攻撃レベル表記
	auto playerAtkLevel = playerLevel.Atk;
	DrawFormatString2F(GameData::kScreenWidth * 0.06f, GameData::kScreenHeight * 0.07f, color, edgeColor, "LV.%d", playerAtkLevel);
	// プレイヤーの防御レベル表記
	auto playerDefLevel = playerLevel.Def;
	DrawFormatString2F(GameData::kScreenWidth * 0.145f, GameData::kScreenHeight * 0.07f, color, edgeColor, "LV.%d", playerDefLevel);
	// プレイヤーのスピードレベル表記
	auto playerAgiLevel = playerLevel.Agi;
	DrawFormatString2F(GameData::kScreenWidth * 0.23f, GameData::kScreenHeight * 0.07f, color, edgeColor, "LV.%d", playerAgiLevel);
}

// マップの描画
void TutorialScene::DrawFloor()
{
}

void TutorialScene::EndingUpdate()
{
	// エンディングの処理 相打ち時は勝ち優先
	// 共通処理
	if (m_isLoseEnd || m_isWinEnd)
	{
		if (m_endingAlpha < kEndingMaxAlpha)
		{
			m_endingAlpha += 2;
		}
		else
		{
			m_endingAlpha = kEndingMaxAlpha;
		}
	}

	if (m_isWinEnd)
	{
		if (m_endingTimer == kEndingTime)
		{
			if (Controller::Instance().GetInputDown(Button::A))
			{
				ChangeState(SceneState::END);
				return;
			}
		}
		else
		{
			m_endingTimer++;
		}
	}
	else if (m_isLoseEnd)
	{
		if (m_endingTimer == kEndingTime)
		{
			if (Controller::Instance().GetInputDown(Button::A))
			{
				ChangeState(SceneState::END);
				return;
			}
		}
		else
		{
			m_endingTimer++;
		}
	}
}

void TutorialScene::EndingDraw()
{
	auto timeRange = m_endingTimer;
	if (m_endingTimer > kEndingTime / 3)
	{
		timeRange = kEndingTime / 3;
	}
	auto range = static_cast<float>(GameData::kScreenHalfHeight / (kEndingTime * 3)) * timeRange;

	// エンディングの描画
	if (m_isWinEnd)
	{
		// アルファ値の設定
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_endingAlpha);
		DrawBox(0, 0, GameData::kScreenWidth + 1, GameData::kScreenHeight + 1, 0x000000, true);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

		DrawBoxAA(0, GameData::kScreenHalfHeight - range - 2, GameData::kScreenWidth + 1, GameData::kScreenHalfHeight + range + 2, 0xf0f0f0, true);
		DrawBoxAA(0, GameData::kScreenHalfHeight - range, GameData::kScreenWidth + 1, GameData::kScreenHalfHeight + range, 0xa0a000, true);
		if (m_endingTimer == kEndingTime)
			DrawFormatString2F(GameData::kScreenWidth * 0.45f, GameData::kScreenHeight * 0.47f, 0xffffff, 0x202020, "  WIN");
	}
	else if (m_isLoseEnd)
	{
		// アルファ値の設定
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_endingAlpha);
		DrawBox(0, 0, GameData::kScreenWidth + 1, GameData::kScreenHeight + 1, 0x000000, true);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

		DrawBoxAA(0, GameData::kScreenHalfHeight - range - 2, GameData::kScreenWidth + 1, GameData::kScreenHalfHeight + range + 2, 0xf0f0f0, true);
		DrawBoxAA(0, GameData::kScreenHalfHeight - range, GameData::kScreenWidth + 1, GameData::kScreenHalfHeight + range, 0x000030, true);
		if (m_endingTimer == kEndingTime)
			DrawFormatString2F(GameData::kScreenWidth * 0.45f, GameData::kScreenHeight * 0.47f, 0x101010, 0xffffff, "Lose...");
	}
}