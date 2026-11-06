// 後ろの画面（不透明まで描いた gOpaqueTexture）を時間でゆらして映す。描画キュー 3000 以上の半透明として描くと渡される
#include "Object3d.hlsli"
#include "Common.hlsli"
#include "View.hlsli"

struct Material
{
    float4 color;    // ゆらした背景に掛ける色（α で濃さ）
    float strength;  // UV をずらす量（画面の幅に対する割合）
    float speed;
    float scale;     // ゆらぎの細かさ（ワールド座標に掛ける）
    float padding;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<View> gView : register(b2);
Texture2D<float4> gOpaqueTexture : register(t0);

float4 main(VertexShaderOutput input) : SV_TARGET
{
    const float2 screenUv = input.pos.xy / gView.screenSize;
    const float t = gView.time * gMaterial.speed;
    const float2 offset = float2(sin(input.worldPos.y * gMaterial.scale + t), cos(input.worldPos.x * gMaterial.scale + t * 1.3f)) * gMaterial.strength;
    const float3 background = gOpaqueTexture.Sample(gLinearClamp, screenUv + offset).rgb;
    return float4(background * gMaterial.color.rgb, gMaterial.color.a);
}
