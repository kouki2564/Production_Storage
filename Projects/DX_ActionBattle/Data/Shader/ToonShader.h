#pragma once
#include <DxLib.h>

class ToonShader
{
public:
	ToonShader();
	virtual ~ToonShader();

	void Update();	//更新処理

	void SetShader(int shaderType);
	void SetShaderField(int shaderType);
	void EndShader();

private:
	int m_vertexShader1FrameHandle;				//頂点シェーダー1フレームハンドル
	int m_vertexShader4FrameHandle;				//頂点シェーダー4フレームハンドル
	int m_vertexShader8FrameHandle;				//頂点シェーダー8フレームハンドル
	int m_vertexShaderNormal4FrameHandle;		//頂点シェーダー4フレーム法線マップありハンドル
	int m_vertexShaderNormal8FrameHandle;		//頂点シェーダー8フレーム法線マップありハンドル
	int m_pixelShaderHandle;		//ピクセルシェーダのハンドル
	int m_pixelShaderFieldHandle;	//ピクセルシェーダーのフィールドに適用させるハンドル

	int m_graphHandle;		//画像ハンドル

	int m_cbufferHandleLightDir;	//定数バッファハンドル
	int m_cbufferHandleCameraTarget;	//定数バッファハンドル
	int m_cbufferHandleCameraPos;	//定数バッファハンドル

	VECTOR* m_cbufferAddressLightDir;	//定数バッファをいじるためのコピー
	VECTOR* m_cbufferAddressCameraTarget;	//定数バッファをいじるためのコピー
	VECTOR* m_cbufferAddressCameraPos;	//定数バッファをいじるためのコピー
};

