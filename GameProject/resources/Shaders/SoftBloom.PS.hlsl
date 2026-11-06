// 明るい所を縮小して抜き出し、横・縦にぼかして元の画像へ足す。SoftBloom.shader.json の Passes が Entry ごとに 1 パスとして描く
#include "FullScreen.hlsli"
#include "Common.hlsli"

// 全パスで同じ並びにする（MaterialLibrary が 1 つの値の組を全パスへ渡す）
struct SoftBloomParam
{
    float threshold;  // これより明るい所だけ光らせる
    float intensity;
    float radius;     // ぼかしの 1 歩の画素数（入力の画素で数える）
    float padding;
};

ConstantBuffer<SoftBloomParam> gParam : register(b0);
Texture2D<float4> gTexture : register(t0);
Texture2D<float4> gBloom : register(t1);

// 9 タップのガウス分布（中心と片側 4 つ）
static const float kWeights[5] = { 0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f };

float4 Blur(float2 uv, float2 direction)
{
    float2 size;
    gTexture.GetDimensions(size.x, size.y);
    const float2 stepUv = direction * gParam.radius / size;

    float4 sum = gTexture.Sample(gLinearClamp, uv) * kWeights[0];
    [unroll]
    for (int i = 1; i < 5; ++i)
    {
        sum += gTexture.Sample(gLinearClamp, uv + stepUv * i) * kWeights[i];
        sum += gTexture.Sample(gLinearClamp, uv - stepUv * i) * kWeights[i];
    }
    return sum;
}

float4 Extract(VertexShaderOutput input) : SV_TARGET
{
    const float4 color = gTexture.Sample(gLinearClamp, input.texCoord);
    const float brightness = max(color.r, max(color.g, color.b));
    // しきい値を超えた分だけ残すので、境目で光が急に途切れない
    const float contribution = max(brightness - gParam.threshold, 0.0f) / max(brightness, 1e-4f);
    return float4(color.rgb * contribution, 0.0f);
}

float4 BlurX(VertexShaderOutput input) : SV_TARGET
{
    return Blur(input.texCoord, float2(1.0f, 0.0f));
}

float4 BlurY(VertexShaderOutput input) : SV_TARGET
{
    return Blur(input.texCoord, float2(0.0f, 1.0f));
}

float4 Combine(VertexShaderOutput input) : SV_TARGET
{
    const float4 base = gTexture.Sample(gLinearClamp, input.texCoord);
    const float3 bloom = gBloom.Sample(gLinearClamp, input.texCoord).rgb;
    // α は入力のまま。描画物の外では α=0 で光だけ残り、シーンへ重ねるときに加算になる
    return float4(base.rgb + bloom * gParam.intensity, base.a);
}
