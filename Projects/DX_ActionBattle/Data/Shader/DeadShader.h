#pragma once
#include<memory>
#include<DxLib.h>

class Model;

class DeadShader
{
public:
	DeadShader();
	virtual ~DeadShader();

	void Update();	//更新処理

	void OnShader();//シェーダーを使う
	void SetShader(int shaderType);	//描画処理
	void EndShader();

	//シェーダーを使っているかどうか
	bool GetUseShader() const { return m_isUseShader; }

private:
	int m_vertexShader1FrameHandle;				//頂点シェーダー1フレームハンドル
	int m_vertexShader4FrameHandle;				//頂点シェーダー4フレームハンドル
	int m_vertexShader8FrameHandle;				//頂点シェーダー8フレームハンドル
	int m_vertexShaderNormal4FrameHandle;		//頂点シェーダー4フレーム法線マップありハンドル
	int m_vertexShaderNormal8FrameHandle;		//頂点シェーダー8フレーム法線マップありハンドル
	int m_pixelShaderHandle;		//ピクセルシェーダのハンドル

	int m_cbufferHandle;	//定数バッファハンドル

	float* m_cbufferAddress;	//定数バッファをいじるためのコピー

	float m_colDensity;		//色の濃さ

	bool m_isUseShader;		//シェーダーを使うかどうか
};

