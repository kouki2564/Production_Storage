#include "SceneManager.h"
#include "TitleScene.h"
#include "TutorialScene.h"
#include "GameScene.h"

#include "HandleManager.h"

#ifdef _DEBUG
#include "debugScene.h"
#endif // _DEBUG


SceneManager::SceneManager() :
	m_isGameEnd(false)
{
	SoundManager::Instance().RegisterSound("Title", HandleManager::Instance().GetSoundHandle(SoundName::BGM_TITLE), 200);
	SoundManager::Instance().RegisterSound("Stage", HandleManager::Instance().GetSoundHandle(SoundName::BGM_STAGE), 120);
	SoundManager::Instance().RegisterSound("Boss", HandleManager::Instance().GetSoundHandle(SoundName::BGM_BOSS), 120);

	SoundManager::Instance().RegisterSound("Hit", HandleManager::Instance().GetSoundHandle(SoundName::SE_HIT), 150);
	SoundManager::Instance().RegisterSound("decision", HandleManager::Instance().GetSoundHandle(SoundName::SE_DECISION), 150);

	EffectManager::Instance().RegisterEffect("SistemWall_Flont", HandleManager::Instance().GetEffectHandle(EffectName::SISTEMWALL), VGet(0, 0, 0), VGet(0, 0, 0));
	EffectManager::Instance().RegisterEffect("SistemWall_Back", HandleManager::Instance().GetEffectHandle(EffectName::SISTEMWALL), VGet(0, 0, 0), VGet(0, 0, 0));
	EffectManager::Instance().RegisterEffect("SistemWall_Right", HandleManager::Instance().GetEffectHandle(EffectName::SISTEMWALL), VGet(0, 0, 0), VGet(0, 0, 0));
	EffectManager::Instance().RegisterEffect("SistemWall_Left", HandleManager::Instance().GetEffectHandle(EffectName::SISTEMWALL), VGet(0, 0, 0), VGet(0, 0, 0));


#ifdef _DEBUG
	m_pScene = std::make_unique<GameScene>();

#else
	m_pScene = std::make_unique<TitleScene>();

#endif // _DEBUG
}

SceneManager::~SceneManager()
{
}

void SceneManager::Init()
{
	m_pScene->InitScene();
}

void SceneManager::Update()
{
	m_pScene->UpdateScene();
	TransScene();
}

void SceneManager::TransScene()
{
	// シーン遷移
	if (m_pScene->GetIsMoveScene())
	{
		int weaponNum = m_pScene->GetWeaponNum();
		// どのシーンに向かうかの決定
		if (m_pScene->GetNextSceneNum() == Scene::TITLE)
		{
			m_pScene.reset();
			m_pScene.release();
			m_pScene = std::make_unique<TitleScene>();
		}
		else if (m_pScene->GetNextSceneNum() == Scene::TUTORIAL)
		{
			m_pScene.reset();
			m_pScene.release();
			m_pScene = std::make_unique<TutorialScene>();
			m_pScene->SetWeaponNum(weaponNum);
		}
		else if (m_pScene->GetNextSceneNum() == Scene::GAME)
		{
			m_pScene.reset();
			m_pScene.release();
			m_pScene = std::make_unique<GameScene>();
			m_pScene->SetWeaponNum(weaponNum);
		}
#ifdef _DEBUG
		else if (m_pScene->GetNextSceneNum() == Scene::DEBUG)
		{
			m_pScene.reset();
			m_pScene.release();
			m_pScene = std::make_unique<DebugScene>();
		}
#endif // _DEBUG
		else
		{
			assert(false);
		}
		m_pScene->InitScene();
	}
}
