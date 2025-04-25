#pragma once
#include "DxLib.h"

class Animation
{
public:
	Animation();
	~Animation();

	/// <summary>
	/// アニメーションのセット
	/// </summary>
	/// <param name="modelHandle">動かすモデルのハンドル</param>
	/// <param name="animaNum">アニメーションの番号</param>
	/// <param name="Speed">アニメーション再生速度(初期値：30)</param>
	/// <param name="frame">アニメーションの開始フレーム(初期値：0)</param>
	void InitAnimation(int modelHandle, int animaNum, float speed, float frame);

	/// <summary>
	/// アニメーションのフレームを調整
	/// </summary>
	/// <param name="frame">フレーム値</param>
	void SetFrame(float frame) { m_animaFrame = frame; }

	/// <summary>
	/// アニメーションの変更
	/// </summary>
	/// <param name="animaNum">アニメーション番号</param>
	/// <param name="speed">アニメーション再生速度</param>
	void SetAnimation(int animaNum, float speed);

	// アニメーション再生
	void TransAnimation();

	// アニメーションがどれくらい進んだのかの割合を獲得
	float GetAnimationProgress() const { return (m_animaFrame / MV1GetAttachAnimTotalTime(m_modelHandle, m_nowAttach)); }

	// 現在再生中のアニメーション番号を獲得
	int GetAnimationNum() const { return MV1GetAttachAnim(m_modelHandle, m_nowAttach); }

	/// <summary>
	/// アニメーション更新
	/// </summary>
	void PlayAnimation();

private:
	void AnimaRate();


	// アニメーション関連変数

	// アニメーション元のモデルハンドル
	int m_modelHandle;
	// アニメーション現在フレーム
	float m_animaFrame;
	// アニメーション遷移rate
	float m_animaRate;
	// アニメーションインデックス
	// 再生中アニメーション
	int m_nowAttach;
	// 移行先アニメーション
	int m_nextAttach;
	// アニメーション速度
	float m_animaSpeed;
};

