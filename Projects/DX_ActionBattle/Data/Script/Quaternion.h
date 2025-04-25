#pragma once
#include "DxLib.h"
#include <cmath>

class Quaternion
{
public:
	Quaternion()
	{
		Qu.w = 1;
		Qu.x = 0;
		Qu.y = 0;
		Qu.z = 0;
		vec = VZero();
	};
	virtual ~Quaternion() {}

private:
	struct Q
	{
		float w;
		float x;
		float y;
		float z;
		

		Q operator * (const Q& _q) const
		{
			Q tempQ;

			/*クオータニオンの掛け算*/
			//公式通り
			tempQ.w = w * _q.w - x * _q.x - y * _q.y - z * _q.z;//実部
			tempQ.x = w * _q.x + x * _q.w + y * _q.z - z * _q.y;//虚部x
			tempQ.y = w * _q.y + y * _q.w + z * _q.x - x * _q.z;//虚部y
			tempQ.z = w * _q.z + z * _q.w + x * _q.y - y * _q.x;//虚部z

			return tempQ;
		}
	};

	Q Qu;
	VECTOR vec;
	float angle;	// angleに関してはまだ改善の余地あり（現時点で1方向にしか曲がれない）

public:
	/// <summary>
	/// 移動量の設定
	/// </summary>
	/// <param name="_angle"> 1フレーム当たりの角度 ( ラジアン値 ) </param>
	/// <param name="_axis"> 回転の軸ベクトル </param>
	/// <param name="_moveVec"> 平行移動ベクトル </param>
	void SetMove(float& _angle, VECTOR& _axis, VECTOR& _moveVec)
	{
		Qu.w = cos(_angle / 2.0f);//実部
		Qu.x = _axis.x * sin(_angle / 2.0f);
		Qu.y = _axis.y * sin(_angle / 2.0f);
		Qu.z = _axis.z * sin(_angle / 2.0f);
		vec = _moveVec;
	}

	/// <summary>
	/// クォータニオンによる実移動
	/// </summary>
	/// <param name="_rotPoint"> 回転する中心の座標 </param>
	/// <param name="_pos"> 回転するものの座標 </param>
	/// <returns> 回転後の座標 </returns>
	VECTOR Move(VECTOR& _rotPoint, VECTOR& _pos)
	{
		Q qPos, qInv;
		VECTOR vPos;

		//3次元座標をクオータニオンに変換
		qPos.w = 1.0f;
		qPos.x = _pos.x - _rotPoint.x;
		qPos.y = _pos.y - _rotPoint.y;
		qPos.z = _pos.z - _rotPoint.z;

		//回転クォータニオンのインバースの作成
		//逆クォータニオンを出すのは大変なので、
		//3次元だと同じ値になる共役クオータニオンで作成(虚部だけマイナス反転)
		qInv.w = Qu.w;
		qInv.x = -Qu.x;
		qInv.y = -Qu.y;
		qInv.z = -Qu.z;

		//回転後のクオータニオンの作成
		qPos = Qu * qPos * qInv;

		//３次元座標に戻す
		vPos.x = qPos.x + _rotPoint.x;
		vPos.y = qPos.y + _rotPoint.y;
		vPos.z = qPos.z + _rotPoint.z;

		// 回転後に移動
		vPos.x += vec.x;
		vPos.y += vec.y;
		vPos.z += vec.z;

		
		return vPos;
	}
};

