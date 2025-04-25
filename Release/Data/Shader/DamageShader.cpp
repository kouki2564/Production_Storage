#include "DamageShader.h"
#include <array>
#include<cassert>
// #include"../Object/Model.h"
#include"GameData.h"

DamageShader::DamageShader(std::shared_ptr<Model> pModel) : 
	m_pixelShaderHandle(LoadPixelShader("data/shader/DamageShader.pso")),
	m_vertexShader1FrameHandle(LoadVertexShader("data/shader/VertexShader3D.vso")),
	m_vertexShader4FrameHandle(LoadVertexShader("data/shader/VertexShader3D4Frame.vso")),
	m_pModel(pModel),
	m_graphHandle(MakeScreen(GameData::kScreenWidth, GameData::kScreenHeight)),
	m_colDensity(0.0f),
	m_isUseShader(false)
{
	//時間定数バッファの作成
	m_cbufferHandleTime = CreateShaderConstantBuffer(sizeof(float) * 4);
	assert(m_cbufferHandleTime != -1);
	//時間定数バッファのアドレスを取得
	m_cbufferTime = static_cast<float*>(GetBufferShaderConstantBuffer(m_cbufferHandleTime));

	//ライト定数バッファの作成
	m_cbufferHandleLightDir = CreateShaderConstantBuffer(sizeof(float) * 4);
	assert(m_cbufferHandleLightDir != -1);
	//ライト定数バッファのアドレスを取得
	m_cbufferAddressLightDir = static_cast<VECTOR*>(GetBufferShaderConstantBuffer(m_cbufferHandleLightDir));

	//カメラターゲット定数バッファの作成
	m_cbufferHandleCameraTarget = CreateShaderConstantBuffer(sizeof(float) * 4);
	assert(m_cbufferHandleCameraTarget != -1);
	//カメラターゲット定数バッファのアドレスを取得
	m_cbufferAddressCameraTarget = static_cast<VECTOR*>(GetBufferShaderConstantBuffer(m_cbufferHandleCameraTarget));

	//カメラ座標定数バッファの作成
	m_cbufferHandleCameraPos = CreateShaderConstantBuffer(sizeof(float) * 4);
	assert(m_cbufferHandleCameraPos != -1);
	//カメラ座標定数バッファのアドレスを取得
	m_cbufferAddressCameraPos = static_cast<VECTOR*>(GetBufferShaderConstantBuffer(m_cbufferHandleCameraPos));
}

DamageShader::~DamageShader()
{
	//シェーダーの解放
	DeleteShader(m_vertexShader1FrameHandle);
	DeleteShader(m_vertexShader4FrameHandle);
	DeleteShader(m_pixelShaderHandle);

	DeleteGraph(m_graphHandle);

	DeleteShaderConstantBuffer(m_cbufferHandleTime);
}

void DamageShader::Update()
{
	m_colDensity = max(m_colDensity - 0.02f, 0.0f);

	if (m_colDensity <= 0.0f)
	{
		m_isUseShader = false;
	}

	m_cbufferTime[0] = m_colDensity;
}

void DamageShader::DrawFirst1Frame()
{
	m_cbufferAddressLightDir[0] = GetLightDirection();		//ライトの向き取得
	m_cbufferAddressCameraTarget[0] = GetCameraTarget();	//カメラの向き取得
	m_cbufferAddressCameraPos[0] = GetCameraPosition();		//カメラの座標取得

	//書き込んだ値をGPUに反映
	UpdateShaderConstantBuffer(m_cbufferHandleTime);
	SetShaderConstantBuffer(m_cbufferHandleTime, DX_SHADERTYPE_PIXEL, 0);

	//書き込んだ値をGPUに反映
	UpdateShaderConstantBuffer(m_cbufferHandleLightDir);
	SetShaderConstantBuffer(m_cbufferHandleLightDir, DX_SHADERTYPE_PIXEL, 1);

	//書き込んだ値をGPUに反映
	UpdateShaderConstantBuffer(m_cbufferHandleCameraTarget);
	SetShaderConstantBuffer(m_cbufferHandleCameraTarget, DX_SHADERTYPE_PIXEL, 2);

	//書き込んだ値をGPUに反映
	UpdateShaderConstantBuffer(m_cbufferHandleCameraPos);
	SetShaderConstantBuffer(m_cbufferHandleCameraPos, DX_SHADERTYPE_PIXEL, 3);
	
	//シェーダーを使う
	MV1SetUseOrigShader(true);
	SetUseVertexShader(m_vertexShader1FrameHandle);
	SetUsePixelShader(m_pixelShaderHandle);

}

void DamageShader::DrawFirst4Frame()
{
	m_cbufferAddressLightDir[0] = GetLightDirection();		//ライトの向き取得
	m_cbufferAddressCameraTarget[0] = GetCameraTarget();	//カメラの向き取得
	m_cbufferAddressCameraPos[0] = GetCameraPosition();		//カメラの座標取得

	//書き込んだ値をGPUに反映
	UpdateShaderConstantBuffer(m_cbufferHandleTime);
	SetShaderConstantBuffer(m_cbufferHandleTime, DX_SHADERTYPE_PIXEL, 0);

	//書き込んだ値をGPUに反映
	UpdateShaderConstantBuffer(m_cbufferHandleLightDir);
	SetShaderConstantBuffer(m_cbufferHandleLightDir, DX_SHADERTYPE_PIXEL, 1);

	//書き込んだ値をGPUに反映
	UpdateShaderConstantBuffer(m_cbufferHandleCameraTarget);
	SetShaderConstantBuffer(m_cbufferHandleCameraTarget, DX_SHADERTYPE_PIXEL, 2);

	//書き込んだ値をGPUに反映
	UpdateShaderConstantBuffer(m_cbufferHandleCameraPos);
	SetShaderConstantBuffer(m_cbufferHandleCameraPos, DX_SHADERTYPE_PIXEL, 3);

	//シェーダーを使う
	MV1SetUseOrigShader(true);
	SetUseVertexShader(m_vertexShader4FrameHandle);
	SetUsePixelShader(m_pixelShaderHandle);
}

void DamageShader::DrawEnd()
{
	//描画終了
	MV1SetUseOrigShader(false);
	SetShaderConstantBuffer(-1, DX_SHADERTYPE_PIXEL, 8);
}

void DamageShader::UseShader()
{
	//シェーダーを使う
	m_isUseShader = true;
	m_colDensity = 1.0f;
}