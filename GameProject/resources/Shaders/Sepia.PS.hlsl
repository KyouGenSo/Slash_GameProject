// 輝度に色を掛けて古い写真風にする
#include "FullScreen.hlsli"
#include "Common.hlsli"

struct SepiaParam
{
    float3 toneColor;
    float strength;
};

ConstantBuffer<SepiaParam> gParam : register(b0);
Texture2D<float4> gTexture : register(t0);

float4 main(VertexShaderOutput input) : SV_TARGET
{
    const float4 color = gTexture.Sample(gLinearClamp, input.texCoord);
    const float luminance = dot(color.rgb, float3(0.299f, 0.587f, 0.114f));
    return float4(lerp(color.rgb, luminance * gParam.toneColor, gParam.strength), color.a);
}
