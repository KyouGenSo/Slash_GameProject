// 輪郭線のパス用。頂点を法線方向へ押し出して裏面だけを描く（Cull Front）と、本体の外側に縁が残る
#include "Object3d.hlsli"
#include "View.hlsli"

struct TransformationMatrix
{
    float4x4 World;
    float4x4 WorldIT;
};

// Outline.PS.hlsl と同じ並び
struct Material
{
    float4 color;
    float4 outlineColor;
    float outlineWidth;
    float3 padding;
};

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
ConstantBuffer<Material> gMaterial : register(b1);
ConstantBuffer<View> gView : register(b2);

struct VertexShaderInput
{
    float4 pos : POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    const float3 normal = normalize(mul(input.normal, (float3x3) gTransformationMatrix.WorldIT));
    const float4 worldPos = float4(mul(input.pos, gTransformationMatrix.World).xyz + normal * gMaterial.outlineWidth, 1.0f);
    output.pos = mul(worldPos, gView.viewProj);
    output.texcoord = input.texcoord;
    output.normal = normal;
    output.worldPos = worldPos.xyz;
    output.lightSpacePos = float4(0.0f, 0.0f, 0.0f, 1.0f);
    return output;
}
