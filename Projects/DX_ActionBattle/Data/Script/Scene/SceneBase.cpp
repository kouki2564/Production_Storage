#include "SceneBase.h"

// SceneState
#include "SceneStart.h"
#include "SceneMain.h"
#include "SceneEnd.h"
#include "ScenePause.h"

SceneBase::SceneBase()
{
	m_nextScene = Scene::DEBUG;

	m_isMoveScene = false;
}

SceneBase::~SceneBase()
{
}

void SceneBase::ChangeState(SceneState state)
{
	if (state == SceneState::START)
	{
		m_sceneState.reset();
		m_sceneState = std::make_unique<SceneStart>();
	}
	else if (state == SceneState::MAIN)
	{
		m_sceneState.reset();
		m_sceneState = std::make_unique<SceneMain>();
	}
	else if (state == SceneState::END)
	{
		m_sceneState.reset();
		m_sceneState = std::make_unique<SceneEnd>();
	}
	else if (state == SceneState::PAUSE)
	{
		m_sceneState.reset();
		m_sceneState = std::make_unique<ScenePause>();
	}
}

bool SceneBase::FadeIn(int value)
{
	m_fadeAlpha -= value;

	// ブラックアウト用
	if (m_fadeAlpha > 0)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_fadeAlpha);
		DrawBox(0, 0, GameData::kScreenWidth, GameData::kScreenHeight, 0x000000, true);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	}

	if (m_fadeAlpha >= 0)
	{

		return false;
	}
	else
	{

		return true;
	}
}

void SceneBase::FadeSkip(int value)
{
	m_fadeAlpha = value;
}

bool SceneBase::FadeOut(int value)
{
	m_fadeAlpha += value;

	if (m_fadeAlpha > 0)
	{
		// ブラックアウト用
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_fadeAlpha);
		DrawBox(0, 0, GameData::kScreenWidth, GameData::kScreenHeight, 0x000000, true);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	}

	if (m_fadeAlpha <= 255)
	{

		return false;
	}
	else
	{

		return true;
	}
}
