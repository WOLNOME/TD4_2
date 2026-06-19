#include "Trail.hlsli"

struct CameraInfo
{
    float4x4 matWorld;
    float4x4 matView;
    float4x4 matProjection;
};

ConstantBuffer<CameraInfo> gCameraInfo : register(b0);

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = mul(input.position, mul(gCameraInfo.matView, gCameraInfo.matProjection));
    output.texcoord = input.texcoord;
    return output;
}