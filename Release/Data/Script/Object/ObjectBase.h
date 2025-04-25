#pragma once
#include "GameData.h"
#include "DxLib.h"
#include <memory>
#include "Physics.h"
#include "DrawingManager.h"
#include "PanelManager.h"
#include "Parameter.h"

class ObjectBase :
	public std::enable_shared_from_this<ObjectBase>
{
public:
	ObjectBase()
	{
		m_pos = VGet(0.0f, 0.0f, 0.0f);
		m_dir = VGet(0.0f, 0.0f, 1.0f);
	}
	virtual ~ObjectBase()
	{ 
		MV1DeleteModel(m_handle);
		m_attackCol.clear();
	}


	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Init(std::string name, VECTOR pos, VECTOR dir) = 0;

	/// <summary>
	/// キャラ周り全体の更新
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 最終的な更新内容の反映
	/// </summary>
	virtual void ApplyUpdating() = 0;

	virtual void InitTitleAnimation() = 0;

	virtual void SetWeapon(int num) = 0;

	void SetCameraDir(VECTOR cameraDir) { m_cameraDir = cameraDir; }
	
	virtual void SetChaseTarget(VECTOR targetPos) = 0;

	// ダメージ処理（ダメージ無しでfalseを返す）
	virtual bool SetDamagePoint(float damagePoint, bool isStan, bool isPowKnock) = 0;
	
	virtual void SetPosAndDir(VECTOR pos, VECTOR dir) = 0;

	virtual void SetUpgrade(int paramNum) = 0;

	/// <summary>
	/// 床からの押し出しベクトルの設定
	/// </summary>
	/// <param name="vec"> 押し出しベクトル </param>
	void SetFloorPushVec(VECTOR vec) { 

		m_physics.SetFloorPushVec(vec); 
	}

	/// <summary>
	/// 押し出しベクトルの設定
	/// </summary>
	/// <param name="vec"> 押し出しベクトル </param>
	void SetPushVec(VECTOR vec) { m_physics.SetPushVec(vec); }

	/// <summary>
	/// オブジェクトの基盤設定
	/// </summary>
	/// <param name="name">mapキー用</param>
	/// <param name="handle">
	/// 
	/// 
	/// 
	/// ハンドル</param>
	/// <param name="pos">座標</param>
	/// <param name="dir">正面の方向ベクトル</param>
	/// <param name="scale">モデルサイズ倍率</param>
	void SetUpObject(std::string name, int handle, VECTOR pos = VGet(0,0,0), VECTOR dir = VZero(), VECTOR scale = VGet(1, 1, 1));

	/// <summary>
	/// 位置補正
	/// </summary>
	void FixPos();

	/// <summary>
	/// ノックバックベクトルの設定（knockBackだけほかで使うため、セッター配置）
	/// </summary>
	/// <param name="vec">ノックバックベクトル</param>
	void SetKnockVec(VECTOR vec) { m_physics.SetKnockVec(vec); }

	/// <summary>
	/// 地上判定の付与
	/// </summary>
	/// <param name="isGround"> フラグ </param>
	void SetIsGround(bool isGround) { m_physics.SetIsGround(isGround); }

	/// <summary>
	/// 次フレームの地上判定の付与
	/// </summary>
	/// <param name="isToGround">フラグ</param>
	void SetIsToGround(bool isToGround) { m_physics.SetIsToGround(isToGround); }

	/// <summary>
	/// 攻撃の当たり判定チェック
	/// </summary>
	/// <param name="colOther">判定チェックする対象のコライダー</param>
	/// <returns>攻撃が当たっているか</returns>
	virtual bool GetIsHitAttack(Collider& colOther) = 0;

	/// <summary>
	/// コライダー情報の受け渡し
	/// </summary>
	/// <returns>コライダー</returns>
	Collider GetCollider() { return m_collider; }

	// 座標取得
	const VECTOR GetPos() const { return m_pos; }

	// 移動量取得
	const VECTOR GetUpdateVec() { return m_physics.GetUpdateVec(); }

	// 移動予定量取得
	const VECTOR GetSemiUpdateVec() { return m_physics.GetSemiUpdateVec(); }

	// 方向取得
	const VECTOR GetDir() const { return m_dir; }

	// ハンドル取得
	const int GetHandle() const { return m_handle; }

	// ダメージ付与（ダメージ値計算は攻撃の当たり判定直後）
	float GetDamagePoint() { return m_damagePoint; }

	// 倒したかどうかのフラグ
	bool GetIsDead() { return (m_parameter.GetPalameter(0) <= 0); }

	// 削除処理をするためのフラグ取得
	bool GetIsDelete() { return m_isDelete; }

	Parameter GetParameter() { return m_parameter; }

	Level GetLevel() { return m_level; }

	void SetLevel(Level level);

	// 敵固定用
	void SetIsLock() { m_isLock = true; }

	void SetIsUse(bool isUse) { m_isUse = isUse; }
	bool GetIsUse() { return m_isUse; }

	// とりあえず移動量とか受け取りたい
	VECTOR GetResVec() { return m_updateVec; }

	// IDのセット(主に敵用)
	void SetID(int id) { m_id = id; };

	// チュートリアル用処理
	void SetTutorialMode(bool isTutorial) { m_isTutorial = isTutorial; }

	// 与スタンのフラグ受け取り
	bool GetIsStanFrag() { return m_isGiveStanFrag; }

	// 強攻撃のノックバックフラグ受け取り
	bool GetIsPowerKnock() { return m_isPowerKnock; }

protected:

	// レベル
	Level m_level;

	// パラメータ
	Parameter m_parameter;

	// 識別名
	std::string m_name;

	// 座標
	VECTOR m_pos = VZero();

	// モデルの方向
	VECTOR m_dir = VZero();

	// カメラ方向
	VECTOR m_cameraDir = VZero();

	// 物理動作
	Physics m_physics;

	// 当たり判定
	Collider m_collider;

	// 攻撃判定
	std::vector<Collider> m_attackCol;

	// モデルハンドル
	int m_handle = -1;

	int m_id = -1;

	// 敵に与えるダメージ値
	float m_damagePoint = 0;

	// スタンを与えるかどうか
	bool m_isGiveStanFrag = false;

	// スタンを受けたかどうか
	bool m_isTakeStanFrag = false;

	bool m_isPowerKnock = false;

	// 削除フラグ
	bool m_isDelete = false;

	// 敵固定用
	bool m_isLock = false;

	bool m_isUse = true;

	VECTOR m_updateVec = VZero();

	bool m_isTutorial = false;
};

