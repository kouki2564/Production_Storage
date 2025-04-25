#pragma once
#include<memory>
#include<DxLib.h>


class Model;

class DamageShader
{
public:
	DamageShader(std::shared_ptr<Model> pModel);
	virtual ~DamageShader();

	void Update();	//更新処理

	void DrawFirst1Frame();	//モデルタイプが1Frameの時の描画
	void DrawFirst4Frame(); //モデルタイプが4Frameの時の描画

	void DrawEnd();	//描画終了

	void UseShader();//シェーダーを使う

	//シェーダーを使っているかどうか
	bool GetUseShader() const { return m_isUseShader; }

private:
	int m_pixelShaderHandle;		//ピクセルシェーダーのハンドル
	int m_vertexShader1FrameHandle;		//頂点シェーダーのハンドル
	int m_vertexShader4FrameHandle;		//頂点シェーダーのハンドル

	std::shared_ptr<Model> m_pModel;//モデル

	int m_graphHandle;		//画像ハンドル
	
	int m_cbufferHandleTime;	//定数バッファハンドル
	int m_cbufferHandleLightDir;	//定数バッファハンドル
	int m_cbufferHandleCameraTarget;	//定数バッファハンドル
	int m_cbufferHandleCameraPos;	//定数バッファハンドル

	float* m_cbufferTime;	//定数バッファをいじるためのコピー
	VECTOR* m_cbufferAddressLightDir;	//定数バッファをいじるためのコピー
	VECTOR* m_cbufferAddressCameraTarget;	//定数バッファをいじるためのコピー
	VECTOR* m_cbufferAddressCameraPos;	//定数バッファをいじるためのコピー

	float m_colDensity;		//色の濃さ

	bool m_isUseShader;		//シェーダーを使うかどうか
};

