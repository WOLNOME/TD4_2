#include "ParticleCommon.hlsli"
#include "../util/ColorPackUtility.hlsli"

struct CameraInfo
{
    float4x4 matWorld;
    float4x4 matView;
    float4x4 matProjection;
};

// 粒の配列
StructuredBuffer<Grain> gGrains : register(t0);
// エミッター情報
StructuredBuffer<EmitterInfo> gEmitterInfo : register(t1);
// JSON情報
StructuredBuffer<JsonInfo> gJsonInfo : register(t2);
// 粒のIndex配列情報
StructuredBuffer<int> gGrainIndices : register(t3);
// エミッターの範囲情報
StructuredBuffer<EmitterRange> gEmitterRange : register(t4);

// カメラ情報
ConstantBuffer<CameraInfo> gCameraInfo : register(b0);
// 総合情報
ConstantBuffer<GeneralInfo> gGeneralInfo : register(b1);
// 対象のエミッターID
ConstantBuffer<TargetEmitterID> gTargetEmitterID : register(b2);

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

float4x4 MakeIdentity4x4()
{
    return float4x4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );
}

float4x4 MakeScaleMatrix(float3 scale)
{
    return float4x4(
        scale.x, 0, 0, 0,
        0, scale.y, 0, 0,
        0, 0, scale.z, 0,
        0, 0, 0, 1
    );
}

float4x4 MakeRotateXMatrix(float radian)
{
    float c = cos(radian);
    float s = sin(radian);
    return float4x4(
        1, 0, 0, 0,
        0, c, s, 0,
        0, -s, c, 0,
        0, 0, 0, 1
    );
}

float4x4 MakeRotateYMatrix(float radian)
{
    float c = cos(radian);
    float s = sin(radian);
    return float4x4(
         c, 0, -s, 0,
         0, 1, 0, 0,
         s, 0, c, 0,
         0, 0, 0, 1
    );
}

float4x4 MakeRotateZMatrix(float radian)
{
    float c = cos(radian);
    float s = sin(radian);

    return float4x4(
        c, s, 0, 0,
       -s, c, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );
}

float4x4 MakeTranslateMatrix(float3 translate)
{
    return float4x4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        translate.x, translate.y, translate.z, 1
    );
}

float4x4 MakeAffineMatrix(float3 scale, float3 rotate, float3 translate)
{
    float4x4 rx = MakeRotateXMatrix(rotate.x);
    float4x4 ry = MakeRotateYMatrix(rotate.y);
    float4x4 rz = MakeRotateZMatrix(rotate.z);
    
    float4x4 matRotate = mul(rx, mul(ry, rz));
    
    float4x4 matScale = MakeScaleMatrix(scale);
    float4x4 matRS = mul(matScale, matRotate);
    
    float4x4 matTranslate = MakeTranslateMatrix(translate);
    float4x4 affine = mul(matRS, matTranslate);

    return affine;
}

VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID)
{
    VertexShaderOutput output;
    
    //粒のindex配列情報の該当要素番号を計算して出す (start+instanceId)
    uint eleNum = gEmitterRange[gTargetEmitterID.id].start + instanceId;
    //求めた該当要素番号から粒配列のインデックスを求める
    int index = gGrainIndices[eleNum];
    //indexが-1（→粒が存在しない）場合は透明にしてPSでdiscardしてもらう
    if (index == -1)
    {
        output = (VertexShaderOutput) 0;
        return output;
    }
    //インデックスを使って粒配列にアクセスする
    Grain grain = gGrains[index];
    
    //WorldMatrixを求める
    float4x4 worldMatrix = MakeIdentity4x4();
    //billboardの計算をする
    if (gJsonInfo[grain.emitterID].isBillboard)
    {
        float4x4 backToFrontMatrix = MakeRotateZMatrix(grain.transform.rotate.z);
        float4x4 billboardMatrix = mul(backToFrontMatrix, gCameraInfo.matWorld);
        billboardMatrix[3][0] = 0.0f;
        billboardMatrix[3][1] = 0.0f;
        billboardMatrix[3][2] = 0.0f;
        worldMatrix = mul(mul(MakeScaleMatrix(grain.transform.scale.xyz), billboardMatrix), MakeTranslateMatrix(grain.transform.translate.xyz));
    }
    //billboardの計算をしない
    else
    {
        worldMatrix = MakeAffineMatrix(grain.transform.scale.xyz, grain.transform.rotate.xyz, grain.transform.translate.xyz);
    }
    output.position = mul(input.position, mul(mul(worldMatrix, gCameraInfo.matView), gCameraInfo.matProjection));
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) worldMatrix));
    output.color = lerp(PackColor(grain.startColor), PackColor(grain.endColor), grain.currentTime * rcp(grain.lifeTime));
    return output;
}