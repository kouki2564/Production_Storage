#pragma once
#include <map>
#include <string>
#include <memory>
#include "ObjectBase.h"
#include "Stage.h"
#include "Camera.h"
#include "Light.h"
#include "SceneStateBase.h"
#include "Controller.h"
#include "PanelManager.h"


class PauseMenu;

enum class Scene
{
	DEBUG,
	TITLE,
	TUTORIAL,
	GAME
};

class SceneBase
{
public:
	SceneBase();
	virtual ~SceneBase();

	/// <summary>
	/// シーン移動フラグ取得
	/// </summary>
	/// <returns>true：移動　false：特になし</returns>
	bool GetIsMoveScene() { return m_isMoveScene; }

	/// <summary>
	/// シーン移動先取得
	/// </summary>
	/// <returns>移動先のScene</returns>
	Scene GetNextSceneNum() { return m_nextScene; }

	virtual void InitScene() = 0;
	virtual void UpdateScene() = 0;

	int GetWeaponNum() { return m_weaponSelect; }
	void SetWeaponNum(int num) { m_weaponSelect = num; }

protected:

	virtual void InitImage() = 0;

	// SceneStateの更新
	void ChangeState(SceneState state);

	/// <summary>
	/// フェードイン処理
	/// </summary>
	/// <param name="value">フェード増加値</param>
	/// <returns>完了でtrue</returns>
	bool FadeIn(int value);

	/// <summary>
	/// フェードスキップ
	/// </summary>
	/// <param name="value">フェード変更後の値</param>
	void FadeSkip(int value);

	/// <summary>
	/// フェードアウト処理
	/// </summary>
	/// <param name="value">フェード減少値</param>
	/// <returns>完了でtrue</returns>
	bool FadeOut(int value);

	virtual void ObjectsUpdate() = 0;

	virtual void CollisionUpdate() = 0;

	virtual void CameraUpdate() = 0;

	Light m_light;

	Scene m_nextScene;
	
	Camera m_camera;

	bool m_isMoveScene = false;

	int m_setEnemyNum = 0;

	// 元のオブジェクト情報


	// 武器選択
	int m_weaponSelect = 0;
	

	std::unique_ptr<SceneStateBase> m_sceneState;

	// シーン進行関連のタイマー
	int m_eventTimer = 0;

	// ポーズ画面
	std::shared_ptr<PauseMenu> m_pPause;

private:
	// フェード処理用のアルファ値
	int m_fadeAlpha = 255;
};

