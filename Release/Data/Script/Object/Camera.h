#pragma once
#include "ObjectBase.h"
#include "Controller.h"
#include "Quaternion.h"

// ビューモード
enum class ViewMode
{
	// 見下ろし視点
	UP,
	// TPS視点
	TPS,
	// クォータービュー視点
	QUARTER
};

class Camera
{
public:
	Camera();
	~Camera();

	void Init(VECTOR firstPos);

	// 主にカメラやTargetの座標更新
	void Update();

	/// <summary>
	/// カメラのターゲット位置の設定
	/// </summary>
	/// <param name="playerPos">プレイヤー座標</param>
	/// <param name="targetPos">プレイヤーとともに定める座標(主にボス)</param>
	void SetTargetPos(VECTOR playerPos, VECTOR targetPos);

	// 座標更新
	void UpdatePos();

	/// <summary>
	/// カメラの視点変更
	/// </summary>
	/// <param name="mode">モード3種のどれか</param>
	/// <param name="playerDir">プレイヤーの向いている方向</param>
	void ChangeViewMode(ViewMode mode, VECTOR playerPos, VECTOR playerDir);

	// ターゲットロックするかどうかの切り替え
	void ChangeIsTarget();

	// 今現在カメラのターゲットをロックしているかの獲得
	bool GetIsTargetRock() { return m_isTargetRock; }

	// ビューモードを獲得
	ViewMode GetViewMode() { return m_view; }

	// カメラの前方向ベクトルの獲得
	VECTOR GetCameraDir();

	// カメラの座標を獲得
	VECTOR GetPos() { return m_pos; }

	// カメラの注視点座標を獲得
	VECTOR GetTargetPos() { return m_viewPos; }

	// カメラの回転を行うかどうか
	void SetIsCameraRotate(bool isRotate) { m_isRotate = isRotate; }

	void SetVibration(int frame);

	void SetTargetMove(VECTOR vec) { m_targetMove = vec; }
private:

	// 注視する
	void Focus(VECTOR forcusPoint);

	// カメラ回転
	void Rotate();
	
	// 座標
	VECTOR m_pos;

	// カメラの方向
	VECTOR m_dir;

	//　カメラの上方向
	VECTOR m_dirY;

	// 物理動作
	Physics m_physics;

	// 当たり判定
	Collider m_collider;

	// 注視点からカメラの距離
	float m_cameraRange;

	// カメラ初期位置
	VECTOR m_firstPos;
	// カメラの視点座標
	VECTOR m_viewPos;
	// カメラの視点先座標
	VECTOR m_targetViewPos;
	// 回転の移動量
	Quaternion m_rotate;
	// カメラ自体の行き先座標
	VECTOR m_targetCameraPos;
	// 直前フレームのターゲット位置
	VECTOR m_lastObjPos;
	// カメラとターゲットのXZ平面上の距離
	float m_distanceXZ;

	float m_debugMove;

	// true：TPSモード false：クォーターviewモード
	ViewMode m_view;

	// ターゲットロック
	bool m_isTargetRock;

	// 回転可能かどうか
	bool m_isRotate = false;

	// カメラ変更時フラグ
	bool m_isChange = false;

	// カメラの移動倍率変更時間
	int m_changeTimer = 0;

	// カメラの振動フラグ
	bool m_isVibration = false;
	// カメラの振動時間
	int m_vibrationFrame = 0;
	// カメラの振動時間の1/4
	float m_vibrationQuaterFrame = 0;

	// カメラの振動距離
	float m_vibrationRange = 0.0f;
	// カメラの振動の左右フラグ
	bool m_isVibrationLeft = false;

	VECTOR m_targetMove = VZero();
};

