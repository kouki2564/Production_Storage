#pragma once
#include "DxLib.h"
#include <map>
#include <string>
#include "ToonShader.h"

/// <summary>
/// アニメーションデータ（モデルデータに付随させる）
/// </summary>
struct AnimationData
{
	// アニメーション現在フレーム
	float animaFrame = 0;
	// アニメーション遷移rate
	float animaRate = 0;
	// アニメーションインデックス
	// 再生中アニメーション
	int nowAttach = -1;
	// 移行先アニメーション
	int nextAttach = -1;
	// 現在アニメーション終了時に移行するアニメーション
	int transAttach = -1;
	// アニメーション速度
	int animaPlayFrame = 0;
	// 移行先のアニメーション速度
	int animaNextPlayFrame = 0;
	// 現在アニメーション終了時に移行するアニメーション速度
	int animaTransPlayFrame = 0;

};

/// <summary>
/// モデルの形状
/// </summary>
enum class ModelForm
{
	SPHERE,
	CUBE,
	CAPSULE,
	MODEL
};

/// <summary>
/// モデルデータ
/// </summary>
struct ModelData
{

	// モデルハンドル
	int handle = -1;
	// モデルの形状
	ModelForm form = ModelForm::MODEL;
	// ３D空間座標
	VECTOR pos = VZero();
	// モデル回転値
	VECTOR dir = VZero();
	// モデル拡大値
	VECTOR scale = VGet(1,1,1);
	// 描画を行うかどうか
	bool isDraw = true;
	// 影を落とすかどうか
	bool isShadow = true;
	// モデルの透明度
	int alpha = 255;

	// アニメーションデータ
	AnimationData animation;


	/* 武器モデル用変数 */
	std::string ornerName = "NULL";
	int setFrameNum = 0;
	VECTOR rot = VZero();


	/* ハンドル外のモデル用変数 */
	// モデルの色
	int difColor = 0xffffff;
	int spcColor = 0xffffff;
	float radius = 0;
	bool isFill = true;
	VECTOR pos2 = VZero();	// カプセル用

};


/// <summary>
/// 描画
/// </summary>
class DrawingManager
{
public:
	DrawingManager()
	{
		m_shadowHandle = MakeShadowMap(4096, 4096);
		SetShadowMapLightDirection(m_shadowHandle, VGet(0.0f, -1.0f, 0.0f));
		// シャドウマップに描画する範囲を設定
		SetShadowMapDrawArea(m_shadowHandle, VGet(-200.0f, -10.0f, -200.0f), VGet(200.0f, 200.0f, 200.0f));
	}

	~DrawingManager()
	{
		for (auto& t : m_models)
		{
			MV1DeleteModel(t.second.handle);
		}
		m_models.clear();
		DeleteShadowMap(m_shadowHandle);
	}

	//インスタンスを返す
	static DrawingManager& Instance() {
		static DrawingManager instance;
		return instance;
	}

	/// <summary>
	/// モデル情報の登録
	/// </summary>
	void RegisterModel(std::string name, int handle, VECTOR pos, VECTOR dir, VECTOR scale);
	
	/// <summary>
	/// モデル情報の登録
	/// </summary>
	void RegisterMapModel(std::string name, int handle, VECTOR pos, VECTOR dir, VECTOR scale);

	/// <summary>
	/// ハンドルを持たないモデルの登録（主に魔法とか諸々）
	/// </summary>
	void RegisterOtherModel(std::string name, VECTOR pos, ModelForm form, int difColor, int spcColor, float radius, bool isFill, VECTOR pos2 = VZero());

	/// <summary>
	/// 武器モデル等、モデルに付随させるモデルの登録
	/// </summary>
	void RegisterWeaponModel(std::string weaponName, int handle, VECTOR scale, VECTOR rot, std::string ownerModelName, std::string frameName);

	void DeleteModel(std::string name);

	void DeleteAllModel();

	/// <summary>
	/// モデル情報の更新
	/// </summary>
	/// <param name="name">：mapデータ呼び出し用の名前</param>
	/// <param name="modelData">：モデルの情報</param>
	void UpdateModelData(std::string name, VECTOR pos, VECTOR dir = VZero(), VECTOR scale = VGet(-1, -1, -1));

	void UpdateWeaponModelData(std::string name);

	void Draw();

	void SetIsDrawModel(std::string name, bool isDraw) { m_models[name].isDraw = isDraw; }

	void SetIsShadowModel(std::string name, bool isShadow) { m_models[name].isShadow = isShadow; }

	int GetModelHandle(std::string name) { return m_models[name].handle; }

	void ChangePopModelFrame(std::string name, int frameNum, bool isPop);

	/// <summary>
	/// モデルの透明度変更
	/// </summary>
	/// <param name="name">モデル名</param>
	/// <param name="frameNum">モデルの透明度変更するフレーム（-1で全体の透明度変更）</param>
	/// <param name="rate">透明度の割合0.0～1.0</param>
	void SetFrameOpacityRate(std::string name, int frameNum, float rate);

	void SetStageDoor(bool isRight, bool isFront, bool isLeft, bool isBack);

	/// <summary>
	/// アニメーション呼び出し（呼び出し時点で切り替えるもの）
	/// </summary>
	/// <param name="modelName">：モデルの名前</param>
	/// <param name="animationName">：アニメーションの名前</param>
	/// <param name="animationFrame">：アニメーションの再生時間</param>
	void CallAnimation(std::string modelName, std::string animationName, int animationFrame);

	/// <summary>
	/// 移行アニメーション呼び出し予約(アニメーション1周後、自動でアニメーション切り替えするためのもの)
	/// </summary>
	/// <param name="modelName">：モデルの名前</param>
	/// <param name="animationName">：アニメーションの名前</param>
	/// <param name="animationFrame">：アニメーションの再生時間</param>
	void CallTransAnimation(std::string modelName, std::string animationName, int animationFrame);

	/// <summary>
	/// アニメーションの再生時間変更
	/// </summary>
	/// <param name="modelName">：モデルの名前</param>
	/// <param name="animationFrame">：アニメーションの再生時間</param>
	void ChangeSpeedAnimation(std::string modelName, int animationFrame);

	/// <summary>
	/// 現行アニメーションの名前獲得
	/// </summary>
	/// <param name="modelName">：モデルの名前</param>
	/// <returns>：アニメーションの名前</returns>
	std::string GetPlayingAnimationName(std::string modelName);

	/// <summary>
	/// 主にアニメーションによるモデルの座標のずれを獲得
	/// </summary>
	/// <param name="modelName">：モデルの名前</param>
	/// <param name="pos">：実際の座標</param>
	/// <returns> [実座標→モデル座標]のベクトル </returns>
	VECTOR GetModelPosLag(std::string& modelName, VECTOR pos) { return VSub(m_models[modelName].pos, pos); }
	
	int getnextattach(std::string name) { return m_models[name].animation.nextAttach; }

	/// <summary>
	/// アニメーション更新
	/// </summary>
	void AnimationUpdate(std::string& modelName);
private:



	void RateAnimation(std::string& modelName);

	// アニメーション関連変数

	//// アニメーション元のモデルハンドル
	//int m_modelHandle;
	//// アニメーション現在フレーム
	//float m_animaFrame;
	//// アニメーション遷移rate
	//float m_animaRate;
	//// アニメーションインデックス
	//// 再生中アニメーション
	//int m_nowAttach;
	//// 移行先アニメーション
	//int m_nextAttach;
	//// アニメーション速度
	//float m_animaSpeed;
	
	ToonShader m_tShader;

	std::map<std::string, ModelData> m_models;

	int m_shadowHandle = -1;
};