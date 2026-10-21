// 輪郭ほど明るく、ワールドの高さで走査線が入る半透明のシェーダー
#include "Object3d.hlsli"
#include "Common.hlsli"
#include "View.hlsli"

struct HologramMaterial
{
    float4 color;
    float fresnelPower;
    float scanlineDensity;
    float scanlineStrength;
};

ConstantBuffer<HologramMaterial> gMaterial : register(b0);
ConstantBuffer<View> gView : register(b2);
Texture2D<float4> gTexture : register(t0);

float4 main(VertexShaderOutput input) : SV_TARGET
{
    const float3 toEye = normalize(gView.worldPos - input.worldPos);
    const float fresnel = pow(1.0f - saturate(dot(normalize(input.normal), toEye)), gMaterial.fresnelPower);
    const float scanline = 0.5f + 0.5f * sin(input.worldPos.y * gMaterial.scanlineDensity);
    const float4 texel = gTexture.Sample(gLinearWrap, input.texcoord);

    const float alpha = saturate(fresnel + scanline * gMaterial.scanlineStrength) * gMaterial.color.a;
    return float4(gMaterial.color.rgb * texel.rgb * (0.5f + fresnel), alpha);
}
