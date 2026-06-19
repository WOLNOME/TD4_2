#include "ParticleCommon.hlsli"

//粒の配列
RWStructuredBuffer<Grain> gGrains : register(u0);
//フリーリストのインデックス
RWStructuredBuffer<int> gFreeListIndex : register(u1);
//フリーリスト
RWStructuredBuffer<uint> gFreeList : register(u2);
//粒のIndex配列情報
RWStructuredBuffer<int> gGrainIndices : register(u3);
//エミッターの範囲情報
RWStructuredBuffer<EmitterRange> gEmitterRange : register(u4);

//エミッターの配列
StructuredBuffer<EmitterInfo> gEmitterInfo : register(t0);
//JSON情報の配列
StructuredBuffer<JsonInfo> gJsonInfo : register(t1);

//総合情報
ConstantBuffer<GeneralInfo> gGeneralInfo : register(b0);

[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint grainIndex = DTid.x;
    
    //粒のIndex配列情報を並列初期化
    gGrainIndices[grainIndex] = -1;
    //エミッター数分の処理(grainIndex→emitterIndexとして扱う)
    if (grainIndex < gGeneralInfo.maxEmitters)
    {
        //生存カウントを0に
        gEmitterRange[grainIndex].aliveCount = 0;
        //スタートはJsonInfoを参照して決める
        gEmitterRange[grainIndex].start = 0;
        for (int i = 0; i < grainIndex; i++)
        {
            //エミッター番号0~iまでのmaxGrainsの合計値とする
            gEmitterRange[grainIndex].start += gJsonInfo[i].maxGrains;
        }
    }
    
    //稼働する必要のないスレッドでは計算処理を省く
    if (grainIndex >= gGeneralInfo.maxGrains)
        return;
    // 寿命が0の粒子は死亡済みと見なして即スキップ
    if (gGrains[grainIndex].lifeTime == 0)
        return;
    
    //Outputから粒の情報を受け取る
    Grain grain = gGrains[grainIndex];
    
    //現在時間の更新
    grain.currentTime += gGeneralInfo.deltaTime;
    
    ///==================///
    /// 粒の削除処理
    ///==================///
    
    //粒に対応するエミッターが既に削除されている場合
    if (!gEmitterInfo[grain.emitterID].isAlive)
    {
        //全データに0を入れる
        gGrains[grainIndex] = (Grain) 0;
        gGrains[grainIndex].emitterID = -1;
        int freeListIndex;
        InterlockedAdd(gFreeListIndex[0], 1, freeListIndex);
        //最新のFreeListIndexの場所に死亡済みGrainのIndexを設定する。
        if ((freeListIndex + 1) < gGeneralInfo.maxGrains)
        {
            gFreeList[freeListIndex + 1] = grainIndex;
            return;
        }
    }
    
    //寿命を迎えたら
    if (grain.currentTime > grain.lifeTime)
    {
        //全データに0を入れる
        gGrains[grainIndex] = (Grain) 0;
        gGrains[grainIndex].emitterID = -1;
        int freeListIndex;
        InterlockedAdd(gFreeListIndex[0], 1, freeListIndex);
        //最新のFreeListIndexの場所に死亡済みGrainのIndexを設定する。
        if ((freeListIndex + 1) < gGeneralInfo.maxGrains)
        {
            gFreeList[freeListIndex + 1] = grainIndex;
            return;
        }
    }
    
    ///==================///
    /// 粒情報の処理
    ///==================///
    
    //重力処理
    if (gJsonInfo[grain.emitterID].isGravity)
        grain.velocity.y += gJsonInfo[grain.emitterID].gravity * gGeneralInfo.deltaTime;
    //バウンド処理
    if (gJsonInfo[grain.emitterID].isBound)
    {
        //粒の最底辺位置の計算
        float leg = grain.transform.translate.y - (grain.transform.scale.y + gGeneralInfo.deltaTime * grain.sizeVelocity);
        //床の反発処理
        if (leg > gJsonInfo[grain.emitterID].floorHeight && leg + (gGeneralInfo.deltaTime * grain.velocity.y) < gJsonInfo[grain.emitterID].floorHeight)
            grain.velocity.y *= (-1.0f) * gJsonInfo[grain.emitterID].repulsion;
    }
    //速度加算
    float4 currentVelocity = gGeneralInfo.deltaTime * grain.velocity;
    grain.transform.translate += currentVelocity;
    //回転更新
    float4 currentRotate = gGeneralInfo.deltaTime * grain.angularVelocity;
    grain.transform.rotate += currentRotate;
    //サイズ更新(初期値からrateを割り出す)
    float t = saturate(grain.currentTime / grain.lifeTime); // 0〜1
    float rate = 1.0f + grain.sizeVelocity * t;
    rate = max(rate, 0.0f);
    grain.transform.scale = grain.initialTransform.scale * rate;
    
    //更新後の粒データを書き込む
    gGrains[grainIndex] = grain;
}