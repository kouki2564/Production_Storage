#include "ShadowMapShader.h"
#include"GameData.h"
#include <cassert>

namespace
{
	constexpr int shadowMapSizeX = 15360;
	constexpr int shadowMapSizeY = 8640;

	//カメラ視野角
	constexpr float kShadowMapFov = 45.0f * DX_PI_F / 180.0f;
	constexpr float kNormalFov = 60.0f * DX_PI_F / 180.0f;

}

ShadowMapShader::ShadowMapShader() :
	m_vertexShader1FrameHandle(LoadVertexShader("data/shader/VertexShader3D.vso")),
	m_vertexShader4FrameHandle(LoadVertexShader("data/shader/VertexShader3D4Frame.vso")),
	m_vertexShader8FrameHandle(LoadVertexShader("data/shader/VertexShader3D8Frame.vso")),
	m_vertexShaderNormal4FrameHandle(LoadVertexShader("data/shader/VertexShader3DNormal4Frame.vso")),
	m_vertexShaderNormal8FrameHandle(LoadVertexShader("data/shader/VertexShader3DNormal8Frame.vso")),
	m_vertexShaderShadowMapField(LoadVertexShader("data/shader/VertexShaderShadowMapField.vso")),
	m_pixelShaderHandle(LoadPixelShader("data/shader/PixelShaderShadowMap.pso")),
	m_pixelShaderFieldHandle(LoadPixelShader("data/shader/ToonPixelShaderField.pso")),
	m_shadowMap(MakeScreen(GameData::kScreenWidth, GameData::kScreenHeight))
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

	//ビュープロジェクション行列の定数バッファの作成
	m_cbufferHandleViewProjectionMat = CreateShaderConstantBuffer(sizeof(MATRIX));
	assert(m_cbufferHandleViewProjectionMat != -1);
	//ビュープロジェクション行列の定数バッファのアドレスを取得
	m_viewProjectionMat = static_cast<MATRIX*>(GetBufferShaderConstantBuffer(m_cbufferHandleViewProjectionMat));

	//シャドウマップの作成
	m_shadowMap = MakeScreen(shadowMapSizeX, shadowMapSizeY, true);
}

ShadowMapShader::~ShadowMapShader()
{
	//シェーダーの解放
	DeleteShader(m_vertexShader1FrameHandle);
	DeleteShader(m_vertexShader4FrameHandle);
	DeleteShader(m_vertexShader8FrameHandle);
	DeleteShader(m_vertexShaderNormal4FrameHandle);
	DeleteShader(m_vertexShaderNormal8FrameHandle);
	DeleteShader(m_vertexShaderShadowMapField);
	DeleteShader(m_pixelShaderHandle);
	DeleteShader(m_pixelShaderFieldHandle);

	DeleteShaderConstantBuffer(m_cbufferHandleLightDir);
	DeleteShaderConstantBuffer(m_cbufferHandleCameraTarget);
	DeleteShaderConstantBuffer(m_cbufferHandleCameraPos);
	DeleteGraph(m_shadowMap);

	DeleteShaderConstantBuffer(m_cbufferHandleViewProjectionMat);
}

void ShadowMapShader::StartWrite(VECTOR targetPos)
{
	//シャドウマップへの書き込み開始
	SetDrawScreen(m_shadowMap);
	ClearDrawScreen();
	DrawBox(0, 0, shadowMapSizeX, shadowMapSizeY, 0xffffff, true);

	//シャドウマップのカメラの位置と向きを設定
	SetCameraPositionAndTargetAndUpVec(GetLightPosition(), targetPos, VGet(1, 0, 0));

	//シャドウマップ書込み時のカメラの視野角(ラジアン)
	SetupCamera_Perspective(kShadowMapFov);
}

void ShadowMapShader::SetShadowMapCameraTarget(VECTOR targetPos)
{
	m_cbufferAddressCameraPos[0] = targetPos;		//カメラの座標取得
}

void ShadowMapShader::SetShader(int shaderType)
{
	m_cbufferAddressLightDir[0] = GetLightDirection();		//ライトの向き取得
	m_cbufferAddressCameraTarget[0] = GetCameraTarget();	//カメラの向き取得
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

void ShadowMapShader::SetShaderField(VECTOR targetPos)
{
	*m_viewProjectionMat = ViewProjectionMatrix(targetPos);

	m_cbufferAddressLightDir[0] = GetLightDirection();		//ライトの向き取得
	m_cbufferAddressCameraTarget[0] = GetCameraTarget();	//カメラの向き取得
	m_cbufferAddressCameraPos[0] = GetCameraPosition();		//カメラの座標取得

	SetUseTextureToShader(1, m_shadowMap);

	//書き込んだ値をGPUに反映
	UpdateShaderConstantBuffer(m_cbufferHandleLightDir);
	SetShaderConstantBuffer(m_cbufferHandleLightDir, DX_SHADERTYPE_PIXEL, 0);

	//書き込んだ値をGPUに反映
	UpdateShaderConstantBuffer(m_cbufferHandleCameraTarget);
	SetShaderConstantBuffer(m_cbufferHandleCameraTarget, DX_SHADERTYPE_PIXEL, 1);

	//書き込んだ値をGPUに反映
	UpdateShaderConstantBuffer(m_cbufferHandleCameraPos);
	SetShaderConstantBuffer(m_cbufferHandleCameraPos, DX_SHADERTYPE_PIXEL, 2);

	//書き込んだ値をGPUに反映
	UpdateShaderConstantBuffer(m_cbufferHandleViewProjectionMat);
	SetShaderConstantBuffer(m_cbufferHandleViewProjectionMat, DX_SHADERTYPE_VERTEX, 4);


	//オリジナルシェーダー適用
	MV1SetUseOrigShader(true);

	SetUseVertexShader(m_vertexShaderShadowMapField);

	//ピクセルシェーダーセット
	SetUsePixelShader(m_pixelShaderFieldHandle);
}

void ShadowMapShader::EndWrite()
{
	//シャドウマップへの書き込み終わり
	SetDrawScreen(DX_SCREEN_BACK);
	MV1SetUseOrigShader(false);

	//通常時のカメラの視野角(ラジアン)
	SetupCamera_Perspective(kNormalFov);
}

MATRIX ShadowMapShader::ViewProjectionMatrix(VECTOR targetPos)
{
	//ビュー行列の変数
	MATRIX viewMat = GetCameraViewportMatrix();

	VECTOR cameraPos = GetLightPosition();
	VECTOR cameraTargetPos = targetPos;
	VECTOR UpVec = VGet(1, 0, 0);

	//ビュー行列を算出
	CreateLookAtMatrix(&viewMat, &cameraPos, &cameraTargetPos, &UpVec);
	//射影行列を作成
	MATRIX mat;

	//射影行列算出
	//行列、視野角、neir,far,(アスペクト比)
	CreatePerspectiveFovMatrix(&mat, kShadowMapFov, GetCameraNear(), GetCameraFar(), 9.0f / 16.0f);

	//ビュープロジェクション行列の作成
	MATRIX viewProjectionMat = MMult(viewMat, mat);

	return viewProjectionMat;
}
