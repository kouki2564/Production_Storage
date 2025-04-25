#pragma once
#include <memory>
#include "Controller.h"
#include "EffectManager.h"
#include "SoundManager.h"
// #include "DataFile.h"

class SceneBase;
class Light;

class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	//インスタンスを返す
	static SceneManager& Instance() {
		static SceneManager instance;
		return instance;
	}

	void Init();
	void Update();


	/// <summary>
	/// ゲーム全体終了フラグ
	/// </summary>
	/// <returns>true：終了　false：スルー</returns>
	bool GetisGameEnd() const { return m_isGameEnd; }

private:
	/// <summary>
	/// シーン遷移
	/// </summary>
	void TransScene();

	std::unique_ptr<SceneBase> m_pScene;

	bool m_isGameEnd;
};

