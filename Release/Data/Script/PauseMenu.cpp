#include "PauseMenu.h"
#include "DxLib.h"
#include "GameData.h"

#include "HandleManager.h"
#include "SoundManager.h"
#include "HandleManager.h"

namespace
{
	constexpr int kSelectNum = 3;
	constexpr int kStatusMenuNum = 3;
	constexpr int kSoundMenuNum = 3;
	constexpr float kSelectPosX = GameData::kScreenWidth * 0.1f;
	constexpr float kSelectPosY = GameData::kScreenHeight * 0.2f;

	constexpr float kMenuPosX = GameData::kScreenWidth * 0.4f;
	constexpr float kMenuPosY = GameData::kScreenHeight * 0.2f;
}

PauseMenu::PauseMenu()
{
	// 各フラグの初期化
	m_isSwitchPause = false;
	m_isPause = false;
	m_isPopUp = false;
	m_isSelect = false;

	Image initData;
	initData.handle = HandleManager::Instance().GetImageHandle(ImageName::PAUSE);
	initData.startX = 0;
	initData.startY = GameData::kScreenHalfHeight;
	initData.endX = GameData::kScreenWidth;
	initData.endY = GameData::kScreenHalfHeight;
	initData.isDraw = false;
	m_imageData.push_back(initData);
}

PauseMenu::~PauseMenu()
{
}

void PauseMenu::Update()
{
	// ポーズメニューの表示
	if (m_isSwitchPause)
	{
		// ポーズ画面が表示されていないとき
		if (!m_isPopUp)
		{
			// ポーズ画面展開
			PopUp();
		}
		// ポーズ画面が表示されているとき
		else
		{
			// ポーズ画面縮小
			Close();
		}
	}
	// ポーズ画面の展開終了時の処理
	else if (m_isOpe)
	{
		// ポーズ画面処理
		SelectMenu();
	}
}

void PauseMenu::Draw()
{
	// 背景を暗くする
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBox(0, 0, GameData::kScreenWidth, GameData::kScreenHeight, 0x000000, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

	// ウインドウの描画
	if (m_imageData[0].isDraw)
	{
		DrawExtendGraph(m_imageData[0].startX, m_imageData[0].startY, m_imageData[0].endX, m_imageData[0].endY, m_imageData[0].handle, TRUE);
	}

	DrawStrings();
}

void PauseMenu::SwitchPause()
{
	if (!m_isSwitchPause)
		m_isSwitchPause = true;
}

int PauseMenu::GetUpgrade()
{
	if (m_isUpgrade)
	{
		m_isUpgrade = false;

		// 強化パラメータの選択

		
	}
	else
	{
		return -1;
	}
}

void PauseMenu::PopUp()
{
	// ポーズ画面の開始
	m_isPause = true;
	// ポーズ画面の表示
	m_imageData[0].isDraw = true;
	// ポーズ画面展開
	if (m_imageData[0].startY > 0)
	{
		m_imageData[0].startY -= 20;
		if (m_imageData[0].startY <= 0)
		{
			m_imageData[0].startY = 0;
		}
	}

	if (m_imageData[0].endY < GameData::kScreenHeight)
	{
		m_imageData[0].endY += 20;
		if (m_imageData[0].endY >= GameData::kScreenHeight)
		{
			m_imageData[0].endY = GameData::kScreenHeight;
		}
	}

	// 展開終了後機能開始
	if (m_imageData[0].startY == 0 && m_imageData[0].endY == GameData::kScreenHeight)
	{
		// ポーズ画面の使用開始
		m_isPopUp = true;
		// 操作開始
		m_isOpe = true;
		m_isSwitchPause = false;
	}
}

void PauseMenu::Close()
{
	// 操作終了
	m_isOpe = false;

	m_selectCursorNum = 0;
	m_menuCursorNum = 0;
	m_isSelect = false;


	// ポーズ画面の縮小
	if (m_imageData[0].startY < GameData::kScreenHalfHeight)
	{
		m_imageData[0].startY += 20;
		if (m_imageData[0].startY >= GameData::kScreenHalfHeight)
		{
			m_imageData[0].startY = GameData::kScreenHalfHeight;
		}
	}
	
	if (m_imageData[0].endY > GameData::kScreenHalfHeight)
	{
		m_imageData[0].endY -= 20;
		if (m_imageData[0].endY <= GameData::kScreenHalfHeight)
		{
			m_imageData[0].endY = GameData::kScreenHalfHeight;
		}
	}

	// ポーズ画面を非表示にする
	if (m_imageData[0].startY == GameData::kScreenHalfHeight && m_imageData[0].endY == GameData::kScreenHalfHeight)
	{
		// ポーズ画面の表示終了
		m_isPopUp = false;
		m_imageData[0].isDraw = false;
		m_isSwitchPause = false;

		// ポーズ画面の終了
		m_isPause = false;

	}
}

void PauseMenu::SelectMenu()
{
	// 概要の選択
	if (m_isOpe)
	{
		// カーソル移動量
		int cursorMove = 0;
		// パッドによる上下の選択
		if (Controller::Instance().GetPOVDown(InputPOV::UP))
		{
			// 上にカーソル移動値
			cursorMove = -1;

		}
		if (Controller::Instance().GetPOVDown(InputPOV::DOWN))
		{
			// 下にカーソル移動値
			cursorMove = 1;
		}
		

		// 内容の選択
		if (!m_isSelect)
		{
			// カーソル位置の指定
			m_selectCursorNum += cursorMove;
			if (m_selectCursorNum == 1) m_selectCursorNum += cursorMove;
			if (m_selectCursorNum < 0) m_selectCursorNum = kSelectNum - 1;
			if (m_selectCursorNum >= kSelectNum) m_selectCursorNum = 0;

			// 右ウインドウ準備
			if (m_selectCursorNum == 0)
			{
				m_pauseMenuState = PauseMenuState::STATUS;
				// 表示物の準備
			}
			else if (m_selectCursorNum == 1)
			{
				m_pauseMenuState = PauseMenuState::SOUND;
				// 表示物の準備
			}
			else if (m_selectCursorNum == 2)
			{
				m_pauseMenuState = PauseMenuState::EXIT;
				// 表示物の準備
			}

			// 概要の決定
			if (Controller::Instance().GetInputDown(Button::A))
			{
				m_isSelect = true;
				// SoundManager::Instance().OnePlay("decision");

				// 追加の画面表示の準備
				if (m_pauseMenuState == PauseMenuState::EXIT)
				{
					// 終了選択画面の表示準備
				}

			}
			// ポーズ解除
			if (Controller::Instance().GetInputDown(Button::B))
			{
				m_isSwitchPause = true;
			}
		}
		else
		{
			m_menuCursorNum += cursorMove;
			// ステータス確認・強化時
			if (m_pauseMenuState == PauseMenuState::STATUS)
			{
				// カーソル箇所の移動
				if (m_menuCursorNum < 0) m_menuCursorNum = kStatusMenuNum - 1;
				if (m_menuCursorNum >= kStatusMenuNum) m_menuCursorNum = 0;

				// 選択
				if (Controller::Instance().GetInputDown(Button::A))
				{
					// SoundManager::Instance().OnePlay("decision");
					// 強化ポイントがあれば、パラメータ強化フラグを立てる
					if (m_upgradePoint > 0)
					{
						m_upgradePoint--;
						if (m_menuCursorNum == 0) m_attackLevel++;
						if (m_menuCursorNum == 1) m_defenseLevel++;
						if (m_menuCursorNum == 2) m_speedLevel++;
						m_isUpgrade = true;
					}
				}
			}
			// サウンド調整時
			else if (m_pauseMenuState == PauseMenuState::SOUND)
			{
				if (m_menuCursorNum < 0) m_menuCursorNum = kSoundMenuNum - 1;
				if (m_menuCursorNum >= kSoundMenuNum) m_menuCursorNum = 0;
			}
			else if (m_pauseMenuState == PauseMenuState::EXIT)
			{
				if (Controller::Instance().GetInputDown(Button::A))
				{
					// 
					// SoundManager::Instance().OnePlay("decision");
					// ゲーム終了
					m_isEndGame = true;
				}
			}
			
			// ポーズ解除時
			if (Controller::Instance().GetInputDown(Button::B))
			{
				m_isSelect = false;
			}
		}
	}
}

void PauseMenu::DrawStrings()
{
	if (!m_isSwitchPause)
	{
		// フォント変更
		HandleManager::Instance().UseFont(FontName::MEDIUM_30);

		auto color = GetColor(100, 100, 100);
		auto darkColor = GetColor(20, 20, 20);
		auto selectColor = GetColor(200, 200, 0);
		auto edgeColor = GetColor(255, 255, 255);
		auto darkEdgeColor = GetColor(100, 100, 100);

		// 左ウインドウ内容
		if (m_isPopUp)
		{
			if (!m_isSelect)
			{
				// メニュー概要の描画
				if (m_selectCursorNum == 0)
				{
					DrawFormatString2F(kSelectPosX, kSelectPosY, selectColor, edgeColor, "ステータス");
					// DrawFormatString2F(kSelectPosX, kSelectPosY + 100, color, edgeColor, "サウンド");
					DrawFormatString2F(kSelectPosX, kSelectPosY + 200, color, edgeColor, "終了");
				}
				else if (m_selectCursorNum == 1)
				{
					DrawFormatString2F(kSelectPosX, kSelectPosY, color, edgeColor, "ステータス");
					// DrawFormatString2F(kSelectPosX, kSelectPosY + 100, selectColor, edgeColor, "サウンド");
					DrawFormatString2F(kSelectPosX, kSelectPosY + 200, color, edgeColor, "終了");
				}
				else if (m_selectCursorNum == 2)
				{
					DrawFormatString2F(kSelectPosX, kSelectPosY, color, edgeColor, "ステータス");
					// DrawFormatString2F(kSelectPosX, kSelectPosY + 100, color, edgeColor, "サウンド");
					DrawFormatString2F(kSelectPosX, kSelectPosY + 200, selectColor, edgeColor, "終了");
				}
			}
			else
			{
				// メニュー概要の描画
				if (m_selectCursorNum == 0)
				{
					DrawFormatString2F(kSelectPosX, kSelectPosY, selectColor, edgeColor, "ステータス");
					// DrawFormatString2F(kSelectPosX, kSelectPosY + 100, darkColor, darkEdgeColor, "サウンド");
					DrawFormatString2F(kSelectPosX, kSelectPosY + 200, darkColor, darkEdgeColor, "終了");
				}
				else if (m_selectCursorNum == 1)
				{
					DrawFormatString2F(kSelectPosX, kSelectPosY, darkColor, darkEdgeColor, "ステータス");
					// DrawFormatString2F(kSelectPosX, kSelectPosY + 100, selectColor, edgeColor, "サウンド");
					DrawFormatString2F(kSelectPosX, kSelectPosY + 200, darkColor, darkEdgeColor, "終了");
				}
				else if (m_selectCursorNum == 2)
				{
					DrawFormatString2F(kSelectPosX, kSelectPosY, darkColor, darkEdgeColor, "ステータス");
					// DrawFormatString2F(kSelectPosX, kSelectPosY + 100, darkColor, darkEdgeColor, "サウンド");
					DrawFormatString2F(kSelectPosX, kSelectPosY + 200, selectColor, edgeColor, "終了");
				}
			}
		}


		// 右ウインドウ内容
		if (m_pauseMenuState == PauseMenuState::STATUS)
		{
			if (m_imageData[0].isDraw)
			{
				if (!m_isSelect)
				{
					// ステータス内容の描画
					DrawFormatString2F(kMenuPosX, kMenuPosY, darkColor, darkEdgeColor, "強化ポイント：%d", m_upgradePoint);
					DrawFormatString2F(kMenuPosX, kMenuPosY + 160, darkColor, darkEdgeColor, "攻撃力 ： Lv. %d", m_attackLevel);
					DrawFormatString2F(kMenuPosX, kMenuPosY + 240, darkColor, darkEdgeColor, "防御力 ： Lv. %d", m_defenseLevel);
					DrawFormatString2F(kMenuPosX, kMenuPosY + 320, darkColor, darkEdgeColor, "素早さ ： Lv. %d", m_speedLevel);
				}
				else if (m_isSelect)
				{
					if (m_upgradePoint > 0)
						DrawFormatString2F(kMenuPosX, kMenuPosY, color, edgeColor, "強化ポイント：%d", m_upgradePoint);
					else
						DrawFormatString2F(kMenuPosX, kMenuPosY, darkColor, darkEdgeColor, "強化ポイント：%d", m_upgradePoint);

					// ステータス内容の描画
					if (m_menuCursorNum == 0)
					{
						if (m_upgradePoint > 0)
							DrawFormatString2F(kMenuPosX, kMenuPosY + 160, selectColor, edgeColor, "攻撃力 ： Lv. %d → %d", m_attackLevel, m_attackLevel + 1);
						else
							DrawFormatString2F(kMenuPosX, kMenuPosY + 160, selectColor, edgeColor, "攻撃力 ： Lv. %d", m_attackLevel);
						DrawFormatString2F(kMenuPosX, kMenuPosY + 240, color, edgeColor, "防御力 ： Lv. %d", m_defenseLevel);
						DrawFormatString2F(kMenuPosX, kMenuPosY + 320, color, edgeColor, "素早さ ： Lv. %d", m_speedLevel);
					}
					else if (m_menuCursorNum == 1)
					{
						DrawFormatString2F(kMenuPosX, kMenuPosY + 160, color, edgeColor, "攻撃力 ： Lv. %d", m_attackLevel);
						if (m_upgradePoint > 0)
							DrawFormatString2F(kMenuPosX, kMenuPosY + 240, selectColor, edgeColor, "防御力 ： Lv. %d → %d", m_defenseLevel, m_defenseLevel + 1);
						else
							DrawFormatString2F(kMenuPosX, kMenuPosY + 240, selectColor, edgeColor, "防御力 ： Lv. %d", m_defenseLevel);
						DrawFormatString2F(kMenuPosX, kMenuPosY + 320, color, edgeColor, "素早さ ： Lv. %d", m_speedLevel);
					}
					else if (m_menuCursorNum == 2)
					{
						DrawFormatString2F(kMenuPosX, kMenuPosY + 160, color, edgeColor, "攻撃力 ： Lv. %d", m_attackLevel);
						DrawFormatString2F(kMenuPosX, kMenuPosY + 240, color, edgeColor, "防御力 ： Lv. %d", m_defenseLevel);
						if (m_upgradePoint > 0)
							DrawFormatString2F(kMenuPosX, kMenuPosY + 320, selectColor, edgeColor, "素早さ ： Lv. %d → %d", m_speedLevel, m_speedLevel + 1);
						else
							DrawFormatString2F(kMenuPosX, kMenuPosY + 320, selectColor, edgeColor, "素早さ ： Lv. %d", m_speedLevel);
					}
				}
			}
		}
		else if (m_pauseMenuState == PauseMenuState::SOUND)
		{
			if (m_imageData[0].isDraw)
			{
				// サウンド調整内容の描画
				DrawFormatString2F(kMenuPosX, kMenuPosY, color, edgeColor, "BGM：");
				DrawFormatString2F(kMenuPosX, kMenuPosY + 80, color, edgeColor, " SE：");
			}
		}
		else if (m_pauseMenuState == PauseMenuState::EXIT)
		{
			if (m_imageData[0].isDraw)
			{
				// タイトルに戻る内容の描画
				if (!m_isSelect)
					DrawFormatString2F(kMenuPosX, kMenuPosY, darkColor, darkEdgeColor, "タイトルに戻る");
				else
					DrawFormatString2F(kMenuPosX, kMenuPosY, selectColor, edgeColor, "タイトルに戻る");
			}
		}
	}
}