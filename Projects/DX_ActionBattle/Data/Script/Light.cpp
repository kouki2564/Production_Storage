#include "Light.h"
#include "Controller.h"

namespace
{
	float kLightRange = 200.0f;
}

Light::Light()
{
	m_lights[0].handle = CreateDirLightHandle(m_lights[0].dir);
	m_lights[1].handle = CreateDirLightHandle(m_lights[1].dir);

	SetLightEnable(true);
	ChangeLightTypeDir(VGet(0, -1, 0));
}

Light::~Light()
{
	for (auto& light : m_lights)
	{
		DeleteLightHandle(light.handle);
	}
}

void Light::Init(VECTOR cameraPos, VECTOR frontPos)
{
	/// 標準ライト
	// ディフューズ
	COLOR_F difColor = { 0.2f, 0.2f, 0.2f, 1.0f };
	SetLightDifColor(difColor);
	// アンビエント
	COLOR_F ambColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	SetLightAmbColor(ambColor);


	/// カメラの視点からカメラ位置までの方向のライト
	// ディフューズ
	m_lights[0].difColor = { 1.0f,1.0f,1.0f,1.0f };
	SetLightDifColorHandle(m_lights[0].handle, m_lights[0].difColor);
	// アンビエント
	m_lights[0].ambColor = { 1.0f,1.0f,1.0f,1.0f };
	SetLightAmbColorHandle(m_lights[0].handle, m_lights[0].ambColor);


	/// カメラの向きのライト
	// ディフューズ
	m_lights[1].difColor = { 1.0f,1.0f,1.0f,1.0f };
	SetLightDifColorHandle(m_lights[1].handle, m_lights[1].difColor);
	// スペキュラー
	m_lights[1].spcColor = { 0.3f,0.3f,0.3f,0.3f };
	SetLightSpcColorHandle(m_lights[1].handle, m_lights[1].spcColor);

	// ライト方向の設定
	auto cameraToPlayer = VSub(frontPos, cameraPos);
	cameraToPlayer.y = 0;
	m_lights[0].dir = VNorm(cameraToPlayer);
	auto playerToCamera = VSub(VGet(cameraPos.x, -10, cameraPos.z), frontPos);
	m_lights[1].dir = playerToCamera;
	SetLightDirectionHandle(m_lights[0].handle, m_lights[0].dir);
	SetLightDirectionHandle(m_lights[1].handle, m_lights[1].dir);
}

void Light::Update(VECTOR cameraPos, VECTOR frontPos)
{
	auto cameraToPlayer = VSub(frontPos, cameraPos);
	m_lights[0].dir = VNorm(cameraToPlayer);
	auto playerToCamera = VSub(VGet(cameraPos.x, -10, cameraPos.z), frontPos);
	m_lights[1].dir = VNorm(playerToCamera);
	SetLightDirectionHandle(m_lights[0].handle, m_lights[0].dir);
	SetLightDirectionHandle(m_lights[1].handle, m_lights[1].dir);

#ifdef _DEBUG
	if(Controller::Instance().GetInputDown(Button::LB))
	{
		if(m_light < 7)
			m_light++;
		else
			m_light = 0;
		printf("Light状況\n");
		switch (m_light)
		{
		case 0:
			SetLightEnable(true);
			SetLightEnableHandle(m_lights[0].handle, true);
			SetLightEnableHandle(m_lights[1].handle, true);
			printf("A F P\n");
			break;
		case 1:
			SetLightEnable(true);
			SetLightEnableHandle(m_lights[0].handle, true);
			SetLightEnableHandle(m_lights[1].handle, false);
			printf("A F\n");
			break;
		case 2:
			SetLightEnable(true);
			SetLightEnableHandle(m_lights[0].handle, false);
			SetLightEnableHandle(m_lights[1].handle, true);
			printf("A P\n");
			break;
		case 3:
			SetLightEnable(false);
			SetLightEnableHandle(m_lights[0].handle, true);
			SetLightEnableHandle(m_lights[1].handle, true);
			printf("F P\n");
			break;
		case 4:
			SetLightEnable(true);
			SetLightEnableHandle(m_lights[0].handle, false);
			SetLightEnableHandle(m_lights[1].handle, false);
			printf("A\n");
			break;
		case 5:
			SetLightEnable(false);
			SetLightEnableHandle(m_lights[0].handle, true);
			SetLightEnableHandle(m_lights[1].handle, false);
			printf("F\n");
			break;
		case 6:
			SetLightEnable(false);
			SetLightEnableHandle(m_lights[0].handle, false);
			SetLightEnableHandle(m_lights[1].handle, true);
			printf("P\n");
			break;
		case 7:
			SetLightEnable(false);
			SetLightEnableHandle(m_lights[0].handle, false);
			SetLightEnableHandle(m_lights[1].handle, false);
			printf("NONE\n");
			break;
		}
	}
#endif // _DEBUG
	
}
