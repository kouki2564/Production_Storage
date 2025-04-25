#pragma once
#include "DxLib.h"
#include <vector>
#include <memory>
#include "Collider.h"
#include "Controller.h"

class Physics
{
public:

	/* ベクトル関連 */

	/// <summary>
	/// 座標の更新最終的な移動量の獲得
	/// </summary>
	/// <returns>移動ベクトル</returns>
	VECTOR GetUpdateVec();

	VECTOR GetSemiUpdateVec();
	/// <summary>
	/// 移動：コントローラ無し（主に敵の移動）
	/// </summary>
	/// <param name="targetPos">移動方向</param>
	/// <param name="movePow">移動量</param>
	void Move(VECTOR targetDir, float movePow);

	/// <summary>
	/// 移動：コントローラ使用
	/// </summary>
	/// <param name="movePow">：移動量</param>
	void MoveControll(VECTOR flontDir, float movePow);

	/// <summary>
	/// コントローラによる向き変更
	/// </summary>
	/// <param name="flontDir"></param>
	void DirControll(VECTOR flontDir);

	/// <summary>
	/// 床からのプッシュ量のセット
	/// </summary>
	/// <param name="floorPushVec"> 押し出しベクトル </param>
	void SetFloorPushVec(VECTOR floorPushVec) { m_floorPushVec = floorPushVec; }

	/// <summary>
	/// プッシュ量のセット
	/// </summary>
	/// <param name="pushVec"> 押し出しベクトル </param>
	void SetPushVec(VECTOR pushVec);

	/// <summary>
	/// 地上判定の付与
	/// </summary>
	/// <param name="isGround"> フラグ </param>
	void SetIsGround(bool isGround);

	/// <summary>
	/// 地上判定の付与
	/// </summary>
	/// <param name="isGround"> フラグ </param>
	void SetIsToGround(bool isToGround);

	/// <summary>
	/// ノックベクトルのセット
	/// </summary>
	/// <param name="knockVec">：ノックベクトル</param>
	void SetKnockVec(VECTOR knockVec) { m_knockVec = knockVec; }

	/// <summary>
	/// ジャンプベクトルのセット
	/// </summary>
	void SetJumpVec(float height, int time, int maxTime);

	void SetJumpFrag();

	/// <summary>
	/// 最終的な移動量の獲得
	/// </summary>
	/// <returns>移動ベクトル</returns>
	VECTOR GetVec() const { return m_responsVec; }

	/// <summary>
	/// オブジェクトの方向を獲得
	/// </summary>
	/// <returns></returns>
	VECTOR GetDir() const { return m_dir; }

	/// <summary>
	/// オブジェクトから方向を付与
	/// </summary>
	/// <param name="dir"></param>
	void SetDir(VECTOR dir) { m_dir = dir; }

	/// <summary>
	/// 重力のセット
	/// </summary>
	/// <param name="gravityForce">：重力量</param>
	/// <param name="dir">：重力方向</param>
	void SetGravity(float gravityForce, VECTOR dir) { m_gravityForce = gravityForce; m_gravityDir = dir; }

	/// <summary>
	/// 浮き上がり動作(主に空中攻撃時)
	/// </summary>
	/// <param name="isLast">：打ち下ろしするか(主にプレイヤー最終打)</param>
	void Hover(bool isLast);

	/// <summary>
	/// 回避動作
	/// </summary>
	void Dodge(float dodgePow);

	/// <summary>
	/// 攻撃動作
	/// </summary>
	/// <param name="movePow"></param>
	void Attack(float movePow);

	/// <summary>
	/// ジャンプ可能かどうか
	/// </summary>
	/// <returns></returns>
	bool GetIsJump() { return (m_isGround || m_isAirJump); }



	/// <summary>
	/// 地上にいるかどうか
	/// </summary>
	/// <returns></returns>
	bool GetIsGround() { return m_isGround; }
	bool GetIsToGround() { return m_isToGround; }

	/// <summary>
	/// 当たり判定の獲得
	/// </summary>
	/// <returns>当たり判定</returns>
	std::shared_ptr<Collider> GetCollision()const { return m_collider; }


	/*各種ゲッター関数（使うか微妙だからとりあえず適当作成）*/

	VECTOR GetMove() 
	{
		// 微小値のゼロ補正
		if (VSize(m_moveVec) < 0.1f)
		{
			return VZero();
		}
		else
		{
			return m_moveVec;
		}
	}
	VECTOR GetPush() { return m_pushVec; }
	VECTOR GetKnock() { return m_knockVec; }
	VECTOR GetJump() { return m_jumpVec; }
	VECTOR GetFloorPushVec() { return m_floorPushVec; }
	float GetGravity() { return m_gravityForce; }


private:

	void ChackIsGround();

	void Jump();

	// 摩擦係数（とまでは言わなくても移動量軽減係数）
	float m_reductRate = 0.5f;
	
	// 前を向いてる方向
	VECTOR m_dir = VGet(0,0,1);

	// 移動量
	VECTOR m_moveVec = VGet(0,0,0);
	// 床からの押し出しベクトル
	VECTOR m_floorPushVec = VZero();
	// 押し出しベクトル
	VECTOR m_pushVec = VZero();
	// 優先的な押し出しベクトル（壁等めり込み防止の為）
	VECTOR m_mustPushVec = VZero();
	// ノックベクトル
	VECTOR m_knockVec = VZero();


	// ジャンプベクトル
	VECTOR m_jumpVec = VZero();
	// ジャンプのみの仮想Y座標
	float m_jumpPosY = 0;
	// ジャンプ時間
	int m_jumpTimer = 0;
	// 地面に着地しているか
	bool m_isGround = true;
	// 次フレームで地面に着地しているか
	bool m_isToGround = true;
	// 空中でジャンプを使用したか
	bool m_isAirJump = true;




	// 最終的な移動ベクトル
	VECTOR m_responsVec = VZero();

	// 重力量
	float m_gravityForce = 0.0f;

	// ジャンプ計算フラグ
	bool m_isMathJump = false;
	// ジャンプ量
	float m_jumpForce = 0;
	// 重力ベクトル
	VECTOR m_gravityDir = VGet(0, -1, 0);
	// 当たり判定
	std::shared_ptr<Collider> m_collider;
};

