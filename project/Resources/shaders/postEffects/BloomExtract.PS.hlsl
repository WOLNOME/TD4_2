#include "CopyImage.hlsli"

struct Threshold
{
    float threshold;
};

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<Threshold> gThreshold : register(b0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};


PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    //元画像の明るさを取得する
    float3 color = gTexture.Sample(gSampler, input.texcoord).rgb;
    float brightness = dot(color, float3(0.2126f, 0.7152f, 0.0722f));
    
    //閾値を超えた明るさのピクセルのみを残す
    float factor = step(gThreshold.threshold, brightness);
    output.color = float4(color * factor, 1.0f);
    
    return output;
}