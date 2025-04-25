#include "Shader/VertexShader.h"

// 頂点シェーダーの入力
struct VS_INPUT
{
    float3 Position : POSITION0; // 座標( ローカル空間 )
    float3 Normal : NORMAL; // 法線
    float4 Diffuse : COLOR0; // ディフューズカラー
    float4 Specular : COLOR1; // スペキュラカラー
    float2 TexCoords0 : TEXCOORD0; // テクスチャ座標０
    float2 TexCoords1 : TEXCOORD1; // テクスチャ座標１

	// スキニングメッシュ
	int4   BlendIndices0   : BLENDINDICES0 ;	// ボーン処理用 Float型定数配列インデックス０
	float4 BlendWeight0    : BLENDWEIGHT0 ;		// ボーン処理用ウエイト値０

	int4   BlendIndices1   : BLENDINDICES1 ;	// ボーン処理用 Float型定数配列インデックス１
	float4 BlendWeight1    : BLENDWEIGHT1 ;		// ボーン処理用ウエイト値１
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

#if SHADOWMAP
	float3 ShadowMap0Pos   : TEXCOORD6 ;	// シャドウマップ０のライト座標( x, y, z )
	float3 ShadowMap1Pos   : TEXCOORD7 ;	// シャドウマップ１のライト座標( x, y, z )
	float3 ShadowMap2Pos   : TEXCOORD8 ;	// シャドウマップ２のライト座標( x, y, z )
#endif // SHADOWMAP

    float4 Position : SV_POSITION; // 座標( プロジェクション空間 )
};

#define LOCAL_WORLD_MAT         lLocalWorldMatrix

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
    float4 lTextureCoordTemp;
#if SHADOWMAP
	float4		lLViewPosition ;
#endif

	// 頂点座標変換 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++( 開始 )

	// スキンメッシュ

	// ブレンド行列の作成
	lBoneFloatIndex = VSInput.BlendIndices0 ;
	lLocalWorldMatrix[ 0 ]  = g_LocalWorldMatrix.Matrix[ lBoneFloatIndex.x + 0 ] * VSInput.BlendWeight0.xxxx;
	lLocalWorldMatrix[ 1 ]  = g_LocalWorldMatrix.Matrix[ lBoneFloatIndex.x + 1 ] * VSInput.BlendWeight0.xxxx;
	lLocalWorldMatrix[ 2 ]  = g_LocalWorldMatrix.Matrix[ lBoneFloatIndex.x + 2 ] * VSInput.BlendWeight0.xxxx;

	lLocalWorldMatrix[ 0 ] += g_LocalWorldMatrix.Matrix[ lBoneFloatIndex.y + 0 ] * VSInput.BlendWeight0.yyyy;
	lLocalWorldMatrix[ 1 ] += g_LocalWorldMatrix.Matrix[ lBoneFloatIndex.y + 1 ] * VSInput.BlendWeight0.yyyy;
	lLocalWorldMatrix[ 2 ] += g_LocalWorldMatrix.Matrix[ lBoneFloatIndex.y + 2 ] * VSInput.BlendWeight0.yyyy;

	lLocalWorldMatrix[ 0 ] += g_LocalWorldMatrix.Matrix[ lBoneFloatIndex.z + 0 ] * VSInput.BlendWeight0.zzzz;
	lLocalWorldMatrix[ 1 ] += g_LocalWorldMatrix.Matrix[ lBoneFloatIndex.z + 1 ] * VSInput.BlendWeight0.zzzz;
	lLocalWorldMatrix[ 2 ] += g_LocalWorldMatrix.Matrix[ lBoneFloatIndex.z + 2 ] * VSInput.BlendWeight0.zzzz;

	lLocalWorldMatrix[ 0 ] += g_LocalWorldMatrix.Matrix[ lBoneFloatIndex.w + 0 ] * VSInput.BlendWeight0.wwww;
	lLocalWorldMatrix[ 1 ] += g_LocalWorldMatrix.Matrix[ lBoneFloatIndex.w + 1 ] * VSInput.BlendWeight0.wwww;
	lLocalWorldMatrix[ 2 ] += g_LocalWorldMatrix.Matrix[ lBoneFloatIndex.w + 2 ] * VSInput.BlendWeight0.wwww;



	lBoneFloatIndex = VSInput.BlendIndices1 ;
	lLocalWorldMatrix[ 0 ] += g_LocalWorldMatrix.Matrix[ lBoneFloatIndex.x + 0 ] * VSInput.BlendWeight1.xxxx;
	lLocalWorldMatrix[ 1 ] += g_LocalWorldMatrix.Matrix[ lBoneFloatIndex.x + 1 ] * VSInput.BlendWeight1.xxxx;
	lLocalWorldMatrix[ 2 ] += g_LocalWorldMatrix.Matrix[ lBoneFloatIndex.x + 2 ] * VSInput.BlendWeight1.xxxx;

	lLocalWorldMatrix[ 0 ] += g_LocalWorldMatrix.Matrix[ lBoneFloatIndex.y + 0 ] * VSInput.BlendWeight1.yyyy;
	lLocalWorldMatrix[ 1 ] += g_LocalWorldMatrix.Matrix[ lBoneFloatIndex.y + 1 ] * VSInput.BlendWeight1.yyyy;
	lLocalWorldMatrix[ 2 ] += g_LocalWorldMatrix.Matrix[ lBoneFloatIndex.y + 2 ] * VSInput.BlendWeight1.yyyy;

	lLocalWorldMatrix[ 0 ] += g_LocalWorldMatrix.Matrix[ lBoneFloatIndex.z + 0 ] * VSInput.BlendWeight1.zzzz;
	lLocalWorldMatrix[ 1 ] += g_LocalWorldMatrix.Matrix[ lBoneFloatIndex.z + 1 ] * VSInput.BlendWeight1.zzzz;
	lLocalWorldMatrix[ 2 ] += g_LocalWorldMatrix.Matrix[ lBoneFloatIndex.z + 2 ] * VSInput.BlendWeight1.zzzz;

	lLocalWorldMatrix[ 0 ] += g_LocalWorldMatrix.Matrix[ lBoneFloatIndex.w + 0 ] * VSInput.BlendWeight1.wwww;
	lLocalWorldMatrix[ 1 ] += g_LocalWorldMatrix.Matrix[ lBoneFloatIndex.w + 1 ] * VSInput.BlendWeight1.wwww;
	lLocalWorldMatrix[ 2 ] += g_LocalWorldMatrix.Matrix[ lBoneFloatIndex.w + 2 ] * VSInput.BlendWeight1.wwww;

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
    VSOutput.VNormal = lViewNrm;

	// ディフューズカラーをセット
    VSOutput.Diffuse = g_Base.DiffuseSource > 0.5f ? VSInput.Diffuse : g_Common.Material.Diffuse;
	
	// スペキュラカラーをセット
    VSOutput.Specular = (g_Base.SpecularSource > 0.5f ? VSInput.Specular : g_Common.Material.Specular) * g_Base.MulSpecularColor;

	// 頂点座標変換 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++( 終了 )
	
    VSOutput.Fog.x = 1.0f;

	// フォグ計算 =============================================( 終了 )

	// テクスチャ座標のセット
    lTextureCoordTemp.zw = 1.0f;

    lTextureCoordTemp.xy = VSInput.TexCoords0;
    VSOutput.TexCoords0_1.x = dot(lTextureCoordTemp, g_OtherMatrix.TextureMatrix[0][0]);
    VSOutput.TexCoords0_1.y = dot(lTextureCoordTemp, g_OtherMatrix.TextureMatrix[0][1]);

    lTextureCoordTemp.xy = VSInput.TexCoords1;
    VSOutput.TexCoords0_1.z = dot(lTextureCoordTemp, g_OtherMatrix.TextureMatrix[0][0]);
    VSOutput.TexCoords0_1.w = dot(lTextureCoordTemp, g_OtherMatrix.TextureMatrix[0][1]);

    return VSOutput;
}

