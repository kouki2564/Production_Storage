#pragma once
#include <string>
#include <map>

enum class ModelName
{
	PLAYER,
	MUTANT,
	ENEMY1,
	ENEMY2,
	ENEMY3,
	ENEMY4,
	SWORD,
	AXE,
	NORMALSTAGE,
	BOSSSTAGE
};

enum class ImageName
{
	BARBACK,
	BARGREEN,
	BARRED,
	ENEMYBAR,
	ENEMYBARBACK,
	STATUS,
	NUMBER,
	PAUSE,
	PIN,
	TUTORIAL_ATTACK,
	TUTORIAL_AVOIDANCE,
	TUTORIAL_BOSS,
	TUTORIAL_JUMP,
	TUTORIAL_JUMPATTACK,
	TUTORIAL_LAP,
	TUTORIAL_MAINSTART,
	TUTORIAL_MOVE,
	TUTORIAL_PAUSE,
	TUTORIAL_SUCCESS,
	TUTORIAL_TARGET,
	TUTORIAL_WEAPON,
	TUTORIAL_WINDOW
};

enum class SoundName
{
	BGM_BOSS,
	BGM_STAGE,
	BGM_TITLE,
	SE_DECISION,
	SE_DODGE,
	SE_HIT,
	SE_MAPMOVE,
	SE_MUTANT_DIVE,
	SE_PAUSE,
	SE_SWORD1,
	SE_SWORD2,
	SE_SWORD3
};

enum class FontName
{
	BOLD_30,
	BOLD_20,
	BOLD_10,
	MEDIUM_30,
	MEDIUM_20,
	MEDIUM_10,
	REGULAR_30,
	REGULAR_20,
	REGULAR_10
};

enum class EffectName
{
	BLESS,
	CIRCLE,
	CRY,
	EHIT,
	ENEMYSHOT,
	ENEMYPOS,
	FLOOR,
	FLYSHOT,
	HIT,
	MAGIC,
	PLAYERSHOT,
	PLAYERPOS,
	SISTEMWALL,
	SKY,
	SMOKE,
	STAN,
	THUNDER,
	TITLEBG,
	TITLEHIDE,
	WAVE,
	WEAPONMOVE
};

class HandleManager
{
public:
	// コンストラクタで一括ロード
	HandleManager();
	// デストラクタで一括解放
	~HandleManager();

	// インスタンスを返す
	static HandleManager& Instance() 
	{
		static HandleManager instance;
		return instance;
	}

	/* 各種ハンドルの取得 */
	// モデルハンドルの取得 
	int GetModelHandle(ModelName name) { return m_models[name]; }
	// 画像ハンドルの取得
	int GetImageHandle(ImageName name) { return m_images[name]; }
	// サウンドハンドルの取得
	int GetSoundHandle(SoundName name) { return m_sounds[name]; }
	// フォントの使用
	void UseFont(FontName name);
	// エフェクトハンドルの取得
	int GetEffectHandle(EffectName name) { return m_effects[name]; }

private:
	/* 各種ハンドル */
	// モデルハンドル
	std::map<ModelName, int> m_models;
	// 画像ハンドル
	std::map<ImageName, int> m_images;
	// サウンドハンドル
	std::map<SoundName, int> m_sounds;
	// フォントハンドル
	std::map<FontName, int> m_fonts;
	// エフェクトハンドル
	std::map<EffectName, int> m_effects;
};

