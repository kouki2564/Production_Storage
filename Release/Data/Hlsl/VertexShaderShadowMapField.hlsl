#include "Shader/VertexShader.h"

// 頂点シェーダーの入力
struct VS_INPUT
{
    float3 Position : POSITION; // 座標( ローカル空間 )
    float3 Normal : NORMAL0; // 法線( ローカル空間 )
    float4 Diffuse : COLOR0; // ディフューズカラー
    float4 Specular : COLOR1; // スペキュラカラー
    float4 TexCoords0 : TEXCOORD0; // テクスチャ座標
    float4 TexCoords1 : TEXCOORD1; // サブテクスチャ座標
};

// 頂点シェーダーの出力
struct VS_OUTPUT
{
    float4 Diffuse : COLOR0; // ディフューズカラー
    float4 Specular : COLOR1; // スペキュラカラー
    float4 TexCoords0_1 : TEXCOORD0; // xy:テクスチャ座標 zw:サブテクスチャ座標
    float3 VPosition : TEXCOORD1; // 座標( ビュー空間 )
    float3 VNormal : TEXCOORD2; // 法線( ビュー空間 )

    float1 Fog : TEXCOORD5; // フォグパラメータ( x )

    float4 Position : SV_POSITION; // 座標( プロジェクション空間 )
    
    //ライトビュースクリーン空間での座標を追加
    float4 posInLVP : TEXCOORD3; // ライトビュースクリーン空間でのピクセルの座標
};

cbuffer ShadowCb : register(b4)
{
    float4x4 mLVP;
};

#ifdef SKINMESH
#define LOCAL_WORLD_MAT         lLocalWorldMatrix
#else
#define LOCAL_WORLD_MAT         g_Base.LocalWorldMatrix
#endif






// main関数
VS_OUTPUT main(VS_INPUT VSInput)
{
    VS_OUTPUT VSOutput;
    int4 lBoneFloatIndex;
    float4 lLocalWorldMatrix[3];
    float4 lLocalPosition;
    float4 lWorldPosition;
    float4 lViewPosition;
    float3 lWorldNrm;
    float3 lWorldTan;
    float3 lWorldBin;
    float3 lViewNrm;
    float3 lViewTan;
    float3 lViewBin;

	// 頂点座標変換 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++( 開始 )

	// ローカル座標のセット
    lLocalPosition.xyz = VSInput.Position;
    lLocalPosition.w = 1.0f;

	// 座標計算( ローカル→ビュー→プロジェクション )
    lWorldPosition.x = dot(lLocalPosition, LOCAL_WORLD_MAT[0]);
    lWorldPosition.y = dot(lLocalPosition, LOCAL_WORLD_MAT[1]);
    lWorldPosition.z = dot(lLocalPosition, LOCAL_WORLD_MAT[2]);
    lWorldPosition.w = 1.0f;

    lViewPosition.x = dot(lWorldPosition, g_Base.ViewMatrix[0]);
    lViewPosition.y = dot(lWorldPosition, g_Base.ViewMatrix[1]);
    lViewPosition.z = dot(lWorldPosition, g_Base.ViewMatrix[2]);
    lViewPosition.w = 1.0f;

    VSOutput.Position.x = dot(lViewPosition, g_Base.ProjectionMatrix[0]);
    VSOutput.Position.y = dot(lViewPosition, g_Base.ProjectionMatrix[1]);
    VSOutput.Position.z = dot(lViewPosition, g_Base.ProjectionMatrix[2]);
    VSOutput.Position.w = dot(lViewPosition, g_Base.ProjectionMatrix[3]);
	
	// 座標( ビュー空間 )を保存
    VSOutput.VPosition = lViewPosition.xyz;
   
	// 法線を計算
    lWorldNrm.x = dot(VSInput.Normal, LOCAL_WORLD_MAT[0].xyz);
    lWorldNrm.y = dot(VSInput.Normal, LOCAL_WORLD_MAT[1].xyz);
    lWorldNrm.z = dot(VSInput.Normal, LOCAL_WORLD_MAT[2].xyz);

    lViewNrm.x = dot(lWorldNrm, g_Base.ViewMatrix[0].xyz);
    lViewNrm.y = dot(lWorldNrm, g_Base.ViewMatrix[1].xyz);
    lViewNrm.z = dot(lWorldNrm, g_Base.ViewMatrix[2].xyz);

	// 法線( ビュー空間 )を保存
    VSOutput.VNormal = lWorldNrm;
    
	// ディフューズカラーをセット
    VSOutput.Diffuse = g_Base.DiffuseSource > 0.5f ? VSInput.Diffuse : g_Common.Material.Diffuse;
	
	// スペキュラカラーをセット
    VSOutput.Specular = (g_Base.SpecularSource > 0.5f ? VSInput.Specular : g_Common.Material.Specular) * g_Base.MulSpecularColor;

    //ライトビュースクリーン空間の座標を計算する
    VSOutput.posInLVP = mul(mLVP, lWorldPosition);
    
	// 頂点座標変換 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++( 終了 )

	// フォグ計算 =============================================( 開始 )
	
    VSOutput.Fog.x = 1.0f;

	// フォグ計算 =============================================( 終了 )

	// テクスチャ座標のセット
    VSOutput.TexCoords0_1.x = dot(VSInput.TexCoords0, g_OtherMatrix.TextureMatrix[0][0]);
    VSOutput.TexCoords0_1.y = dot(VSInput.TexCoords0, g_OtherMatrix.TextureMatrix[0][1]);

    VSOutput.TexCoords0_1.z = dot(VSInput.TexCoords1, g_OtherMatrix.TextureMatrix[0][0]);
    VSOutput.TexCoords0_1.w = dot(VSInput.TexCoords1, g_OtherMatrix.TextureMatrix[0][1]);

    return VSOutput;
}

