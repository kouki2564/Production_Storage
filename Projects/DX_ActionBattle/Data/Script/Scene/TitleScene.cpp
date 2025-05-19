#include "TitleScene.h"

#include "SoundManager.h"
#include "EffectManager.h"
#include "HandleManager.h"

// SceneState
#include "SceneStart.h"
#include "SceneMain.h"
#include "SceneEnd.h"
#include "ScenePause.h"

// モデル情報
#include "Player.h"
#include "Mutant.h"
#include "EnemyFly.h"
#include "EnemyStandard.h"
#include "EnemyMagic.h"
#include "EnemyTank.h"

#include "WeaponBase.h"
#include "Sword.h"
#include "Axe.h"
#include "Magic.h"

namespace
{
	constexpr int kBgColorTop = 0x1d1d8d;
	constexpr int kBgColorBottom = 0x000000;
	constexpr float kBgColorMoveR = 0x1d;
	constexpr float kBgColorMoveG = 0x1d;
	constexpr float kBgColorMoveB = 0x8d;

	constexpr float kStageScale = 10.0f;
	constexpr float kModelScale = 10.0f;
	constexpr VECTOR kPlayerPos = { 0, 0, 0 };
	constexpr VECTOR kMutantPos = { 0, 0, 100 };
	constexpr VECTOR kEnemy1Pos = { 80, 10, 30 };
	constexpr VECTOR kEnemy2Pos = { -80, 0, 30 };
	constexpr VECTOR kEnemy3Pos = { -70, 0, 50 };
	constexpr VECTOR kEnemy4Pos = { 70, 0, 50 };
	constexpr VECTOR kBgEffectPos = { 0, 30, 100 };

	constexpr VECTOR kSwordPos = { -10, 20, 0 };
	constexpr VECTOR kSwordDir = { 0, 0, 1 };
	constexpr VECTOR kAxePos = { 0, 30, 10 };
	constexpr VECTOR kAxeDir = { 0, 0, 1 };
	constexpr VECTOR kMagicPos = { 10, 20, 0 };
}

TitleScene::TitleScene()
{
	m_titleHandle = LoadGraph("Data/Image/TitleLogo.png");
	m_isBlink = false;
	m_sceneState = std::make_unique<SceneStart>();

	m_bgColorR = 0x1d;
	m_bgColorG = 0x1d;
	m_bgColorB = 0x8d;
	SoundManager::Instance().loopPlay("Title");

	/*m_camera.SetTargetPos(VZero(), VZero());
	m_light.Init(m_camera.GetPos(), m_camera.GetCameraDir());*/

	// m_stage.insert(std::make_pair("Stage", std::make_unique<Stage>()));
	// m_stage["Stage"]->InitStage("Stage", HandleManager::Instance().GetModelHandle(ModelName::NORMALSTAGE));
	m_stage.insert(std::make_pair("Boss", std::make_unique<Stage>()));
	m_stage["Boss"]->InitStage("Boss", HandleManager::Instance().GetModelHandle(ModelName::BOSSSTAGE));
	// m_stage["Boss"]->SetIsUse(false);

	// 配置モデル情報
	m_objects.insert(std::make_pair("Player", std::make_unique<Player>()));
	m_objects["Player"]->Init("Player", kPlayerPos, VGet(0, 0, 1));
	m_objects.insert(std::make_pair("Mutant", std::make_unique<Mutant>()));
	m_objects["Mutant"]->Init("Mutant", kMutantPos, VGet(0, 0, -1));
	m_objects.insert(std::make_pair("EnemyFly", std::make_unique<EnemyFly>()));
	auto dir = VNorm(VSub(m_objects["Player"]->GetPos(), kEnemy1Pos));
	m_objects["EnemyFly"]->Init("EnemyFly", kEnemy1Pos, dir);
	m_objects.insert(std::make_pair("EnemyStandard", std::make_unique<EnemyStandard>()));
	dir = VNorm(VSub(m_objects["Player"]->GetPos(), kEnemy2Pos));
	m_objects["EnemyStandard"]->Init("EnemyStandard", kEnemy2Pos, dir);
	m_objects.insert(std::make_pair("EnemyMagic", std::make_unique<EnemyMagic>()));
	dir = VNorm(VSub(m_objects["Player"]->GetPos(), kEnemy3Pos));
	m_objects["EnemyMagic"]->Init("EnemyMagic", kEnemy3Pos, dir);
	m_objects.insert(std::make_pair("EnemyTank", std::make_unique<EnemyTank>()));
	dir = VNorm(VSub(m_objects["Player"]->GetPos(), kEnemy4Pos));
	m_objects["EnemyTank"]->Init("EnemyTank", kEnemy4Pos, dir);

	/*m_weapons.insert(std::make_pair("Sword", std::make_unique<Sword>()));
	m_weapons["Sword"]->SetModel(kSwordPos, kSwordDir);
	m_weapons.insert(std::make_pair("Axe", std::make_unique<Axe>()));
	m_weapons["Axe"]->SetModel(kAxePos, kAxeDir);
	m_weapons.insert(std::make_pair("Magic", std::make_unique<Magic>()));
	m_weapons["Magic"]->SetPos(kMagicPos);*/

	// m_objects["Player"]->SetWeapon(m_weaponSelect);

	EffectManager::Instance().RegisterEffect("Floor", HandleManager::Instance().GetEffectHandle(EffectName::FLOOR), VZero(), VGet(0, 0, -1));
	EffectManager::Instance().RegisterEffect("Sky", HandleManager::Instance().GetEffectHandle(EffectName::SKY), VZero(), VZero());
	EffectManager::Instance().RegisterEffect("Smoke", HandleManager::Instance().GetEffectHandle(EffectName::SMOKE), VZero(), VZero());
	EffectManager::Instance().RegisterEffect("Bg", HandleManager::Instance().GetEffectHandle(EffectName::TITLEBG), kBgEffectPos, VGet(-1, 0, 0));
	EffectManager::Instance().RegisterEffect("Hide", HandleManager::Instance().GetEffectHandle(EffectName::TITLEHIDE), VZero(), VGet(0, 0, -1));
	EffectManager::Instance().RegisterEffect("Thunder", HandleManager::Instance().GetEffectHandle(EffectName::THUNDER), VZero(), VGet(0, 0, -1));
}

TitleScene::~TitleScene()
{
	m_objects.clear();
	m_sceneState.reset();
	DeleteGraph(m_titleHandle);
	SoundManager::Instance().Stop("Title");

	EffectManager::Instance().DeleteEffect("Bg");
	EffectManager::Instance().DeleteEffect("Hide");
	EffectManager::Instance().DeleteEffect("Floor");
	EffectManager::Instance().DeleteEffect("Sky");
	EffectManager::Instance().DeleteEffect("Smoke");
	EffectManager::Instance().DeleteEffect("Thunder");

	DrawingManager::Instance().DeleteAllModel();
}

void TitleScene::InitScene()
{
	//// 背景の準備
	//for (int i = 0; i < 10; i++)
	//{
	//	// 座標をランダムに設定
	//	m_bg[i].posX = GetRand(GameData::kScreenWidth);
	//	m_bg[i].posY = GetRand(GameData::kScreenHeight);
	//	// 円の半径を5～20でランダムに設定
	//	m_bg[i].range = GetRand(5) + 5;
	//	// 移動スピードを1.0～2.0でランダムに設定
	//	m_bg[i].speed = GetRand(100) * 0.01f + 1.0f;
	//}

	// カメラ初期化
	m_camera.Init(VGet(0, 30, -180));

	m_camera.ChangeViewMode(ViewMode::TPS, m_objects["Player"]->GetPos(), m_objects["Player"]->GetDir());
	// m_camera.SetTargetPos(m_objects["Player"]->GetPos(), m_objects["Mutant"]->GetCollider().pos[1]);
	m_camera.SetIsCameraRotate(false);

	EffectManager::Instance().PlayEffect("Bg", kBgEffectPos, VGet(-1, 0, 0));
	EffectManager::Instance().PlayEffect("Hide", kMutantPos, VGet(0, 0, -1));

	// モデルのアニメーションスタート
	for (auto& i : m_objects)
	{
		i.second->InitTitleAnimation();
	}
}

void TitleScene::UpdateScene()
{
	// m_camera.UpdatePos();
	m_light.Update(m_camera.GetPos(), m_camera.GetTargetPos());
	// 主操作
	if (m_sceneState->isMove)
	{
		// スタート文の点滅処理
		BlinkStartString(5);

		if (Controller::Instance().GetInputDown(Button::A))
		{
			// シーン遷移
			ChangeState(SceneState::END);
			SoundManager::Instance().OnePlay("decision");
		}
		DrawSphere3D(VZero(), 1.0f, GetColor(255, 0, 0), 0xffffff,0xffffff, true);

	}

	ObjectsUpdate();
	EffectManager::Instance().Update();
	/* Draw */
	// 背景
	// BackgroundUpdate();
	DrawingManager::Instance().Draw();
	EffectManager::Instance().Draw();
	PanelManager::Instance().Draw();
	// タイトルロゴ
	DrawGraph(0, 0, m_titleHandle, true);
	// スタート文章
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_startAlpha);
	DrawFormatString2F(GameData::kScreenWidth * 0.38f, GameData::kScreenHeight * 0.9f, 0xa0a0a0, 0x101010, "PUSH A TO START");

		// ゲーム進行
	if (m_sceneState->isProgress)
	{
		if (m_sceneState->GetState() == SceneState::START)
		{
			// フェードスキップ
			if (Controller::Instance().GetInputDown(Button::A))
			{
				FadeSkip(0);
			}
			// フェードイン
			if (FadeIn(2))
			{
				ChangeState(SceneState::MAIN);
				m_isBlink = true;
			}

		}
		else if (m_sceneState->GetState() == SceneState::END)
		{
			// フェードアウト
			if (FadeOut(3))
			{
				m_nextScene = Scene::TUTORIAL;
				m_isMoveScene = true;
			}

			// スタート文の点滅処理
			BlinkStartString(20);
		}
	}

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	CameraUpdate();

}

void TitleScene::InitImage()
{
}

void TitleScene::ObjectsUpdate()
{
	for (auto& t : m_objects)
	{
		DrawingManager::Instance().UpdateModelData(t.first, t.second->GetPos(), t.second->GetDir());
	}

	/*for (auto& t : m_weapons)
	{
		t.second->WeaponUpdate();
	}*/

	/* インプット処理、各オブジェクトの行動決定パート */
	//for (auto it = m_objects.begin(); it != m_objects.end(); )
	//{
	//	auto& t1 = *it;
	//	// プレイヤーのとき
	//	if (t1.first == "Player")
	//	{
	//		auto pos = VZero();
	//		/// 基準距離
	//		float enemyRange = 100.0f;

	//		// 最接近の敵を検索
	//		for (auto its = m_objects.begin(); its != m_objects.end(); ++its)
	//		{
	//			auto& t2 = *its;
	//			auto vec = VSub(t2.second->GetCollider().pos[0], t1.second->GetCollider().pos[0]);
	//			if (t2.first != "Player" && VSize(vec) <= enemyRange)
	//			{
	//				enemyRange = VSize(vec);
	//				pos = t2.second->GetCollider().centerPos;
	//			}
	//		}
	//		t1.second->SetChaseTarget(pos);
	//	}
	//	// プレイヤー以外の時
	//	else
	//	{
	//		t1.second->SetChaseTarget(m_objects["Player"]->GetPos());
	//	}

	//	// 各オブジェクトの行動
	//	t1.second->Update();

	//	++it;
	//}

}

void TitleScene::CollisionUpdate()
{
}

void TitleScene::CameraUpdate()
{
	// カメラ関係
	int size = static_cast<int>(m_objects.size());
	auto pos = m_objects["Player"]->GetPos();
	pos.y += 20;
	m_camera.SetTargetPos(pos, VZero());


	for (auto& t : m_objects)
	{
		t.second->SetCameraDir(m_camera.GetCameraDir());
	}

	m_camera.Update();
}

void TitleScene::BackgroundUpdate()
{
	// SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	// sin波の移動
	m_sinMove1 += 3.0f;
	m_sinMove2 -= 5.0f;

	// 合成したsin波を基準色として、上をだんだん青く、下をだんだん黒くしたグラデーション背景
	for (int x = 0; x < GameData::kScreenWidth; x++)
	{
		// 二つのsin波を合成
		auto baseY = static_cast<int>(((GameData::kScreenHeight / 2.0f + sin((x + m_sinMove1) * 0.05f) * 40.0f) +
									   (GameData::kScreenHeight / 2.0f + sin((x + m_sinMove2 + 80.0f) * 0.025f) * 40.0f)) * 0.25f);

		// sin波を合成したy座標地点から上を基準色から青く、下を基準色から黒く変化させる
		for (int y = 0; y < GameData::kScreenHeight; y++)
		{
			// 描画する色
			auto colorR = static_cast<int>(abs(baseY - y * 0.5f) * kBgColorMoveR / GameData::kScreenHalfHeight);
			if (colorR < 0) colorR = 0;
			else if (colorR > 0xff) colorR = 0xff;
			auto colorG = static_cast<int>(abs(baseY - y * 0.5f) * kBgColorMoveG / GameData::kScreenHalfHeight);
			if (colorG < 0) colorG = 0;
			else if (colorG > 0xff) colorG = 0xff;
			auto colorB = static_cast<int>(abs(baseY - y * 0.5f) * kBgColorMoveB / GameData::kScreenHalfHeight);
			if (colorB < 0) colorB = 0;
			else if (colorB > 0xff) colorB = 0xff;

			// 描画
			DrawPixel(x, y, GetColor(colorR, colorG, colorB));

		}
	}

	// 上昇して消えていく円の描画
	for (int i = 0; i < 10; i++)
	{
		// 上昇
		m_bg[i].posY -= m_bg[i].speed;

		// 画面外に出たらｙ座標を一番下の画面外に再配置してｘ座標をランダムに設定
		if (m_bg[i].posY + m_bg[i].range < 0)
		{
			m_bg[i].posY = static_cast<float>(GameData::kScreenHeight + m_bg[i].range);
			m_bg[i].posX = static_cast<float>(GetRand(GameData::kScreenWidth));
		}

		// アルファ値の変化量の設定
		int alphaMove = 255 / m_bg[i].range;
		// 座標の上昇によるアルファ値倍率
		int alphaScale = static_cast<int>(m_bg[i].posY / GameData::kScreenHeight);

		// 円を半径の長さ分に分けて描画
		for (int range = 0; range <= m_bg[i].range; range++)
		{
			// アルファ値を半径に応じて、外側に行くほど薄く、上昇するほど薄く変化
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255 - alphaMove * range));
			// DrawCircle(m_bg[i].posX, m_bg[i].posY, range, 0xffffff, true);
			DrawCircle(static_cast<int>(m_bg[i].posX), static_cast<int>(m_bg[i].posY), range, 0xffffff, true);
		}
	}
	// アルファ値を元に戻す
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

	
}

void TitleScene::BlinkStartString(int value)
{
	if (m_isBlink)
	{
		if (m_startAlpha > 255)
		{
			m_startAlpha += value;
		}
		else
		{
			m_startAlpha = 255;
			m_isBlink = false;
		}
	}
	else
	{
		if (m_startAlpha > 0)
		{
			m_startAlpha -= value;
		}
		else
		{
			m_startAlpha = 0;
			m_isBlink = true;
		}
	}
}