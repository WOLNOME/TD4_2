#include "CopyImage.hlsli"

struct Intensity
{
    float intensity;
};

Texture2D<float4> gBloomTexture : register(t0);
Texture2D<float4> gSceneTexture : register(t1);
SamplerState gSampler : register(s0);
ConstantBuffer<Intensity> gIntensity : register(b0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    float3 bloom = gBloomTexture.Sample(gSampler, input.texcoord).rgb;
    float3 scene = gSceneTexture.Sample(gSampler, input.texcoord).rgb;

    output.color = float4(scene + bloom * gIntensity.intensity, 1.0f);

    return output;
}