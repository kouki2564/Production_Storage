#include "DeadShader.h"
#include "GameData.h"
#include <cassert>

namespace
{
	//1フレームごとに減少する色の濃さ
	constexpr float kColDensity = 0.008f;

	//シェーダーを使うタイミングで色の濃さを設定する
	constexpr float kColDensityOnShader = 0.55f;
}

DeadShader::DeadShader() :
	m_vertexShader1FrameHandle(LoadVertexShader("data/shader/VertexShader3D.vso")),
	m_vertexShader4FrameHandle(LoadVertexShader("data/shader/VertexShader3D4Frame.vso")),
	m_vertexShader8FrameHandle(LoadVertexShader("data/shader/VertexShader3D8Frame.vso")),
	m_vertexShaderNormal4FrameHandle(LoadVertexShader("data/shader/VertexShader3DNormal4Frame.vso")),
	m_vertexShaderNormal8FrameHandle(LoadVertexShader("data/shader/VertexShader3DNormal8Frame.vso")),
	m_pixelShaderHandle(LoadPixelShader("data/shader/PixelShaderDead.pso"))
{
	//定数バッファの作成
	m_cbufferHandle = CreateShaderConstantBuffer(sizeof(float) * 4);
	assert(m_cbufferHandle != -1);

	//定数バッファのアドレスを取得
	m_cbufferAddress = static_cast<float*>(GetBufferShaderConstantBuffer(m_cbufferHandle));

	m_isUseShader = false;
	m_colDensity = 1.0f;

	//定数バッファアドレスに色の濃さを設定
	m_cbufferAddress[0] = m_colDensity;
}

DeadShader::~DeadShader()
{
	//シェーダーの解放
	DeleteShader(m_vertexShader1FrameHandle);
	DeleteShader(m_vertexShader4FrameHandle);
	DeleteShader(m_vertexShader8FrameHandle);
	DeleteShader(m_vertexShaderNormal4FrameHandle);
	DeleteShader(m_vertexShaderNormal8FrameHandle);
	DeleteShader(m_pixelShaderHandle);
	DeleteShaderConstantBuffer(m_cbufferHandle);
}

void DeadShader::Update()
{
	//シェーダーが使われている間は更新
	if (m_isUseShader)
	{
		//色の濃さを減らす
		m_colDensity = max(m_colDensity - kColDensity, 0.0f);

		m_cbufferAddress[0] = m_colDensity;
	}

}

void DeadShader::OnShader()
{
	//シェーダーを使う
	m_isUseShader = true;

	//色の濃さを設定
	if (m_colDensity > kColDensityOnShader)
	{
		m_colDensity = kColDensityOnShader;
	}
}

void DeadShader::SetShader(int shaderType)
{
	//オリジナルシェーダー適応
	MV1SetUseOrigShader(true);

	//書き込んだ値をGPUに反映
	UpdateShaderConstantBuffer(m_cbufferHandle);
	SetShaderConstantBuffer(m_cbufferHandle, DX_SHADERTYPE_PIXEL, 0);

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

void DeadShader::EndShader()
{
	//シェーダーを使わない
	MV1SetUseOrigShader(false);
}
