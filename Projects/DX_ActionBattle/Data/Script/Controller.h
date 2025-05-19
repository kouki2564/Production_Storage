#pragma once
#include "DxLib.h"
#include "GameData.h"
#include <cassert>

/// <summary>
/// 入力するボタン
/// </summary>
enum class Button
{
	A,
	B,
	X,
	Y,
	LB,
	RB,
	BACK,
	START,
	STICK_L,
	STICK_R,
	LT,
	RT,
	NONE = 99
};

/// <summary>
/// 左右スティック
/// </summary>
enum class Stick
{
	STICK_L,
	STICK_R
};

/// <summary>
/// 左右スライダー
/// </summary>
enum class Slider
{
	LT,
	RT
};

/// <summary>
/// 十字キー（８方向対応）
/// </summary>
enum class InputPOV
{
	UP = 0,
	UP_RIGHT = 1,
	RIGHT = 2,
	DOWN_RIGHT = 3,
	DOWN = 4,
	DOWN_LEFT = 5,
	LEFT = 6,
	UP_LEFT = 7,
	NONE = 0xffffff
};

/// <summary>
/// スティックの入力値
/// </summary>
struct InputVec
{
	float x = 0;
	float y = 0;
};

class Controller
{
public:
	Controller()
	{
		GetJoypadDirectInputState(DX_INPUT_PAD1, &m_pad);
		for (int i = 0; i < GameData::kButtonNum; i++)
		{
			m_isLastHit[i] = true;
		}
		m_isLastHitPOV = false;
		m_isLastStick = true;
	}

	// インスタンス生成
	static Controller& Instance()
	{
		static Controller instance;
		return instance;
	}

	/* スティック */

	/// <summary>
	/// スティック入力
	/// </summary>
	/// <param name="stick">：スティック左右どちらか</param>
	/// <returns>int x (-1000～1000), int y (-1000～1000)</returns>
	InputVec GetStick(Stick stick);

	/// <summary>
	/// スティック入力（XY方向指定の単押し）
	/// </summary>
	/// <param name="stick">：スティック左右どちらか</param>
	/// <param name="isX">：(true:X軸の入力値　false:Y軸の入力値)</param>
	/// <returns>スティック倒し度合(-1000～1000)</returns>
	int GetStickDown(Stick stick, bool isX, int slope);


	/* ボタン */

	/// <summary>
	/// ボタンの押しっぱなし判定（スティックは使えません）
	/// </summary>
	/// <param name="button">ボタン(enum Input)</param>
	/// <returns>true : 押している, false : 押していない</returns>
	bool GetInputStay(Button button);

	/// <summary>
	/// ボタンの単押し（スティックは使えません）
	/// </summary>
	/// <param name="button">ボタン(enum Input)</param>
	/// <returns>true : 押している, false : 押しっぱなし or 押していない</returns>
	bool GetInputDown(Button button);

	/// <summary>
	/// ボタン押し状態→離し（スティックは使えません）
	/// </summary>
	/// <param name="button">ボタン(enum Input)</param>
	/// <returns>true : 離した, false : 押しっぱなし or 押していない</returns>
	bool GetInputUp(Button button);

	/// <summary>
	/// ボタンの最終入力
	/// </summary>
	/// <returns></returns>
	Button GetLastInputButton();

	/// <summary>
	/// ボタンの最終入力のリセット
	/// </summary>
	void ResetLastInputButton() { m_LastButton = Button::NONE; }

	/* 十字キー */

	/// <summary>
	/// 十字キー入力
	/// </summary>
	/// <returns>int型(x,y)で返す</returns>
	InputVec GetPOV();

	/// <summary>
	/// 十字キー入力（単押し）
	/// </summary>
	/// <param name="pov">十字キーの方向</param>
	/// <returns>true : 押している, false : 押しっぱなし or 押していない</returns>
	bool GetPOVDown(InputPOV pov);

private:

	// 各ボタンの直前フレームの押し状況
	bool m_isLastHit[GameData::kButtonNum];
	// 十字キーの直前フレームの押し状況
	bool m_isLastHitPOV;
	// スティックの直前フレームの倒し状況
	bool m_isLastStick;
	// 入力用のパッド変数
	DINPUT_JOYSTATE m_pad;

	Button m_LastButton = Button::NONE;

	// InputPOV m_lastOrder = InputPOV::NONE;
	// InputPOV m_lastPush = InputPOV::NONE;
};

