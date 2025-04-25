#pragma once
#include "SceneBase.h"
#include "EnemyBase.h"
#include <vector>

class TutorialScene :
    public SceneBase
{
public:
	TutorialScene();
	~TutorialScene();
	void InitScene();
	// 全体処理
	void UpdateScene();
	// ステージの更新
	void UpdateStage();

private:
	// 画像データの準備
	void InitImage();
	// オブジェクトの更新
	void ObjectsUpdate();
	// 当たり判定
	void CollisionUpdate();
	// チュートリアルのアップデート
	void TutorialUpdate();
	// チュートリアルの描画
	void TutorialDraw();
	// カメラ移動
	void CameraUpdate();
	// マップ移動
	void ChackMoveMap();
	// 現行中のステージデータの削除
	void DeleteStatgeData();
	// 移行先のステージデータの準備
	void InitStageData(int stageLine, int stageRow);
	// ステージ内に置くエネミーの確認
	void InitEnemyData(int stageLine, int stageRow);
	// ステージ内に置くエネミーの実準備
	void SetEnemyData(int stageLine, int stageRow);
	// プレイヤーのHPバーの位置調整
	void PlayerStatusPop();
	void DrawParameter();
	void DrawFloor();
	void EndingUpdate();
	void EndingDraw();


	// 現在ステージ番号
	int m_nowStageNum[2];

	// 移動方向
	int m_moveRoomDir = -1;


	// 配置するオブジェクト
	std::map<std::string, std::unique_ptr<ObjectBase>> m_objects;

	std::map<std::string, std::unique_ptr<Stage>> m_stage;

	// プレイヤーの配置座標
	VECTOR m_nextPlayerPos;
	VECTOR m_nextPlayerDir;

	// プレイヤーの1フレーム前のHP割合
	float m_lastPlayerHpRate = 1.0f;

	// ステージ移動フラグ
	bool m_isMoveMap[GameData::kTutorialMaxLine];

	// エネミーデータ
	std::vector<EnemyKind> m_enemyData;

	// プレイヤーの強化ポイント
	int m_upgradePoint = 0;

	//std::vector<std::unique_ptr<WeaponBase>> m_weaponModel;

	// チュートリアルステージの削除フレーム番号たち
	std::vector<int> m_invisibleFrame;
	// チュートリアルの表示アルファ
	int m_tutorialAlpha = 0;
	// ゲーム終了フラグ
	bool m_isLoseEnd = false;
	bool m_isWinEnd = false;

	int m_endingTimer = 0;
	int m_endingAlpha = 0;

	// 回避チュートリアル用
	bool m_isClearDodge = false;

	// ターゲットチュートリアル用
	bool m_isClearTarget = false;

	// チュートリアル成功時のサクセス表示関係
	// サクセス表示のスケール
	float m_sucsessScale = 0.0f;
	// サクセス表示のアルファ
	int m_sucsessAlpha = 0;

	// チュートリアルのウインドウのｙサイズ
	float m_tutorialWindowScaleY = 0.0f;
};

