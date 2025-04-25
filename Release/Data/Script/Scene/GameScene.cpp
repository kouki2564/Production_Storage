#include "GameScene.h"
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
	constexpr int kTutorialDeltaAlpha = 255 / 40;
	// チュートリアルの説明表示位置
	constexpr float kTutorialPosX = GameData::kScreenHalfWidth;
	constexpr float kTutorialPosY = GameData::kScreenHeight * 0.85f;
	// チュートリアルの説明表示のスケール
	constexpr float kTutorialScale = 1.0f;
	// チュートリアルの初期表示時間
	constexpr int kTutorialTime = 300;


	constexpr int kEndingTime = 30;
	constexpr int kEndingMaxAlpha = 100;

	constexpr int kStageRange = 140;
	constexpr int kStageMoveLine = 130;
}

GameScene::GameScene()
{
	SoundManager::Instance().loopPlay("Stage");

	m_sceneState = std::make_unique<SceneStart>();
	m_pPause = std::make_shared<PauseMenu>();

	// ステージ移動フラグをfalseに初期化
	for (int i = 0; i < GameData::kStageMaxLine; i++)
	{
		for (int j = 0; j < GameData::kStageMaxRow; j++)
		{
			m_isMoveMap[i][j] = false;
		}
	}
	m_nextPlayerPos = VGet(0, 0, -100);
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

GameScene::~GameScene()
{
	PanelManager::Instance().DeleteAllImage();

	m_objects.clear();
	// std::map<std::string, std::shared_ptr<ObjectBase>>().swap(m_objects);
	m_enemyData.clear();
	m_pPause.reset();
	m_sceneState.reset();

	EffectManager::Instance().DeleteEffect("Floor");
	EffectManager::Instance().DeleteEffect("Sky");
	EffectManager::Instance().DeleteEffect("Smoke");
	

	/*for (auto& i : m_stage)
	{
		i.second.reset();
	}
	m_stage.clear();

	for (auto& i : m_objects)
	{
		i.second.reset();
	}
	m_objects.clear();*/

	DrawingManager::Instance().DeleteAllModel();
	SoundManager::Instance().Stop("Stage");
	SoundManager::Instance().Stop("Boss");
}

void GameScene::InitScene()
{
	// カメラの初期化
	m_camera.Init(VGet(0, 80, -180));

	// ステージ番号の初期化
	m_nowStageNum[0] = 4;
	m_nowStageNum[1] = 1;

	// プレイヤー情報
	m_objects.insert(std::make_pair("Player", std::make_unique<Player>()));
	m_objects["Player"] ->Init("Player", m_nextPlayerPos, m_nextPlayerDir);
	m_objects["Player"]->SetWeapon(m_weaponSelect);

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

void GameScene::UpdateScene()
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
				if (m_enemyData.size() <= 0)
				{
					// ステージクリア時だけポイント加算
					if (!m_isMoveMap[m_nowStageNum[0]][m_nowStageNum[1]] && 
						!(m_nowStageNum[0] == 0 && m_nowStageNum[1] == 1))
					{
						m_isMoveMap[m_nowStageNum[0]][m_nowStageNum[1]] = true;
						m_upgradePoint++;
					}
				}
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
			m_objects["Player"] ->SetUpgrade(upgrade);
		}
	}
	// プレイヤーのHP表示の更新
	PlayerStatusPop();

	// カメラの更新
	CameraUpdate();
	EffectManager::Instance().Update();

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
				// オブジェクトの参照数チェック
				for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
				{
					auto& t = *it;
				}

				ChangeState(SceneState::MAIN);
			}
		}
		else if (m_sceneState->GetState() == SceneState::END)
		{
			// ステージ更新処理等

			// フェードアウト後
			if (FadeOut(5))
			{
				// 最終ステージ時
				if (m_isLoseEnd || m_isWinEnd)
				{
					// エンディング移行
					m_nextScene = Scene::TITLE;
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
					if (!m_isMoveMap[m_nowStageNum[0]][m_nowStageNum[1]])
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

// ステージの更新
void GameScene::UpdateStage()
{
}

void GameScene::InitImage()
{
	// 表示ステータスの画像登録
	PanelManager::Instance().RegisterImage("BarBack", HandleManager::Instance().GetImageHandle(ImageName::BARBACK), kBarPosX, kBarPosY, kStatusScale, kStatusScale);
	PanelManager::Instance().RegisterImage("BarGreen", HandleManager::Instance().GetImageHandle(ImageName::BARGREEN), kBarPosX, kBarPosY, kStatusScale, kStatusScale);
	PanelManager::Instance().RegisterImage("BarRed", HandleManager::Instance().GetImageHandle(ImageName::BARRED), kBarPosX, kBarPosY, kStatusScale, kStatusScale);
	PanelManager::Instance().RegisterImage("Status", HandleManager::Instance().GetImageHandle(ImageName::STATUS), kStatusPosX, kStatusPosY, kStatusScale, kStatusScale);

	PanelManager::Instance().SetIsDrawImage("BarRed", false);

	PanelManager::Instance().RegisterImage("Pin", HandleManager::Instance().GetImageHandle(ImageName::PIN), 0.0f, 0.0f, 1.0f, 1.0f);

	// チュートリアルの画像登録
	PanelManager::Instance().RegisterImage("TutorialWindow", HandleManager::Instance().GetImageHandle(ImageName::TUTORIAL_WINDOW), kTutorialPosX, kTutorialPosY, kTutorialScale, m_tutorialWindowScaleY);
	PanelManager::Instance().SetIsDrawImage("TutorialWindow", true);
	PanelManager::Instance().RegisterImage("Tutorial_1", HandleManager::Instance().GetImageHandle(ImageName::TUTORIAL_WEAPON), kTutorialPosX, kTutorialPosY, kTutorialScale, kTutorialScale);
	PanelManager::Instance().SetIsDrawImage("Tutorial_1", false);
	PanelManager::Instance().RegisterImage("Tutorial_2", HandleManager::Instance().GetImageHandle(ImageName::TUTORIAL_MAINSTART), kTutorialPosX, kTutorialPosY, kTutorialScale, kTutorialScale);
	PanelManager::Instance().SetIsDrawImage("Tutorial_2", false);
	PanelManager::Instance().RegisterImage("Tutorial_3", HandleManager::Instance().GetImageHandle(ImageName::TUTORIAL_PAUSE), kTutorialPosX, kTutorialPosY, kTutorialScale, kTutorialScale);
	PanelManager::Instance().SetIsDrawImage("Tutorial_3", false);
}

// オブジェクトの更新
void GameScene::ObjectsUpdate()
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
				auto vec = VSub(t2.second ->GetCollider().pos[0], t1.second ->GetCollider().pos[0]);
				if (t2.first != "Player" && VSize(vec) <= enemyRange)
				{
					enemyRange = VSize(vec);
					pos = t2.second ->GetCollider().centerPos;
				}
			}
			t1.second ->SetChaseTarget(pos);
		}
		// プレイヤー以外の時
		else
		{
			t1.second ->SetChaseTarget(m_objects["Player"] ->GetPos());
		}

		// 各オブジェクトの行動
		t1.second ->Update();

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

void GameScene::CollisionUpdate()
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
		for (auto it2 = m_stage.begin(); it2 != m_stage.end(); ++it2)
		{
			auto& t2 = *it2;
			// 押し出しがない（マップに接していない）ときに
			// 次フレームでの接触を確認する

			if (m_nowStageNum[0] == 0 && m_nowStageNum[1] == 1 && t2.first == "Stage")
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
			t1.second->SetPosAndDir(VGet(0, 10, 0), VGet(0, 0, 1));
		}

		++it1;
	}
}

void GameScene::TutorialUpdate()
{
	// 初期フロアだけチュートリアル表示
	if (m_nowStageNum[0] == 4 && m_nowStageNum[1] == 1)
	{
		// チュートリアル表示の切り替え
		if (!m_isTutorialSwitch)
		{
			if (m_tutorialAlpha == 255)
			{
				if (m_tutorialTimer < kTutorialTime)
				{
					m_tutorialTimer++;
				}
				else
				{
					m_tutorialTimer = kTutorialTime;
					m_isTutorialSwitch = true;
				}
			}
		}
		else
		{
			if (PanelManager::Instance().GetIsDrawImage("Tutorial_1"))
			{
				m_tutorialAlpha = 0;
			}
			PanelManager::Instance().SetIsDrawImage("Tutorial_1", false);
			PanelManager::Instance().SetIsDrawImage("Tutorial_2", true);
		}

		// フロア開始時処理
		if (!m_isMoveMap[m_nowStageNum[0]][m_nowStageNum[1]])
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

				PanelManager::Instance().SetIsDrawImage("Tutorial_1", true);
			}

			// チュートリアル本文のアルファ値上昇
			if (m_tutorialWindowScaleY == kTutorialWindowMaxScaleY)
			{
				if (PanelManager::Instance().GetIsDrawImage("Tutorial_1"))
				{
					if (m_tutorialAlpha < 255)
					{
						m_tutorialAlpha += kTutorialDeltaAlpha;
						PanelManager::Instance().SetAlpha("Tutorial_1", m_tutorialAlpha);
					}
					else if (m_tutorialAlpha > 255)
					{
						m_tutorialAlpha = 255;
						PanelManager::Instance().SetAlpha("Tutorial_1", m_tutorialAlpha);
					}
				}
				if (PanelManager::Instance().GetIsDrawImage("Tutorial_2"))
				{
					if (m_tutorialAlpha < 255)
					{
						m_tutorialAlpha += kTutorialDeltaAlpha;
						PanelManager::Instance().SetAlpha("Tutorial_2", m_tutorialAlpha);
					}
					else if (m_tutorialAlpha > 255)
					{
						m_tutorialAlpha = 255;
						PanelManager::Instance().SetAlpha("Tutorial_2", m_tutorialAlpha);
					}
				}
				if (PanelManager::Instance().GetIsDrawImage("Tutorial_3"))
				{
					if (m_tutorialAlpha < 255)
					{
						m_tutorialAlpha += kTutorialDeltaAlpha;
						PanelManager::Instance().SetAlpha("Tutorial_3", m_tutorialAlpha);
					}
					else if (m_tutorialAlpha > 255)
					{
						m_tutorialAlpha = 255;
						PanelManager::Instance().SetAlpha("Tutorial_3", m_tutorialAlpha);
					}
				}
			}
		}
		else
		{
			if (PanelManager::Instance().GetIsDrawImage("Tutorial_2"))
			{
				m_tutorialAlpha = 0;
				PanelManager::Instance().SetIsDrawImage("Tutorial_2", false);
				PanelManager::Instance().SetIsDrawImage("Tutorial_3", true);
			}
		}
	}
	else
	{
		PanelManager::Instance().SetIsDrawImage("TutorialWindow", false);
		PanelManager::Instance().SetIsDrawImage("Tutorial_1", false);
		PanelManager::Instance().SetIsDrawImage("Tutorial_2", false);
		PanelManager::Instance().SetIsDrawImage("Tutorial_3", false);
	}
}

void GameScene::TutorialDraw()
{
}

void GameScene::CameraUpdate()
{
	// カメラ関係
	int size = m_objects.size();
	m_camera.SetTargetPos(m_objects["Player"] ->GetPos(), VZero());

	if (m_nowStageNum[0] == 0 && m_nowStageNum[1] == 1)
	{
		if (Controller::Instance().GetInputDown(Button::RB))
		{
			m_camera.ChangeIsTarget();
		}
		m_camera.SetTargetPos(m_objects["Player"]->GetPos(), m_objects["Mutant"]->GetCollider().pos[1]);
	}

	for (auto& t : m_objects)
	{
		t.second->SetCameraDir(m_camera.GetCameraDir());
	}

	m_camera.Update();
}

void GameScene::ChackMoveMap()
{
	// 現在マップの敵が全滅してマップ移動可能かどうかのチェック
	if (!(m_nowStageNum[0] == 0 && m_nowStageNum[1] == 1) &&
		m_isMoveMap[m_nowStageNum[0]][m_nowStageNum[1]])
	{
		auto pos = m_objects["Player"] ->GetPos();

		// どの方向に進めるかのチェック
		if ((m_nowStageNum[0] > 0 && m_nowStageNum[0] < 4) && m_nowStageNum[1] > 0)															// 左
		{
			EffectManager::Instance().StopEffect("SistemWall_Left");
			// プレイヤーの座標チェック
			if (pos.x <= -kStageMoveLine && abs(pos.z) < 15)
			{
				m_moveRoomDir = LEFT;
				ChangeState(SceneState::END);
			}
		}
		if ((m_nowStageNum[0] > 0 && m_nowStageNum[0] < 4) && m_nowStageNum[1] < GameData::kStageMaxRow - 1)								// 右
		{
			EffectManager::Instance().StopEffect("SistemWall_Right");
			// プレイヤーの座標チェック
			if (pos.x >= kStageMoveLine && abs(pos.z) < 15)
			{
				m_moveRoomDir = RIGHT;
				ChangeState(SceneState::END);
			}
		}
		if (m_nowStageNum[0] > 0 && !(m_nowStageNum[0] == 1 && m_nowStageNum[1] != 1))														// 奥
		{
			EffectManager::Instance().StopEffect("SistemWall_Flont");
			// プレイヤーの座標チェック
			if (abs(pos.x) < 15 && pos.z >= kStageMoveLine)
			{
				m_moveRoomDir = UP;
				ChangeState(SceneState::END);
			}
		}
		if (m_nowStageNum[0] < GameData::kStageMaxLine - 1 && !(m_nowStageNum[0] == GameData::kStageMaxLine - 2 && m_nowStageNum[1] != 1))	// 手前
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

void GameScene::DeleteStatgeData()
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
			t.second.reset();
			it = m_objects.erase(it);
		}
		else
		{
			++it;
		}
	}


	m_setEnemyNum = 0;
}

void GameScene::InitStageData(int stageLine, int stageRow)
{
	m_stage["Stage"]->ResetNoUseFrame();
	// ステージ本体の準備
	// ボスステージ
	if (stageLine == 0 && stageRow == 1)
	{

		SoundManager::Instance().Stop("Stage");
		SoundManager::Instance().loopPlay("Boss");
		// ステージセットアップ
		m_stage["Stage"]->SetIsUse(false);
		m_stage["Boss"]->SetIsUse(true);
		DrawingManager::Instance().SetIsDrawModel("Boss", true);
		DrawingManager::Instance().SetIsDrawModel("Stage", false);

		// カメラ更新
		m_camera.ChangeViewMode(ViewMode::TPS, m_objects["Player"]->GetPos(), m_objects["Player"]->GetDir());
		m_camera.SetIsCameraRotate(true);
	}
	// それ以外のステージ
	else
	{
		// ステージセットアップ
		m_stage["Stage"]->SetIsUse(true);
		m_stage["Boss"]->SetIsUse(false);

		// 諸々設定
		switch (stageLine)
		{
		case 1:
			switch (stageRow)
			{
			case 0:
				DrawingManager::Instance().SetStageDoor(true, false, false, true);
				break;
			case 1:
				DrawingManager::Instance().SetStageDoor(true, true, true, true);
				break;
			case 2:
				DrawingManager::Instance().SetStageDoor(false, false, true, true);
				break;
			default:
				break;
			}
			break;
		case 2:
			switch (stageRow)
			{
			case 0:
				DrawingManager::Instance().SetStageDoor(true, true, false, true);
				break;
			case 1:
				DrawingManager::Instance().SetStageDoor(true, true, true, true);
				break;
			case 2:
				DrawingManager::Instance().SetStageDoor(false, true, true, true);
				break;
			default:
				break;
			}
			break;
		case 3:
			switch (stageRow)
			{
			case 0:
				DrawingManager::Instance().SetStageDoor(true, true, false, false);
				break;
			case 1:
				DrawingManager::Instance().SetStageDoor(true, true, true, true);
				break;
			case 2:
				DrawingManager::Instance().SetStageDoor(false, true, true, false);
				break;
			default:
				break;
			}
			break;
		case 4:
			DrawingManager::Instance().SetStageDoor(false, true, false, false);
			break;
		default:
			break;
		}
		DrawingManager::Instance().SetIsDrawModel("Boss", false);
		DrawingManager::Instance().SetIsDrawModel("Stage", true);
		// 手前壁透け
		DrawingManager::Instance().SetFrameOpacityRate("Stage", 127, 0.4f);
	}

}

void GameScene::InitEnemyData(int stageLine, int stageRow)
{
	EffectManager::Instance().StopEffect("SistemWall_Flont");
	EffectManager::Instance().StopEffect("SistemWall_Back");
	EffectManager::Instance().StopEffect("SistemWall_Left");
	EffectManager::Instance().StopEffect("SistemWall_Right");

	// 前データを削除
	m_enemyData.clear();

	// フロアごとの敵とシステムウォールの設定
	// 縦列
	switch (stageLine)
	{
	case 0:
		// ボスの情報
		break;
	case 1:
		// 横列
		switch (stageRow)
		{
		case 0:
			m_enemyData.push_back(EnemyKind::MAGIC);
			m_enemyData.push_back(EnemyKind::FLY);
			m_enemyData.push_back(EnemyKind::MAGIC);
			EffectManager::Instance().PlayEffect("SistemWall_Back", VGet(0, 0, -kStageRange), VGet(-1, 0, 0));
			EffectManager::Instance().PlayEffect("SistemWall_Right", VGet(kStageRange, 0, 0), VGet(0, 0, 1));
			break;
		case 1:
			m_enemyData.push_back(EnemyKind::STANDARD);
			m_enemyData.push_back(EnemyKind::MAGIC);
			m_enemyData.push_back(EnemyKind::TANK);
			EffectManager::Instance().PlayEffect("SistemWall_Flont", VGet(0, 0, kStageRange), VGet(1, 0, 0));
			EffectManager::Instance().PlayEffect("SistemWall_Back", VGet(0, 0, -kStageRange), VGet(-1, 0, 0));
			EffectManager::Instance().PlayEffect("SistemWall_Left", VGet(-kStageRange, 0, 0), VGet(0, 0, -1));
			EffectManager::Instance().PlayEffect("SistemWall_Right", VGet(kStageRange, 0, 0), VGet(0, 0, 1));
			break;
		case 2:
			m_enemyData.push_back(EnemyKind::TANK);
			m_enemyData.push_back(EnemyKind::TANK);
			m_enemyData.push_back(EnemyKind::FLY);
			EffectManager::Instance().PlayEffect("SistemWall_Back", VGet(0, 0, -kStageRange), VGet(-1, 0, 0));
			EffectManager::Instance().PlayEffect("SistemWall_Left", VGet(-kStageRange, 0, 0), VGet(0, 0, -1));
			break;
		}
		break;
	case 2:
		// 横列
		switch (stageRow)
		{
		case 0:
			m_enemyData.push_back(EnemyKind::MAGIC);
			m_enemyData.push_back(EnemyKind::FLY);
			m_enemyData.push_back(EnemyKind::STANDARD);
			EffectManager::Instance().PlayEffect("SistemWall_Flont", VGet(0, 0, kStageRange), VGet(1, 0, 0));
			EffectManager::Instance().PlayEffect("SistemWall_Back", VGet(0, 0, -kStageRange), VGet(-1, 0, 0));
			EffectManager::Instance().PlayEffect("SistemWall_Right", VGet(kStageRange, 0, 0), VGet(0, 0, 1));
			break;
		case 1:
			m_enemyData.push_back(EnemyKind::STANDARD);
			m_enemyData.push_back(EnemyKind::FLY);
			m_enemyData.push_back(EnemyKind::FLY);
			EffectManager::Instance().PlayEffect("SistemWall_Flont", VGet(0, 0, kStageRange), VGet(1, 0, 0));
			EffectManager::Instance().PlayEffect("SistemWall_Back", VGet(0, 0, -kStageRange), VGet(-1, 0, 0));
			EffectManager::Instance().PlayEffect("SistemWall_Left", VGet(-kStageRange, 0, 0), VGet(0, 0, -1));
			EffectManager::Instance().PlayEffect("SistemWall_Right", VGet(kStageRange, 0, 0), VGet(0, 0, 1));
			break;
		case 2:
			m_enemyData.push_back(EnemyKind::TANK);
			m_enemyData.push_back(EnemyKind::STANDARD);
			m_enemyData.push_back(EnemyKind::FLY);
			EffectManager::Instance().PlayEffect("SistemWall_Flont", VGet(0, 0, kStageRange), VGet(1, 0, 0));
			EffectManager::Instance().PlayEffect("SistemWall_Back", VGet(0, 0, -kStageRange), VGet(-1, 0, 0));
			EffectManager::Instance().PlayEffect("SistemWall_Left", VGet(-kStageRange, 0, 0), VGet(0, 0, -1));
			break;
		}
		break;
	case 3:
		// 横列
		switch (stageRow)
		{
		case 0:
			m_enemyData.push_back(EnemyKind::STANDARD);
			m_enemyData.push_back(EnemyKind::MAGIC);
			EffectManager::Instance().PlayEffect("SistemWall_Flont", VGet(0, 0, kStageRange), VGet(1, 0, 0));
			EffectManager::Instance().PlayEffect("SistemWall_Right", VGet(kStageRange, 0, 0), VGet(0, 0, 1));
			break;
		case 1:
			m_enemyData.push_back(EnemyKind::FLY);
			EffectManager::Instance().PlayEffect("SistemWall_Flont", VGet(0, 0, kStageRange), VGet(1, 0, 0));
			EffectManager::Instance().PlayEffect("SistemWall_Back", VGet(0, 0, -kStageRange), VGet(-1, 0, 0));
			EffectManager::Instance().PlayEffect("SistemWall_Left", VGet(-kStageRange, 0, 0), VGet(0, 0, -1));
			EffectManager::Instance().PlayEffect("SistemWall_Right", VGet(kStageRange, 0, 0), VGet(0, 0, 1));
			break;
		case 2:
			m_enemyData.push_back(EnemyKind::STANDARD);
			m_enemyData.push_back(EnemyKind::TANK);
			EffectManager::Instance().PlayEffect("SistemWall_Flont", VGet(0, 0, kStageRange), VGet(1, 0, 0));
			EffectManager::Instance().PlayEffect("SistemWall_Left", VGet(-kStageRange, 0, 0), VGet(0, 0, -1));
			break;
		}
		break;
	case 4:
		// 横列
		switch (stageRow)
		{
		case 1:
			m_enemyData.push_back(EnemyKind::STANDARD);
			EffectManager::Instance().PlayEffect("SistemWall_Flont", VGet(0, 0, kStageRange), VGet(1, 0, 0));
			break;
		}
		break;
	}
}

void GameScene::SetEnemyData(int stageLine, int stageRow)
{
	auto enemyNum = m_enemyData.size();

	// ボスマップ以外
	if (!(stageLine == 0 && stageRow == 1))
	{
		// 敵の配置
		for (int i = 0; i < kEnemyMaxNum; i++)
		{
			// エネミーの配置座標
			VECTOR enemyPos = VZero();

			if (i == 0)
			{
				enemyPos = VGet(0, 0, -10);
			}
			else if (i == 1)
			{
				enemyPos = VGet(-75, 0, 50);
			}
			else
			{
				enemyPos = VGet(75, 0, 50);
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
				else if (m_enemyData[i] == EnemyKind::TANK)
				{
					m_objects.insert(std::make_pair(name, std::make_unique<EnemyTank>()));
				}
				else if (m_enemyData[i] == EnemyKind::MAGIC)
				{
					m_objects.insert(std::make_pair(name, std::make_unique<EnemyMagic>()));
				}
				else if (m_enemyData[i] == EnemyKind::FLY)
				{
					enemyPos.y = 45;
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
		m_objects["Mutant"]->Init("Mutant", VGet(0, 0, 100), VGet(0, 0, -1));
	}
	
}

void GameScene::PlayerStatusPop()
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
void GameScene::DrawParameter()
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

// マジックナンバーの塊
// ミニマップ表示
void GameScene::DrawFloor()
{
	// アルファ値の設定
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);

	DrawBoxAA(GameData::kScreenWidth -50 - 10 - 100,	20,			GameData::kScreenWidth - 50 - 10 - 50,		20 + 50,	   GetColor(200, 200, 200), true);
	
	DrawBox(GameData::kScreenWidth -50 - 20 - 150,	20 + 50 + 10,	GameData::kScreenWidth - 50 - 20 - 100,		20 + 10 + 100, GetColor(200, 200, 200), true);
	DrawBox(GameData::kScreenWidth -50 - 10 - 100,	20 + 50 + 10,	GameData::kScreenWidth - 50 - 10 - 50,		20 + 10 + 100, GetColor(200, 200, 200), true);
	DrawBox(GameData::kScreenWidth -50 - 50,		20 + 50 + 10,	GameData::kScreenWidth - 50,				20 + 10 + 100, GetColor(200, 200, 200), true);
																																		
	DrawBox(GameData::kScreenWidth - 50 - 20 - 150, 20  + 100+ 20,	GameData::kScreenWidth - 50 - 20 - 100,		20 + 20 + 150, GetColor(200, 200, 200), true);
	DrawBox(GameData::kScreenWidth - 50 - 10 - 100, 20  + 100+ 20,	GameData::kScreenWidth - 50 - 10 - 50,		20 + 20 + 150, GetColor(200, 200, 200), true);
	DrawBox(GameData::kScreenWidth - 50 - 50,		20  + 100+ 20,	GameData::kScreenWidth - 50,				20 + 20 + 150, GetColor(200, 200, 200), true);
																																		
	DrawBox(GameData::kScreenWidth - 50 - 20 - 150, 20 + 150 + 30,	GameData::kScreenWidth - 50 - 20 - 100,		20 + 30 + 200, GetColor(200, 200, 200), true);
	DrawBox(GameData::kScreenWidth - 50 - 10 - 100, 20 + 150 + 30,	GameData::kScreenWidth - 50 - 10 - 50,		20 + 30 + 200, GetColor(200, 200, 200), true);
	DrawBox(GameData::kScreenWidth - 50 - 50,		20 + 150 + 30,	GameData::kScreenWidth - 50,				20 + 30 + 200, GetColor(200, 200, 200), true);
																																		
	DrawBox(GameData::kScreenWidth - 50 - 10 - 100,	20 + 200 + 40,	GameData::kScreenWidth - 50 - 10 - 50,		20 + 40 + 250, GetColor(200, 200, 200), true);

	// アルファ値もどし
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

	// pinサイズ調整
	float posX;
	float posY;
	PanelManager::Instance().GetSize("Pin", &posX, &posY);
	posX *= 0.5f;
	posY *= 0.3f;
	// ミニマップ位置まで調整
	posX += GameData::kScreenWidth - 50 - (3 - m_nowStageNum[1]) * 50 - (2 - m_nowStageNum[1]) * 10 + 15;
	posY += 20 + 60 * m_nowStageNum[0] + 15;

	// マップピン
	PanelManager::Instance().UpdateImagePos("Pin", posX, posY);
	// DrawExtendGraph(posX,posY, posX + 50 - 30, posY + 50 - 25, m_tutorialHandle[9], true);

}

void GameScene::EndingUpdate()
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

void GameScene::EndingDraw()
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
		DrawBoxAA(0, GameData::kScreenHalfHeight - range, GameData::kScreenWidth+1, GameData::kScreenHalfHeight + range, 0xa0a000, true);
		if(m_endingTimer == kEndingTime)
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
