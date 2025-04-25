#pragma once
#include <vector>
#include "Controller.h"

struct Image
{
	// 画像のハンドル
	int handle;
	// 画像の座標
	int startX;
	int startY;
	int endX;
	int endY;
	// 画像を描画するかどうか
	bool isDraw = true;
};

struct UpgradeData
{
	// 強化ポイント
	int upgradePoint;
	// 攻撃レベル
	int attackLevel;
	// 防御レベル
	int defenseLevel;
	// 速度レベル
	int speedLevel;
};

enum class PauseMenuState
{
	STATUS,
	SOUND,
	EXIT
};

class PauseMenu
{
public:
	PauseMenu();
	~PauseMenu();

	
	void Update();
	void Draw();

	// ポーズメニューの表示切り替え
	void SwitchPause();

	bool GetIsEndPause() { return !m_isPause; }

	int GetUpgrade();

	// 強化ポイントの共有
	void SetUpgradePoint(UpgradeData data) 
	{ 
		m_upgradePoint = data.upgradePoint;
		m_attackLevel = data.attackLevel;
		m_defenseLevel = data.defenseLevel;
		m_speedLevel = data.speedLevel;
	}

	UpgradeData GetUpgradePoint()
	{
		UpgradeData data;
		data.upgradePoint = m_upgradePoint;
		data.attackLevel = m_attackLevel;
		data.defenseLevel = m_defenseLevel;
		data.speedLevel = m_speedLevel;
		return data;
	}

	bool GetIsEndGame() { return m_isEndGame; }

private:

	// ポーズメニューの展開
	void PopUp();
	// ポーズメニューの縮小
	void Close();


	// ポーズ画面展開フラグ
	bool m_isSwitchPause = false;

	// メニューの選択
	void SelectMenu();

	void DrawStrings();

	// ポーズ中かどうか
	bool m_isPause = false;

	// ポーズメニューの表示フラグ
	bool m_isPopUp = false;

	// 操作可能フラグ
	bool m_isOpe = false;

	// ポーズメニューの選択フラグ
	bool m_isSelect = false;
	// ポーズメニューの状態
	PauseMenuState m_pauseMenuState = PauseMenuState::STATUS;

	// ポーズメニューのハンドル
	std::vector<Image> m_imageData;

	// ポーズメニューの座標
	int m_posX;
	int m_posY;

	// カーソル位置
	int m_selectCursorNum = 0;
	int m_menuCursorNum = 0;

	// プレイヤーの強化関連
	int m_upgradePoint = 0;
	bool m_isUpgrade = false;
	int m_attackLevel = 1;
	int m_defenseLevel = 1;
	int m_speedLevel = 1;

	// ゲーム終了フラグ
	bool m_isEndGame = false;
};

