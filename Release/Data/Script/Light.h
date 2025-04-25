#pragma once
#include "DxLib.h"

struct LightSet
{
	int handle = 0;

	VECTOR pos = VGet(0.0f, 0.0f, 0.0f);
	VECTOR targetPos = VGet(0.0f, 0.0f, 0.0f);
	VECTOR dir = VGet(0.0f, 0.0f, 0.0f);
	
	float outAngle = 0.0f;
	float inAngle = 0.0f;
	float range = 0.0f;

	COLOR_F difColor = { 0.0f,0.0f,0.0f,1.0f };
	COLOR_F spcColor = { 0.0f,0.0f,0.0f,1.0f };
	COLOR_F ambColor = { 0.0f,0.0f,0.0f,1.0f };
};

class Light
{
public:
	Light();
	virtual ~Light();

	void Init(VECTOR cameraPos, VECTOR frontPos);
	void Update(VECTOR cameraPos, VECTOR frontPos);

private:

	// ポイントライトハンドル（2個まで設定可能な為、２で固定）
	LightSet m_lights[2];

	int m_light = 0;
};