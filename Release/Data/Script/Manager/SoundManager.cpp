#include "SoundManager.h"
#include "DxLib.h"

SoundManager::SoundManager()
{
}

SoundManager::~SoundManager()
{
	m_sounds.clear();
}

void SoundManager::RegisterSound(std::string name, int handle, int vol)
{
	Sound sound;
	sound.handle = handle;
	sound.vol = vol;
	m_sounds.insert(std::make_pair(name, sound));
}

void SoundManager::OnePlay(std::string name)
{
	if (m_sounds[name].vol != 0)
		ChangeVolumeSoundMem(m_sounds[name].vol, m_sounds[name].handle);
	PlaySoundMem(m_sounds[name].handle, DX_PLAYTYPE_BACK, true);

	// 再生確認
	auto i = CheckSoundMem(m_sounds[name].handle);
	if (i == 0)
	{
		printf("OneSound_Error_0\n");
	}
	else if (i == -1)
	{
		printf("OneSound_Error_-1\n");
	}
	else
	{
		printf("OneSound_OK\n");
	}
}

void SoundManager::loopPlay(std::string name)
{
	if (m_sounds[name].vol != 0)
		ChangeVolumeSoundMem(m_sounds[name].vol, m_sounds[name].handle);
	PlaySoundMem(m_sounds[name].handle, DX_PLAYTYPE_LOOP, true);

	// 再生確認
	auto i = CheckSoundMem(m_sounds[name].handle);
	if (i == 0)
	{
		printf("LoopSound_Error_0\n");
	}
	else if (i == -1)
	{
		printf("LoopSound_Error_-1\n");
	}
	else
	{
		printf("LoopSound_OK\n");
	}
}

void SoundManager::Stop(std::string name)
{
	StopSoundMem(m_sounds[name].handle);
}

void SoundManager::StopAll()
{
	for (auto& t : m_sounds)
	{
		StopSoundMem(t.second.handle);
	}
}

void SoundManager::ChangeVolume(std::string name, int vol)
{
	m_sounds[name].vol = vol;
	ChangeVolumeSoundMem(m_sounds[name].vol, m_sounds[name].handle);
}

bool SoundManager::GetIsPlaying(std::string name)
{
	return CheckSoundMem(m_sounds[name].handle);
}
