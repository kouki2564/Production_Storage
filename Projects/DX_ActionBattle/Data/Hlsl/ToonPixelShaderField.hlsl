struct PS_INPUT
{
    float4 Diffuse : COLOR0;        //ディフューズカラー
    float4 Specular : COLOR1;       //スペキュラカラー
    float4 TexCoords0_1 : TEXCOORD0;//xy:テクスチャ座標 zw:サブテクスチャ座標
    float3 VPosition : TEXCOORD1;   //座標( ビュー空間 )
    float3 VNormal : TEXCOORD2;     //法線( ビュー空間 )

    float1 Fog : TEXCOORD5;         //フォグパラメータ( x )

    float4 Position : SV_POSITION;  //座標( プロジェクション空間 )
    //ライトビュースクリーン空間での座標を追加
    float4 posInLVP : TEXCOORD3;    //ライトビュースクリーン空間でのピクセルの座標
};

SamplerState smp : register(s0);    //ディフューズマップテクスチャ
Texture2D tex : register(t0);       //ディフューズマップテクスチャ

Texture2D shadowMapSmp : register(t1); //シャドウマップテクスチャ

//定数バッファ(ライトの方向)
cbuffer ConstantBuffer : register(b0)
{
    float3 LightDir;
}

//定数バッファ(カメラのターゲット)
cbuffer ConstantBuffer : register(b1)
{
    float3 CameraTarget;
}

//定数バッファ(カメラの位置)
cbuffer ConstantBuffer : register(b2)
{
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

    //光の強さをライト方向と法線方向の内積で求める(フィールドには少し強めに光を当てる)
    float bright = dot(lightDir, n) * 1.5f;
     
    //光の強さを段階に分ける
    bright = round(bright * 4) / 4;
    
    //下限値を設定
    bright = max(bright, 0.6f);
    
    //上限値を設定
    bright = min(bright, 1.0f);
    
    //影の処理
    float2 uv = input.posInLVP.xy / input.posInLVP.w;
    uv = uv * float2(0.5f, 0.5f);
    uv += 0.5f;
    
    //影が反転を防ぐ処理
    uv.y *= -1;
    uv.y += 1.0f;
    
    //影の処理
    float4 shadowMap = float4(0.0f, 0.0f, 0.0f,1.0f);
    
    //影のサンプリングを行う
    shadowMap = shadowMapSmp.Sample(smp, uv);
    
    //影情報を反映させた色を返す
    ret.col.rgb = ret.col.rgb * shadowMap.rgb * bright;
    
    return ret;
}