#pragma once
#include<DxLib.h>

class ShadowMapShader
{
public:
	ShadowMapShader();
	~ShadowMapShader();
	
	//シャドウマップへの書き込み開始
	void StartWrite(VECTOR targetPos);
	
	//シャドウマップのカメラから見たターゲットを設定
	void SetShadowMapCameraTarget(VECTOR targetPos);

	void SetShader(int shaderType);
	void SetShaderField(VECTOR targetPos);

	//シャドウマップへの書き込み終わり
	void EndWrite();



private:
	//ビュープロジェクション行列を作るための関数
	MATRIX ViewProjectionMatrix(VECTOR targetPos);
private:
	int m_vertexShader1FrameHandle;				//頂点シェーダー1フレームハンドル
	int m_vertexShader4FrameHandle;				//頂点シェーダー4フレームハンドル
	int m_vertexShader8FrameHandle;				//頂点シェーダー8フレームハンドル
	int m_vertexShaderNormal4FrameHandle;		//頂点シェーダー4フレーム法線マップありハンドル
	int m_vertexShaderNormal8FrameHandle;		//頂点シェーダー8フレーム法線マップありハンドル
	
	//頂点シェーダーのシャドウマップをフィールドに張り付けるための頂点シェーダー
	int m_vertexShaderShadowMapField;		
	int m_pixelShaderHandle;		//ピクセルシェーダのハンドル
	int m_pixelShaderFieldHandle;	//ピクセルシェーダーのフィールドに適用させるハンドル

	int m_cbufferHandleLightDir;	//定数バッファハンドル
	int m_cbufferHandleCameraTarget;	//定数バッファハンドル
	int m_cbufferHandleCameraPos;	//定数バッファハンドル
	int m_cbufferHandleViewProjectionMat;	//定数バッファハンドル

	VECTOR* m_cbufferAddressLightDir;	//定数バッファをいじるためのコピー
	VECTOR* m_cbufferAddressCameraTarget;	//定数バッファをいじるためのコピー
	VECTOR* m_cbufferAddressCameraPos;	//定数バッファをいじるためのコピー

	//ビュープロジェクション行列
	//シャドウマップテクスチャをフィールドモデル
	//に張り付ける際に座標の変換を行うために必要
	MATRIX* m_viewProjectionMat;
	int m_shadowMap = -1;	//シャドウマップ用の変数
};

