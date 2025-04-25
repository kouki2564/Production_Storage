#include "ToonShader.h"
#include "GameData.h"
#include <cassert>
#include <array>

ToonShader::ToonShader() :
	m_vertexShader1FrameHandle(LoadVertexShader("data/shader/VertexShader3D.vso")),
	m_vertexShader4FrameHandle(LoadVertexShader("data/shader/VertexShader3D4Frame.vso")),
	m_vertexShader8FrameHandle(LoadVertexShader("data/shader/VertexShader3D8Frame.vso")),
	m_vertexShaderNormal4FrameHandle(LoadVertexShader("data/shader/VertexShader3DNormal4Frame.vso")),
	m_vertexShaderNormal8FrameHandle(LoadVertexShader("data/shader/VertexShader3DNormal8Frame.vso")),
	m_pixelShaderHandle(LoadPixelShader("data/shader/ToonPixelShader.pso")),
	m_pixelShaderFieldHandle(LoadPixelShader("data/shader/ToonPixelShaderField.pso")),
	m_graphHandle(MakeScreen(GameData::kScreenWidth, GameData::kScreenHeight))
{
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

ToonShader::~ToonShader()
{
	//シェーダーの解放
	DeleteShader(m_vertexShader1FrameHandle);
	DeleteShader(m_vertexShader4FrameHandle);
	DeleteShader(m_vertexShader8FrameHandle);
	DeleteShader(m_vertexShaderNormal4FrameHandle);
	DeleteShader(m_vertexShaderNormal8FrameHandle);
	DeleteShader(m_pixelShaderHandle);
	DeleteShader(m_pixelShaderFieldHandle);

	DeleteShaderConstantBuffer(m_cbufferHandleLightDir);
	DeleteShaderConstantBuffer(m_cbufferHandleCameraTarget);
	DeleteShaderConstantBuffer(m_cbufferHandleCameraPos);
	DeleteGraph(m_graphHandle);
}

void ToonShader::Update()
{
	
}

void ToonShader::SetShader(int shaderType)
{
	m_cbufferAddressLightDir[0] = GetLightDirection();		//ライトの向き取得
	m_cbufferAddressCameraTarget[0] = GetCameraTarget();	//カメラの向き取得
	m_cbufferAddressCameraPos[0] = GetCameraPosition();		//カメラの座標取得

	//書き込んだ値をGPUに反映
	UpdateShaderConstantBuffer(m_cbufferHandleLightDir);
	SetShaderConstantBuffer(m_cbufferHandleLightDir, DX_SHADERTYPE_PIXEL, 0);

	//書き込んだ値をGPUに反映
	UpdateShaderConstantBuffer(m_cbufferHandleCameraTarget);
	SetShaderConstantBuffer(m_cbufferHandleCameraTarget, DX_SHADERTYPE_PIXEL, 1);

	//書き込んだ値をGPUに反映
	UpdateShaderConstantBuffer(m_cbufferHandleCameraPos);
	SetShaderConstantBuffer(m_cbufferHandleCameraPos, DX_SHADERTYPE_PIXEL, 2);

	//オリジナルシェーダー適応
	MV1SetUseOrigShader(true);

	//モデルのタイプによって適応させるシェーダーを変える
	if (shaderType == DX_MV1_VERTEX_TYPE_1FRAME)
	{
		SetUseVertexShader(m_vertexShader1FrameHandle);

	}
	else if (shaderType == DX_MV1_VERTEX_TYPE_4FRAME)
	{
		SetUseVertexShader(m_vertexShader4FrameHandle);
	}
	else if (shaderType == DX_MV1_VERTEX_TYPE_8FRAME)
	{
		SetUseVertexShader(m_vertexShader8FrameHandle);
	}
	else if (shaderType == DX_MV1_VERTEX_TYPE_NMAP_1FRAME)
	{
		SetUseVertexShader(m_vertexShader1FrameHandle);
	}
	else if (shaderType == DX_MV1_VERTEX_TYPE_NMAP_4FRAME)
	{
		SetUseVertexShader(m_vertexShaderNormal4FrameHandle);
	}
	else if (shaderType == DX_MV1_VERTEX_TYPE_NMAP_8FRAME)
	{
		SetUseVertexShader(m_vertexShaderNormal8FrameHandle);
	}
	else if (shaderType == DX_MV1_VERTEX_TYPE_NMAP_FREE_FRAME)
	{
		SetUseVertexShader(m_vertexShader1FrameHandle);
	}
	else
	{
		MV1SetUseOrigShader(false);
	}
	
	//ピクセルシェーダ―セット
	SetUsePixelShader(m_pixelShaderHandle);
}

void ToonShader::SetShaderField(int shaderType)
{
	m_cbufferAddressLightDir[0] = GetLightDirection();		//ライトの向き取得
	m_cbufferAddressCameraTarget[0] = GetCameraTarget();	//カメラの向き取得
	m_cbufferAddressCameraPos[0] = GetCameraPosition();		//カメラの座標取得

	//書き込んだ値をGPUに反映
	UpdateShaderConstantBuffer(m_cbufferHandleLightDir);
	SetShaderConstantBuffer(m_cbufferHandleLightDir, DX_SHADERTYPE_PIXEL, 0);

	//書き込んだ値をGPUに反映
	UpdateShaderConstantBuffer(m_cbufferHandleCameraTarget);
	SetShaderConstantBuffer(m_cbufferHandleCameraTarget, DX_SHADERTYPE_PIXEL, 1);

	//書き込んだ値をGPUに反映
	UpdateShaderConstantBuffer(m_cbufferHandleCameraPos);
	SetShaderConstantBuffer(m_cbufferHandleCameraPos, DX_SHADERTYPE_PIXEL, 2);

	//オリジナルシェーダー適応
	MV1SetUseOrigShader(true);

	//モデルのタイプによって適応させるシェーダーを変える
	if (shaderType == DX_MV1_VERTEX_TYPE_1FRAME)
	{
		SetUseVertexShader(m_vertexShader1FrameHandle);

	}
	else if (shaderType == DX_MV1_VERTEX_TYPE_4FRAME)
	{
		SetUseVertexShader(m_vertexShader4FrameHandle);
	}
	else if (shaderType == DX_MV1_VERTEX_TYPE_8FRAME)
	{
		SetUseVertexShader(m_vertexShader8FrameHandle);
	}
	else if (shaderType == DX_MV1_VERTEX_TYPE_NMAP_1FRAME)
	{
		SetUseVertexShader(m_vertexShader1FrameHandle);
	}
	else if (shaderType == DX_MV1_VERTEX_TYPE_NMAP_4FRAME)
	{
		SetUseVertexShader(m_vertexShaderNormal4FrameHandle);
	}
	else if (shaderType == DX_MV1_VERTEX_TYPE_NMAP_8FRAME)
	{
		SetUseVertexShader(m_vertexShaderNormal8FrameHandle);
	}
	else if (shaderType == DX_MV1_VERTEX_TYPE_NMAP_FREE_FRAME)
	{
		SetUseVertexShader(m_vertexShader1FrameHandle);
	}
	else
	{
		MV1SetUseOrigShader(false);
	}

	//ピクセルシェーダーセット
	SetUsePixelShader(m_pixelShaderFieldHandle);
}

void ToonShader::EndShader()
{
	//シェーダーを使わない
	MV1SetUseOrigShader(false);
}