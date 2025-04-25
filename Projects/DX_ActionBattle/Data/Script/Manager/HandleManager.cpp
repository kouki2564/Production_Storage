#include "HandleManager.h"
#include "DxLib.h"
#include "EffekseerForDXLib.h"
#include <cassert>

HandleManager::HandleManager()
{
	// 各ハンドルの作成
	// モデル
	m_models.insert(std::make_pair(ModelName::PLAYER, MV1LoadModel("Data/Model/Player.mv1")));
	m_models.insert(std::make_pair(ModelName::MUTANT, MV1LoadModel("Data/Model/Boss_1.mv1")));
	m_models.insert(std::make_pair(ModelName::ENEMY1, MV1LoadModel("Data/Model/Enemy_1.mv1")));
	m_models.insert(std::make_pair(ModelName::ENEMY2, MV1LoadModel("Data/Model/Enemy_2.mv1")));
	m_models.insert(std::make_pair(ModelName::ENEMY3, MV1LoadModel("Data/Model/Enemy_3.mv1")));
	m_models.insert(std::make_pair(ModelName::ENEMY4, MV1LoadModel("Data/Model/Enemy_4.mv1")));
	m_models.insert(std::make_pair(ModelName::SWORD, MV1LoadModel("Data/Model/Sword.mv1")));
	m_models.insert(std::make_pair(ModelName::AXE, MV1LoadModel("Data/Model/Axe.mv1")));
	m_models.insert(std::make_pair(ModelName::NORMALSTAGE, MV1LoadModel("Data/Model/Stage.mv1")));
	m_models.insert(std::make_pair(ModelName::BOSSSTAGE, MV1LoadModel("Data/Model/BossStage.mv1")));

	// 画像
	m_images.insert(std::make_pair(ImageName::BARBACK, LoadGraph("Data/Image/BarBack.png")));
	m_images.insert(std::make_pair(ImageName::BARGREEN, LoadGraph("Data/Image/BarGreen.png")));
	m_images.insert(std::make_pair(ImageName::BARRED, LoadGraph("Data/Image/BarRed.png")));
	m_images.insert(std::make_pair(ImageName::ENEMYBAR, LoadGraph("Data/Image/BarRed.png")));
	m_images.insert(std::make_pair(ImageName::ENEMYBARBACK, LoadGraph("Data/Image/BarBack.png")));
	m_images.insert(std::make_pair(ImageName::STATUS, LoadGraph("Data/Image/Player_StatusBar.png")));
	m_images.insert(std::make_pair(ImageName::NUMBER, LoadGraph("Data/Image/number.png")));
	m_images.insert(std::make_pair(ImageName::PAUSE, LoadGraph("Data/Image/PauseMenu.png")));
	m_images.insert(std::make_pair(ImageName::PIN, LoadGraph("Data/Image/pin.png")));
	m_images.insert(std::make_pair(ImageName::TUTORIAL_ATTACK, LoadGraph("Data/Image/Tutorial_Attack.png")));
	m_images.insert(std::make_pair(ImageName::TUTORIAL_AVOIDANCE, LoadGraph("Data/Image/Tutorial_Avoidance.png")));
	m_images.insert(std::make_pair(ImageName::TUTORIAL_BOSS, LoadGraph("Data/Image/Tutorial_BossKnockDown.png")));
	m_images.insert(std::make_pair(ImageName::TUTORIAL_JUMP, LoadGraph("Data/Image/Tutorial_Jump.png")));
	m_images.insert(std::make_pair(ImageName::TUTORIAL_JUMPATTACK, LoadGraph("Data/Image/Tutorial_JumpAttack.png")));
	m_images.insert(std::make_pair(ImageName::TUTORIAL_LAP, LoadGraph("Data/Image/Tutorial_Lap.png")));
	m_images.insert(std::make_pair(ImageName::TUTORIAL_MAINSTART, LoadGraph("Data/Image/Tutorial_MainStart.png")));
	m_images.insert(std::make_pair(ImageName::TUTORIAL_MOVE, LoadGraph("Data/Image/Tutorial_Move.png")));
	m_images.insert(std::make_pair(ImageName::TUTORIAL_PAUSE, LoadGraph("Data/Image/Tutorial_Pause.png")));
	m_images.insert(std::make_pair(ImageName::TUTORIAL_SUCCESS, LoadGraph("Data/Image/Tutorial_Success.png")));
	m_images.insert(std::make_pair(ImageName::TUTORIAL_TARGET, LoadGraph("Data/Image/Tutorial_Target.png")));
	m_images.insert(std::make_pair(ImageName::TUTORIAL_WEAPON, LoadGraph("Data/Image/Tutorial_WeaponSwitch 1.png")));
	m_images.insert(std::make_pair(ImageName::TUTORIAL_WINDOW, LoadGraph("Data/Image/Tutorial_Window.png")));

	// サウンド
	m_sounds.insert(std::make_pair(SoundName::BGM_BOSS, LoadSoundMem("Data/Sound/BGM_Boss.mp3")));
	m_sounds.insert(std::make_pair(SoundName::BGM_STAGE, LoadSoundMem("Data/Sound/BGM_Stage.mp3")));
	m_sounds.insert(std::make_pair(SoundName::BGM_TITLE, LoadSoundMem("Data/Sound/BGM_Title.mp3")));
	m_sounds.insert(std::make_pair(SoundName::SE_DECISION, LoadSoundMem("Data/Sound/SE_Decision.mp3")));
	m_sounds.insert(std::make_pair(SoundName::SE_DODGE, LoadSoundMem("Data/Sound/SE_Dodge.mp3")));
	m_sounds.insert(std::make_pair(SoundName::SE_HIT, LoadSoundMem("Data/Sound/SE_Hit.mp3")));
	m_sounds.insert(std::make_pair(SoundName::SE_MAPMOVE, LoadSoundMem("Data/Sound/SE_MapMove.mp3")));
	m_sounds.insert(std::make_pair(SoundName::SE_MUTANT_DIVE, LoadSoundMem("Data/Sound/SE_MutantDive.mp3")));
	m_sounds.insert(std::make_pair(SoundName::SE_PAUSE, LoadSoundMem("Data/Sound/SE_Pause.mp3")));
	m_sounds.insert(std::make_pair(SoundName::SE_SWORD1, LoadSoundMem("Data/Sound/SE_Sword1.mp3")));
	m_sounds.insert(std::make_pair(SoundName::SE_SWORD2, LoadSoundMem("Data/Sound/SE_Sword2.mp3")));
	m_sounds.insert(std::make_pair(SoundName::SE_SWORD3, LoadSoundMem("Data/Sound/SE_Sword3.mp3")));

	// フォント
		// フォントのロード＆変更
	LPCSTR font_path = "Data/Font/Senobi-Gothic-Bold.ttf"; // 読み込むフォントファイルのパス
	if (AddFontResourceEx(font_path, FR_PRIVATE, NULL) > 0) {
	}
	else {
		// フォント読込エラー処理
		assert(false);
	}
	font_path = "Data/Font/Senobi-Gothic-Medium.ttf"; // 読み込むフォントファイルのパス
	if (AddFontResourceEx(font_path, FR_PRIVATE, NULL) > 0) {
	}
	else {
		// フォント読込エラー処理
		assert(false);
	}
	font_path = "Data/Font/Senobi-Gothic-Regular.ttf"; // 読み込むフォントファイルのパス
	if (AddFontResourceEx(font_path, FR_PRIVATE, NULL) > 0) {
	}
	else {
		// フォント読込エラー処理
		assert(false);
	}
	m_fonts.insert(std::make_pair(FontName::BOLD_30, CreateFontToHandle("せのびゴシック Bold", 30, 3, DX_FONTTYPE_ANTIALIASING_EDGE_8X8)));
	m_fonts.insert(std::make_pair(FontName::BOLD_20, CreateFontToHandle("せのびゴシック Bold", 20, 3, DX_FONTTYPE_ANTIALIASING_EDGE_8X8)));
	m_fonts.insert(std::make_pair(FontName::BOLD_10, CreateFontToHandle("せのびゴシック Bold", 10, 3, DX_FONTTYPE_ANTIALIASING_EDGE_8X8)));
	m_fonts.insert(std::make_pair(FontName::MEDIUM_30, CreateFontToHandle("せのびゴシック Medium", 30, 3, DX_FONTTYPE_ANTIALIASING_EDGE_8X8)));
	m_fonts.insert(std::make_pair(FontName::MEDIUM_20, CreateFontToHandle("せのびゴシック Medium", 20, 3, DX_FONTTYPE_ANTIALIASING_EDGE_8X8)));
	m_fonts.insert(std::make_pair(FontName::MEDIUM_10, CreateFontToHandle("せのびゴシック Medium", 10, 3, DX_FONTTYPE_ANTIALIASING_EDGE_8X8)));
	m_fonts.insert(std::make_pair(FontName::REGULAR_30, CreateFontToHandle("せのびゴシック Regular", 30, 3, DX_FONTTYPE_ANTIALIASING_EDGE_8X8)));
	m_fonts.insert(std::make_pair(FontName::REGULAR_20, CreateFontToHandle("せのびゴシック Regular", 20, 3, DX_FONTTYPE_ANTIALIASING_EDGE_8X8)));
	m_fonts.insert(std::make_pair(FontName::REGULAR_10, CreateFontToHandle("せのびゴシック Regular", 10, 3, DX_FONTTYPE_ANTIALIASING_EDGE_8X8)));

	// エフェクト
	m_effects.insert(std::make_pair(EffectName::BLESS, LoadEffekseerEffect("Data/Effect/Bless.efkefc", 15.0f)));
	m_effects.insert(std::make_pair(EffectName::CIRCLE, LoadEffekseerEffect("Data/Effect/Circle.efkefc", 15.0f)));
	m_effects.insert(std::make_pair(EffectName::CRY, LoadEffekseerEffect("Data/Effect/Cry.efkefc", 15.0f)));
	m_effects.insert(std::make_pair(EffectName::ENEMYSHOT, LoadEffekseerEffect("Data/Effect/EnemyShot.efkefc", 3.5f)));
	m_effects.insert(std::make_pair(EffectName::ENEMYPOS, LoadEffekseerEffect("Data/Effect/EnemyPos.efkefc", 5.0f)));
	m_effects.insert(std::make_pair(EffectName::FLOOR, LoadEffekseerEffect("Data/Effect/Floor.efkefc", 80.0f)));
	m_effects.insert(std::make_pair(EffectName::FLYSHOT, LoadEffekseerEffect("Data/Effect/FlyShot.efkefc", 10.0f)));
	m_effects.insert(std::make_pair(EffectName::HIT, LoadEffekseerEffect("Data/Effect/EHit.efkefc", 15.0f)));
	m_effects.insert(std::make_pair(EffectName::MAGIC, LoadEffekseerEffect("Data/Effect/Magic.efkefc", 1.0f)));
	m_effects.insert(std::make_pair(EffectName::PLAYERSHOT, LoadEffekseerEffect("Data/Effect/PlayerShot.efkefc", 15.0f)));
	m_effects.insert(std::make_pair(EffectName::PLAYERPOS, LoadEffekseerEffect("Data/Effect/PlayerPos.efkefc", 5.0f)));
	m_effects.insert(std::make_pair(EffectName::SISTEMWALL, LoadEffekseerEffect("Data/Effect/SistemWall.efkefc", 10.0f)));
	m_effects.insert(std::make_pair(EffectName::SKY, LoadEffekseerEffect("Data/Effect/Sky.efkefc", 10.0f)));
	m_effects.insert(std::make_pair(EffectName::SMOKE, LoadEffekseerEffect("Data/Effect/Smoke.efkefc", 10.0f)));
	m_effects.insert(std::make_pair(EffectName::STAN, LoadEffekseerEffect("Data/Effect/Stan.efkefc", 5.0f)));
	m_effects.insert(std::make_pair(EffectName::THUNDER, LoadEffekseerEffect("Data/Effect/Thunder.efkefc", 15.0f)));
	m_effects.insert(std::make_pair(EffectName::TITLEBG, LoadEffekseerEffect("Data/Effect/TitleBg.efkefc", 5.0f)));
	m_effects.insert(std::make_pair(EffectName::TITLEHIDE, LoadEffekseerEffect("Data/Effect/TitleMutantHide.efkefc", 10.0f)));
	m_effects.insert(std::make_pair(EffectName::WAVE, LoadEffekseerEffect("Data/Effect/wave.efk")));
	m_effects.insert(std::make_pair(EffectName::WEAPONMOVE, LoadEffekseerEffect("Data/Effect/WeaponMove.efkefc", 15.0f)));

}

HandleManager::~HandleManager()
{
	// ハンドルの削除
	// モデル
	for (auto& i : m_models)
	{
		MV1DeleteModel(i.second);
	}
	// 画像
	for (auto& i : m_images)
	{
		DeleteGraph(i.second);
	}
	// サウンド
	for (auto& i : m_sounds)
	{
		DeleteSoundMem(i.second);
	}

	// フォント
	for (auto& i : m_fonts)
	{
		DeleteFontToHandle(i.second);
	}

	// エフェクト
	for (auto& i : m_effects)
	{
		DeleteEffekseerEffect(i.second);
	}
}

void HandleManager::UseFont(FontName name)
{
	ChangeFontFromHandle(m_fonts[name]);
}
