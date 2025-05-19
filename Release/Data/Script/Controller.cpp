#include "Controller.h"
#include <cmath>

namespace
{
	// スティックによる入力の最大絶対値が1000であるため、１に統一するためのscale値
	constexpr float kStickScale = 0.001f;
}

InputVec Controller::GetStick(Stick stick)
{
	// LRスティック以外ならば止める
	assert(stick == Stick::STICK_L || stick == Stick::STICK_R);

	GetJoypadDirectInputState(DX_INPUT_PAD1, &m_pad);
	if (stick == Stick::STICK_L)			// Lスティック
	{
		return { m_pad.X * kStickScale, m_pad.Y * kStickScale };
	}
	else if (stick == Stick::STICK_R)	// Rスティック
	{
		return { m_pad.Rx * kStickScale, m_pad.Ry * kStickScale };
	}
	else
	{
		assert(false);
		return { 0, 0 };
	}
}

int Controller::GetStickDown(Stick stick, bool isX, int slope)
{
	// LRスティック以外ならば止める
	assert(stick == Stick::STICK_L || stick == Stick::STICK_R);
	GetJoypadDirectInputState(DX_INPUT_PAD1, &m_pad);

	if (!m_isLastStick)
	{
		if (stick == Stick::STICK_L)			// Lスティック
		{
			if (isX == true)
			{
				if (m_pad.X >= slope || m_pad.X <= -slope)
				{
					m_isLastStick = true;
				}
				return  m_pad.X;
			}
			else
			{
				if (m_pad.Y >= slope || m_pad.Y <= -slope)
				{
					m_isLastStick = true;
				}
				return m_pad.Y;
			}
		}
		else if (stick == Stick::STICK_R)	// Rスティック
		{
			if (isX == true)
			{
				if (m_pad.Rx >= slope || m_pad.Rx <= -slope)
				{
					m_isLastStick = true;
				}
				return  m_pad.Rx;
			}
			else
			{
				if (m_pad.Ry >= slope || m_pad.Ry <= -slope)
				{
					m_isLastStick = true;
				}
				return m_pad.Ry;
			}
		}
		else
		{
			assert(false);
		}
	}
	else
	{
		if (stick == Stick::STICK_L)			// Lスティック
		{
			if (isX == true && m_pad.X < slope && m_pad.X > -slope)
			{
				m_isLastStick = false;
			}
			else if (isX = false && m_pad.Y < slope && m_pad.Y > -slope)
			{
				m_isLastStick = false;
			}
		}
		else if (stick == Stick::STICK_R)	// Rスティック
		{
			if (isX == true && m_pad.Rx < slope && m_pad.Rx > -slope)
			{
				m_isLastStick = false;
			}
			else if (isX = false && m_pad.Ry < slope && m_pad.Ry > -slope)
			{
				m_isLastStick = false;
			}
		}
		else
		{
			assert(false);
		}
	}

	return 0;
}

bool Controller::GetInputStay(Button button)
{
	int num = static_cast<int>(button);

	GetJoypadDirectInputState(DX_INPUT_PAD1, &m_pad);
	if (button <= Button::STICK_R)
	{
		// 最終入力の更新
		if(m_pad.Buttons[num])
			m_LastButton = button;
		// ボタン入力をそのままboolとして返す
		return m_pad.Buttons[num];
	}
	// スライダー
	else if (button == Button::LT)
	{
		if (m_pad.Slider[static_cast<int>(Slider::LT)] > 35000)
		{
			// 最終入力の更新
			m_LastButton = button;
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (button == Button::RT)
	{
		if (m_pad.Slider[static_cast<int>(Slider::RT)] > 35000)
		{
			// 最終入力の更新
			m_LastButton = button;
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

bool Controller::GetInputDown(Button button)
{
	int num = static_cast<int>(button);
	// 指定しているボタン以外ならば止める
	assert(num >= 0 && num < GameData::kButtonNum);


	GetJoypadDirectInputState(DX_INPUT_PAD1, &m_pad);
	// 直前動作で押されていないとき
	if (!m_isLastHit[num])
	{
		// 押し状況をそのままboolにして返す
		m_isLastHit[num] = m_pad.Buttons[num];

		// 最終入力の更新
		if (m_isLastHit[num])
		{
			m_LastButton = button;
		}

		return m_isLastHit[num];
	}
	// 直前動作で押されていた時
	else
	{
		// 現在フレームで押されていなければfalseに
		m_isLastHit[num] = m_pad.Buttons[num];
		// 返り値は関係なくfalse
		return false;
	}
}

bool Controller::GetInputUp(Button button)
{
	int num = static_cast<int>(button);
	// 指定しているボタン以外ならば止める
	assert(num >= 0 && num < GameData::kButtonNum);

	GetJoypadDirectInputState(DX_INPUT_PAD1, &m_pad);
	// 直前動作で押されていたとき
	if (m_isLastHit[num])
	{
		// 押し状況を逆にして返す
		m_isLastHit[num] = m_pad.Buttons[num];
		return !m_isLastHit[num];
	}
	// 直前動作で押されていない時
	else
	{
		// 現在フレームで押されていなければfalseに
		m_isLastHit[num] = m_pad.Buttons[num];
		// 返り値は関係なくfalse
		return false;
	}
}

Button Controller::GetLastInputButton()
{
	return m_LastButton;
}

InputVec Controller::GetPOV()
{
	GetJoypadDirectInputState(DX_INPUT_PAD1, &m_pad);
	InputVec vec;
	if (m_pad.POV[0] != 0xffffffff)
	{
		auto angle = ((static_cast<float>(m_pad.POV[0]) / 100.0f) + 90.0f) / 180.0f * DX_PI_F;
		vec.x = -sin(angle);
		vec.y = cos(angle);
	}
	else
	{
		vec.x = 0;
		vec.y = 0;
	}
	return vec;
}

bool Controller::GetPOVDown(InputPOV pov)
{
	auto num = static_cast<int>(pov) * 4500;
	// m_lastOrder = pov;

	GetJoypadDirectInputState(DX_INPUT_PAD1, &m_pad);
	if (!m_isLastHitPOV)
	{
		if (m_pad.POV[0] == num)
		{
			m_isLastHitPOV = true;
			// m_lastPush = pov;
			return true;
		}
	}
	else
	{
		if (m_pad.POV[0] == 0xffffffff)
		{
			m_isLastHitPOV = false;
		}
	}
	// 最終入力の更新
	// m_lastPush = static_cast<InputPOV>(m_pad.POV[0]/4500);
	
	return false;
}
