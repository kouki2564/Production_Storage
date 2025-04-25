struct PS_INPUT
{
    float4 Diffuse : COLOR0; // ディフューズカラー
    float4 Specular : COLOR1; // スペキュラカラー
    float4 TexCoords0_1 : TEXCOORD0; // xy:テクスチャ座標 zw:サブテクスチャ座標
    float3 VPosition : TEXCOORD1; // 座標( ビュー空間 )
    float3 VNormal : TEXCOORD2; // 法線( ビュー空間 )

    float1 Fog : TEXCOORD5; // フォグパラメータ( x )

    float4 Position : SV_POSITION; // 座標( プロジェクション空間 )
};

SamplerState smp : register(s0); // ディフューズマップテクスチャ
Texture2D tex : register(t0); // ディフューズマップテクスチャ

cbuffer ConstantBuffer : register(b0)
{
    //ライトの向きを設定
    float3 LightDir;
}

cbuffer ConstantBuffer : register(b1)
{
    //カメラの向いている方向を設定
    float3 CameraTarget;
}

cbuffer ConstantBuffer : register(b2)
{
    //カメラの位置を設定
    float3 CameraPos;
}

struct PS_OUTPUT
{
    float4 col : SV_TARGET0;
    float4 depth : SV_TARGET1;
    float4 norm : SV_TARGET2;
};

PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT ret;
    ret.depth = input.Position.zwyx;
    ret.col = input.Diffuse * tex.Sample(smp, input.TexCoords0_1.xy);
    ret.norm = float4(input.VNormal, 1.0f);
    
     //ライトの向きを設定
    float3 lightDir = LightDir;
    lightDir = normalize(lightDir);

    //法線情報を正規化
    float3 n = normalize(input.VNormal);

    //光の強さをライト方向と法線方向の内積で求める
    float bright = dot(lightDir, n);
    
    //光の強さを段階に分ける
    bright = round(bright * 4) / 4;
    
    //下限値を設定
    bright = max(bright, 0.4f);

    //もとの色に光の強さをかける
    ret.col.rgb *= bright;
    
    //カメラの向いている方向を求める
    float3 cameraDir = normalize(CameraTarget - CameraPos);
    
    //法線の向きとカメラの向きでリムライトの強さを求める
    float rim = dot(n, cameraDir);
    
    //リムライトの強さが-0.25~0.25の間の場合
    if (rim > -0.25f && rim < 0.25f)
    {
        ret.col.rbg = 0.05f;
    }
    
    return ret;
}