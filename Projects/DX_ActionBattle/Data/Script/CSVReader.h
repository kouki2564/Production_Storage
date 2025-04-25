#pragma once
#include <string>
#include <vector>
#include "DxLib.h"

enum class Object
{
	PLAYER,
	ENEMY_STANDARD,
	ENEMY_MAGIC,
	ENEMY_TANK,
	MUTANT
};

struct ObjectData
{
	int handle;
	VECTOR pos;
	VECTOR dir;
	VECTOR scale;
};

class CSVReader
{
public:
	enum class CSV
	{
		NUM,
		CHARACTER
	};

	CSVReader();
	~CSVReader();

	ObjectData GetObjectData(Object obj);

private:
	std::vector<std::string> m_path;
};

