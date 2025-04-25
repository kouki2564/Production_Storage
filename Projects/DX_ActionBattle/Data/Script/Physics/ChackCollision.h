#pragma once
#include "Collider.h"
#include <memory>

namespace ColInfo
{
	//最大当たり判定ポリゴン数
	constexpr int kMaxColHitPolyNum = 2000;
	//壁押し出し処理の最大試行回数
	constexpr int kMaxColHitTryNum = 16;
	//壁押し出し時にスライドさせる距離
	constexpr float kColHitSlideLength = 1.0f;
}

class ChackCollision
{
public:
	ChackCollision();
	~ChackCollision();


	//インスタンスを返す
	static ChackCollision& Instance() {
		static ChackCollision instance;
		return instance;
	}

	bool GetIsCollision(Collider& colA, Collider& colB);

	/// <summary>
	/// 次フレームで床と接触するかの判定
	/// </summary>
	/// <param name="colA">判定したいオブジェクト</param>
	/// <param name="colB">判定先のマップオブジェクト</param>
	/// <returns>次フレームで接触</returns>
	bool GetIsToGround(Collider& colA, Collider& colB, VECTOR& colAPreMoveVec);

	VECTOR GetPushVec(Collider& colA, Collider& colB, VECTOR& colAPreMoveVec);

	bool GetIsGround(Collider& colA, Collider& colB, VECTOR& colAPreMoveVec);

private:
	// 球と球

	/// <summary>
	/// 球と球
	/// </summary>
	/// <param name="colA">球</param>
	/// <param name="colB">球</param>
	/// <returns>colA基準の移動ベクトル</returns>
	VECTOR GetIsColSphereAndSphere(Collider& colA, Collider& colB);

	/// <summary>
	/// 球とカプセル
	/// </summary>
	/// <param name="colA">球</param>
	/// <param name="colB">カプセル</param>
	/// <returns>colA基準の移動ベクトル</returns>
	VECTOR GetIsColSphereAndCapsule(Collider& colA, Collider& colB);

	/// <summary>
	/// カプセルとカプセル
	/// </summary>
	/// <param name="colA">カプセル</param>
	/// <param name="colB">カプセル</param>
	/// <returns>colA基準の移動ベクトル</returns>
	VECTOR GetIsColCapsuleAndCapsule(Collider& colA, Collider& colB);


	/// <summary>
	/// オブジェクトとマップ
	/// </summary>
	/// <param name="colOther">マップ以外のオブジェクト</param>
	/// <param name="colMap">マップのオブジェクト</param>
	/// <param name="isAddGravity">重力落下を加味するか（次フレームで床と当たるかの判定のため）</param>
	/// <returns>補正ベクトル</returns>
	VECTOR GetIsColOtherAndMap(Collider& colOther, Collider& colMap, VECTOR& colAPreMoveVec, bool isAddGravity);


	VECTOR MoveCollFieldUpdate(Collider& colOther, Collider& colMap, VECTOR& preMoveVec, bool isAddGravity);

	void CheckWallAndFloor(Collider& colOther);
	VECTOR FixPositionWithWall(Collider& colOther, VECTOR& nextPos);
	VECTOR FixPositionWithWallInternal(Collider& colOther, VECTOR& nextPos);
	VECTOR FixNowPositionWithFloor(Collider& colOther, VECTOR& nextPos);


	bool m_isMoveFlag = false;		//移動したかどうか
	bool m_isHitFlag = false;		//ポリゴンに当たったかどうか
	int m_wallNum = 0;				//壁ポリゴンと判断されたポリゴン数
	int m_floorNum = 0;				//床ポリゴンと判断されたポリゴン数

	MV1_COLL_RESULT_POLY_DIM m_hitDim{};								//当たり判定結果構造体
	MV1_COLL_RESULT_POLY* m_pWallPoly[ColInfo::kMaxColHitPolyNum]{};	// 壁ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておくためのポインタ配列
	MV1_COLL_RESULT_POLY* m_pFloorPoly[ColInfo::kMaxColHitPolyNum]{};	// 床ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておくためのポインタ配列
	MV1_COLL_RESULT_POLY* m_pPoly = nullptr;							// ポリゴンの構造体にアクセスするために使用するポインタ
	HITRESULT_LINE m_lineRes{};											// 線分とポリゴンとの当たり判定の結果を代入する構造体
};

