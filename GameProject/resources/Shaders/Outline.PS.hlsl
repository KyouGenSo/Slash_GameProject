// 本体（Body）と輪郭線（Edge）。Outline.shader.json の Forward が Body → Edge の順に同じ描画物を描く
#include "Object3d.hlsli"
#include "Common.hlsli"

// Outline.VS.hlsl と同じ並び。輪郭線のパスでは VS と PS が同じ名前・同じレジスタで宣言して値を共有する
struct Material
{
    float4 color;
    float4 outlineColor;
    float outlineWidth;  // 法線方向に押し出す距離（ワールド単位）
    float3 padding;
};

ConstantBuffer<Material> gMaterial : register(b1);
Texture2D<float4> gTexture : register(t0);

float4 Body(VertexShaderOutput input) : SV_TARGET
{
    // 上からの光と下からの照り返しの半球ライト
    const float sky = dot(normalize(input.normal), float3(0.0f, 1.0f, 0.0f)) * 0.5f + 0.5f;
    const float4 albedo = gTexture.Sample(gLinearWrap, input.texcoord) * gMaterial.color;
    return float4(albedo.rgb * lerp(0.35f, 1.0f, sky), albedo.a);
}

float4 Edge(VertexShaderOutput input) : SV_TARGET
{
    return gMaterial.outlineColor;
}
