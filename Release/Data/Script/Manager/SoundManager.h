#pragma once
#include <string>
#include <map>

struct Sound
{
	// サウンドハンドル
	int handle;
	// 音量
	int vol;
};

class SoundManager
{
public:
	SoundManager();
	~SoundManager();

	//インスタンスを返す
	static SoundManager& Instance() {
		static SoundManager instance;
		return instance;
	}

	void RegisterSound(std::string name, int handle, int vol);

	void OnePlay(std::string name);

	void loopPlay(std::string name);

	void Stop(std::string name);

	void StopAll();

	void ChangeVolume(std::string name, int vol);

	bool GetIsPlaying(std::string name);

private:

	std::map<std::string, Sound> m_sounds;

};

