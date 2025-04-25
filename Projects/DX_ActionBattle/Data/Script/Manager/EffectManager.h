#pragma once
#include <string>
#include <map>
#include "DxLib.h"
#include "EffekseerForDXLib.h"


struct Effect
{
	int handle = -1;
	VECTOR pos = VZero();
	VECTOR dir = VZero();
	// 再生中のエフェクトハンドル
	int playinghandle = -1;
};

class EffectManager
{
public:
	EffectManager();
	~EffectManager();

	// インスタンス生成
	static EffectManager& Instance()
	{
		static EffectManager instance;
		return instance;
	}

	void Update();
	void Draw();

	// エフェクトの登録
	void RegisterEffect(std::string name,int handle, VECTOR pos, VECTOR dir);

	// エフェクトの削除
	void DeleteEffect(std::string name);

	// エフェクトの座標更新
	void UpdateEffectData(std::string name, VECTOR pos, VECTOR dir);

	void PlayEffect(std::string name, VECTOR pos, VECTOR dir);

	void MoveEffectPos(std::string name, VECTOR pos, VECTOR dir);

	void StopEffect(std::string name);

	bool GetIsPlaying(std::string name);

private:

	int m_backDrawHandle;

	// エフェクトの情報
	std::map<std::string, Effect> m_effects;
};

