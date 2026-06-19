#include "Skybox.hlsli"

struct WorldTransformationMatrix
{
    float4x4 World; //ワールド行列
    float4x4 WorldInverseTranspose; //ワールド逆転置行列
};
struct CameraInfo
{
    float4x4 matWorld;
    float4x4 matView;
    float4x4 matProjection;
};

//ワールドトランスフォーム情報
StructuredBuffer<WorldTransformationMatrix> gWorldTransformationMatrix : register(t0);

//カメラ情報
ConstantBuffer<CameraInfo> gCameraInfo : register(b0);

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID)
{
    VertexShaderOutput output;
    float4x4 wvpMatrix = mul(mul(gWorldTransformationMatrix[instanceId].World, gCameraInfo.matView), gCameraInfo.matProjection);
    output.position = mul(input.position, wvpMatrix).xyww;
    output.texcoord = input.position.xyz;
    return output;
}
