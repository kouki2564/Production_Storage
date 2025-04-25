#include "EffectManager.h"

EffectManager::EffectManager()
{
	m_backDrawHandle = LoadGraph("Data/Image/null.png");
}

EffectManager::~EffectManager()
{
	m_effects.clear();
	DeleteGraph(m_backDrawHandle);
}

void EffectManager::Update()
{
	// えふぇくしあの同期
	Effekseer_Sync3DSetting();
	// えふぇくしあの更新
	UpdateEffekseer3D();
}

void EffectManager::Draw()
{
	DrawGraph(0, 0, m_backDrawHandle, true);
	auto isDraw = DrawEffekseer3D();
	if (isDraw == -1)
	{
		printf("EffectDraw_Error\n");
	}

	for (auto& i : m_effects)
	{
		auto is = IsEffekseer3DEffectPlaying(i.second.playinghandle);
	}
}

void EffectManager::RegisterEffect(std::string name, int handle, VECTOR pos, VECTOR dir)
{
#ifdef _DEBUG
	if (handle == -1)
	{
		printf("EffectHandle_Error\n");
	}
#endif // _DEBUG

	// ダブりチェック
	for (auto& i : m_effects)
	{
		if (i.first == name)
		{
			return;
		}
	}
	// エフェクトの登録
	Effect e;
	e.handle = handle;
	e.pos = pos;
	e.dir = dir;
	m_effects.insert(std::make_pair(name, e));
}

void EffectManager::DeleteEffect(std::string name)
{
	// エフェクトの停止とハンドルの解放
	if (m_effects[name].playinghandle != -1)
	{
		StopEffekseer3DEffect(m_effects[name].playinghandle);
	}

	// エフェクトの削除
	m_effects.erase(name);
}

void EffectManager::UpdateEffectData(std::string name, VECTOR pos, VECTOR dir)
{
	auto isUpdate = false;
	for (auto& i : m_effects)
	{
		if (i.first == name)
		{
			isUpdate = true;
			break;
		}
	}
	if (!isUpdate)
	{
		return;
	}

	// エフェクトの座標更新
	m_effects[name].pos = pos;
	m_effects[name].dir = dir;
	SetPosPlayingEffekseer3DEffect(m_effects[name].playinghandle,
		m_effects[name].pos.x, m_effects[name].pos.y, m_effects[name].pos.z);
	SetRotationPlayingEffekseer3DEffect(m_effects[name].playinghandle,
		0.0f, atan2(m_effects[name].dir.x, m_effects[name].dir.z) - (DX_PI_F / 2), 0.0f);
	// UpdateEffekseer3D();
}

void EffectManager::PlayEffect(std::string name, VECTOR pos, VECTOR dir)
{
	auto isPlay = IsEffekseer3DEffectPlaying(m_effects[name].playinghandle);
	if (isPlay == 0)
	{
		// 古いエフェクトハンドルを解放
		if (m_effects[name].playinghandle != -1)
		{
			StopEffekseer3DEffect(m_effects[name].playinghandle);
		}
		m_effects[name].playinghandle = PlayEffekseer3DEffect(m_effects[name].handle);
		MoveEffectPos(name, pos, dir);
		auto is = IsEffekseer3DEffectPlaying(m_effects[name].playinghandle);
		if (is == -1)
		{
			printf("EffectPlay_Error\n");
		}
	}
	else if (isPlay == -1)
	{
		// 古いエフェクトハンドルを解放
		if (m_effects[name].playinghandle != -1)
		{
			StopEffekseer3DEffect(m_effects[name].playinghandle);
		}

		m_effects[name].playinghandle = PlayEffekseer3DEffect(m_effects[name].handle);

		MoveEffectPos(name, pos, dir);
		auto is = IsEffekseer3DEffectPlaying(m_effects[name].playinghandle);
		if (is == -1)
		{
			printf("EffectPlay_Error\n");
		}
	}
}

void EffectManager::MoveEffectPos(std::string name, VECTOR pos, VECTOR dir)
{
	m_effects[name].pos = pos;
	m_effects[name].dir = dir;
	SetPosPlayingEffekseer3DEffect(m_effects[name].playinghandle,
		m_effects[name].pos.x, m_effects[name].pos.y, m_effects[name].pos.z);
	SetRotationPlayingEffekseer3DEffect(m_effects[name].playinghandle,
		0.0f, atan2(m_effects[name].dir.x, m_effects[name].dir.z) - (DX_PI_F / 2), 0.0f);
	// UpdateEffekseer3D();
}

void EffectManager::StopEffect(std::string name)
{
	if (IsEffekseer3DEffectPlaying(m_effects[name].playinghandle) == 0)
	{
		StopEffekseer3DEffect(m_effects[name].playinghandle);
		m_effects[name].playinghandle = -1; // ハンドルをリセット
	}
}

bool EffectManager::GetIsPlaying(std::string name)
{
	auto is = IsEffekseer3DEffectPlaying(m_effects[name].playinghandle);
	if (is == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
