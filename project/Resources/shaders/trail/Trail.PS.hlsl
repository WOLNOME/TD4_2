#include "Trail.hlsli"

struct PixelShaderOutput
{
    float4 color : SV_Target0;
};

//テクスチャ
Texture2D<float4> gTexture : register(t0);

//通常サンプラー
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    //テクスチャカラーの設定
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    
    output.color = float4(0.6f, 0.6f, 0.0f, 1.0f);
    
    return output;
}