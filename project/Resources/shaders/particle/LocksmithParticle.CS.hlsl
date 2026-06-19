#include "ParticleCommon.hlsli"

// 粒の配列
RWStructuredBuffer<Grain> gGrains : register(u0);
// 粒のIndex配列情報
RWStructuredBuffer<int> gGrainIndices : register(u1);
// エミッターの範囲情報
RWStructuredBuffer<EmitterRange> gEmitterRange : register(u2);

// エミッター情報
StructuredBuffer<EmitterInfo> gEmitterInfo : register(t0);
// JSON情報の配列
StructuredBuffer<JsonInfo> gJsonInfo : register(t1);

// 総合情報
ConstantBuffer<GeneralInfo> gGeneralInfo : register(b0);

[numthreads(1024, 1, 1)]
//粒配列からエミッターIDで対象の要素番号を取得できるようにするためにするための処理
//Update.CSで既に初期化済み
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint grainIndex = DTid.x;
    uint emitterID = gGrains[grainIndex].emitterID;
    
    //稼働する必要のないスレッドでは計算処理を省く
    if (grainIndex >= gGeneralInfo.maxGrains)
        return;
    //粒インデックスの「エミッターID」== -1ならこの粒は空なのでreturn
    if (emitterID == -1)
        return;
    //稼働していないエミッターの粒の場合return
    if (!gEmitterInfo[emitterID].isAlive)
        return;
    
    //エミッターの範囲情報の生存カウントをインクリメント
    uint countValue;
    InterlockedAdd(gEmitterRange[emitterID].aliveCount, 1, countValue);
    
    //粒のIndex情報に登録
    int start = gEmitterRange[emitterID].start;
    gGrainIndices[start + countValue] = grainIndex;
}