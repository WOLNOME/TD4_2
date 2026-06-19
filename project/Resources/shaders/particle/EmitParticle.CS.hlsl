#include "ParticleCommon.hlsli"
#include "../util/RandomUtility.hlsli"
#include "../util/ColorPackUtility.hlsli"

//全粒の配列
RWStructuredBuffer<Grain> gGrains : register(u0);
//フリーリストのインデックス
RWStructuredBuffer<int> gFreeListIndex : register(u1);
//フリーリスト
RWStructuredBuffer<uint> gFreeList : register(u2);
//エミッターの範囲情報
RWStructuredBuffer<EmitterRange> gEmitterRange : register(u3);

//エミッター情報
StructuredBuffer<EmitterInfo> gEmitterInfo : register(t0);
//JSON情報
StructuredBuffer<JsonInfo> gJsonInfo : register(t1);

//総合情報
ConstantBuffer<GeneralInfo> gGeneralInfo : register(b0);

//乱数ジェネレーター
class RandomGenerator
{
    float3 seed;

    float3 Generate3d()
    {
        seed = rand3dTo3d(seed);
        return seed;
    }
    float Generate1d()
    {
        float result = rand3dTo1d(seed);
        seed.x = result;
        return result;
    }
    float GenerateInRange(float min, float max)
    {
        return lerp(min, max, Generate1d());
    }
    float3 GenerateInRange(float3 min, float3 max)
    {
        return lerp(min, max, Generate3d());
    }
};

//粒生成関数 (前方宣言)
void GenerateGrain(uint emitterIndex, int generateNum, RandomGenerator generator);

[numthreads(1, 1, 1)] //1エミッターにつき1スレッド
void main(uint3 DTid : SV_DispatchThreadID)
{
    //エミッターのインデックス取得
    uint emitterIndex = DTid.x;
    //エミッターが生きていなければ処理を行わない
    if(!gEmitterInfo[emitterIndex].isAlive)
        return;
    //エミッターが稼働していなければ処理を行わない
    if (!gEmitterInfo[emitterIndex].isPlay)
        return;
    
    //生成に必要なローカル変数
    int max = gJsonInfo[emitterIndex].maxGrains;
    int rate = gJsonInfo[emitterIndex].emitRate;
    float ratePerFrame = rate * gGeneralInfo.deltaTime;
    int generateNum = 0; //このフレームで生成する数
    
    
    //乱数生成機の作成
    RandomGenerator generator;
    //乱数のシードを更新
    generator.seed = frac(float3(emitterIndex * 0.1234f, emitterIndex * 0.5678f, emitterIndex * 0.9101f) + gGeneralInfo.time * 0.1f) * 1000.0f;
    
    //ループ処理
    if (gJsonInfo[emitterIndex].effectStyle == 0)      
    {
        //ratePerFrameの整数部分を生成数に設定
        generateNum = floor(ratePerFrame);
        //ratePerFrameの小数部分を確率として計算
        if (generator.GenerateInRange(0.0f, 1.0f) < frac(ratePerFrame))
        {
            generateNum++;
        }
    }
    //一度きり処理(OneShot)
    else if (gJsonInfo[emitterIndex].effectStyle == 1)     
    {
        //生成数は1つだけ
        generateNum = 1;
    }
    //粒の生成
    if (generateNum > 0)
    {
        //生成する粒の数をクランプ
        uint aliveGrainNum = gEmitterRange[emitterIndex].aliveCount;
        if (generateNum + aliveGrainNum > max)
        {
            generateNum = max - aliveGrainNum;
        }
        
        GenerateGrain(emitterIndex, generateNum, generator);
    }
}

void GenerateGrain(uint emitterIndex, int generateNum, RandomGenerator generator)
{
    for (int i = 0; i < generateNum; i++)
    {
        //乱数のシードを更新
        generator.seed = frac(float3(i * 0.1234f, i * 0.5678f, i * 0.9101f) + gGeneralInfo.time * 0.1f) * 1000.0f;
        
        int freeListIndex;
        
        //生成方法ごとに処理を分ける
        if (gJsonInfo[emitterIndex].generateMethod == 0)       //ランダム生成
        {
            //FreeListのIndexを1つ前に設定し、現在のIndexを取得する
            InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);
            if ((0 <= freeListIndex) && (freeListIndex < gGeneralInfo.maxGrains))
            {
                uint grainIndex = gFreeList[freeListIndex];
                //値を入れていく
                gGrains[grainIndex].transform.translate.x = generator.GenerateInRange(gEmitterInfo[emitterIndex].worldTransform.translate.x - gEmitterInfo[emitterIndex].worldTransform.scale.x, gEmitterInfo[emitterIndex].worldTransform.translate.x + gEmitterInfo[emitterIndex].worldTransform.scale.x);
                gGrains[grainIndex].transform.translate.y = generator.GenerateInRange(gEmitterInfo[emitterIndex].worldTransform.translate.y - gEmitterInfo[emitterIndex].worldTransform.scale.y, gEmitterInfo[emitterIndex].worldTransform.translate.y + gEmitterInfo[emitterIndex].worldTransform.scale.y);
                gGrains[grainIndex].transform.translate.z = generator.GenerateInRange(gEmitterInfo[emitterIndex].worldTransform.translate.z - gEmitterInfo[emitterIndex].worldTransform.scale.z, gEmitterInfo[emitterIndex].worldTransform.translate.z + gEmitterInfo[emitterIndex].worldTransform.scale.z);
                gGrains[grainIndex].transform.rotate.x = generator.GenerateInRange(gJsonInfo[emitterIndex].initRotateMin.x, gJsonInfo[emitterIndex].initRotateMax.x);
                gGrains[grainIndex].transform.rotate.y = generator.GenerateInRange(gJsonInfo[emitterIndex].initRotateMin.y, gJsonInfo[emitterIndex].initRotateMax.y);
                gGrains[grainIndex].transform.rotate.z = generator.GenerateInRange(gJsonInfo[emitterIndex].initRotateMin.z, gJsonInfo[emitterIndex].initRotateMax.z);
                gGrains[grainIndex].transform.scale.x = generator.GenerateInRange(gJsonInfo[emitterIndex].initScaleMin.x, gJsonInfo[emitterIndex].initScaleMax.x);
                gGrains[grainIndex].transform.scale.y = generator.GenerateInRange(gJsonInfo[emitterIndex].initScaleMin.y, gJsonInfo[emitterIndex].initScaleMax.y);
                gGrains[grainIndex].transform.scale.z = generator.GenerateInRange(gJsonInfo[emitterIndex].initScaleMin.z, gJsonInfo[emitterIndex].initScaleMax.z);
                gGrains[grainIndex].initialTransform = gGrains[grainIndex].transform; //初期値保存
                gGrains[grainIndex].angularVelocity.x = generator.GenerateInRange(gJsonInfo[emitterIndex].angularVelocityMin.x, gJsonInfo[emitterIndex].angularVelocityMax.x);
                gGrains[grainIndex].angularVelocity.y = generator.GenerateInRange(gJsonInfo[emitterIndex].angularVelocityMin.y, gJsonInfo[emitterIndex].angularVelocityMax.y);
                gGrains[grainIndex].angularVelocity.z = generator.GenerateInRange(gJsonInfo[emitterIndex].angularVelocityMin.z, gJsonInfo[emitterIndex].angularVelocityMax.z);
                gGrains[grainIndex].sizeVelocity = generator.GenerateInRange(gJsonInfo[emitterIndex].sizeVelocityMin, gJsonInfo[emitterIndex].sizeVelocityMax);
                float4 startColor = { generator.GenerateInRange(gJsonInfo[emitterIndex].startColorMin.r, gJsonInfo[emitterIndex].startColorMax.r), generator.GenerateInRange(gJsonInfo[emitterIndex].startColorMin.g, gJsonInfo[emitterIndex].startColorMax.g), generator.GenerateInRange(gJsonInfo[emitterIndex].startColorMin.b, gJsonInfo[emitterIndex].startColorMax.b), generator.GenerateInRange(gJsonInfo[emitterIndex].startColorMin.a, gJsonInfo[emitterIndex].startColorMax.a) };
                float4 endColor = { generator.GenerateInRange(gJsonInfo[emitterIndex].endColorMin.r, gJsonInfo[emitterIndex].endColorMax.r), generator.GenerateInRange(gJsonInfo[emitterIndex].endColorMin.g, gJsonInfo[emitterIndex].endColorMax.g), generator.GenerateInRange(gJsonInfo[emitterIndex].endColorMin.b, gJsonInfo[emitterIndex].endColorMax.b), generator.GenerateInRange(gJsonInfo[emitterIndex].endColorMin.a, gJsonInfo[emitterIndex].endColorMax.a) };
                gGrains[grainIndex].startColor = PackColor(startColor);
                gGrains[grainIndex].endColor = PackColor(endColor);
                gGrains[grainIndex].velocity.x = generator.GenerateInRange(gJsonInfo[emitterIndex].velocityMin.x, gJsonInfo[emitterIndex].velocityMax.x);
                gGrains[grainIndex].velocity.y = generator.GenerateInRange(gJsonInfo[emitterIndex].velocityMin.y, gJsonInfo[emitterIndex].velocityMax.y);
                gGrains[grainIndex].velocity.z = generator.GenerateInRange(gJsonInfo[emitterIndex].velocityMin.z, gJsonInfo[emitterIndex].velocityMax.z);
                gGrains[grainIndex].lifeTime = generator.GenerateInRange(gJsonInfo[emitterIndex].lifeTimeMin, gJsonInfo[emitterIndex].lifeTimeMax);
                gGrains[grainIndex].currentTime = 0.0f;
                gGrains[grainIndex].emitterID = emitterIndex;
            }
            else
            {
                //発生させられなかったので減らした分元に戻す。
                InterlockedAdd(gFreeListIndex[0], 1);
                //Emit中にParticleは消えないので、この後発生することはないためreturnして終わらせる
                return;
            }
        }
        else if (gJsonInfo[emitterIndex].generateMethod == 1)      //クランプ生成
        {
            //FreeListのIndexを1つ前に設定し、現在のIndexを取得する
            InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);
            if ((0 <= freeListIndex) && (freeListIndex < gGeneralInfo.maxGrains))
            {
                uint grainIndex = gFreeList[freeListIndex];
            
                //共通の値は先に決めておく
                float4 basicTranslate = (float4) 0.0f;
                basicTranslate.x = generator.GenerateInRange(gEmitterInfo[emitterIndex].worldTransform.translate.x - gEmitterInfo[emitterIndex].worldTransform.scale.x, gEmitterInfo[emitterIndex].worldTransform.translate.x + gEmitterInfo[emitterIndex].worldTransform.scale.x);
                basicTranslate.y = generator.GenerateInRange(gEmitterInfo[emitterIndex].worldTransform.translate.y - gEmitterInfo[emitterIndex].worldTransform.scale.y, gEmitterInfo[emitterIndex].worldTransform.translate.y + gEmitterInfo[emitterIndex].worldTransform.scale.y);
                basicTranslate.z = generator.GenerateInRange(gEmitterInfo[emitterIndex].worldTransform.translate.z - gEmitterInfo[emitterIndex].worldTransform.scale.z, gEmitterInfo[emitterIndex].worldTransform.translate.z + gEmitterInfo[emitterIndex].worldTransform.scale.z);
                float4 velocity = (float4) 0.0f;
                velocity.x = generator.GenerateInRange(gJsonInfo[emitterIndex].velocityMin.x, gJsonInfo[emitterIndex].velocityMax.x);
                velocity.y = generator.GenerateInRange(gJsonInfo[emitterIndex].velocityMin.y, gJsonInfo[emitterIndex].velocityMax.y);
                velocity.z = generator.GenerateInRange(gJsonInfo[emitterIndex].velocityMin.z, gJsonInfo[emitterIndex].velocityMax.z);
                float lifeTime = 0.0f;
                lifeTime = generator.GenerateInRange(gJsonInfo[emitterIndex].lifeTimeMin, gJsonInfo[emitterIndex].lifeTimeMax);
                //クランプ数だけ回す
                for (int j = 0; j < gJsonInfo[emitterIndex].clumpNum; j++)
                {
                    //値を入れていく
                    gGrains[grainIndex].transform.translate = basicTranslate;
                    gGrains[grainIndex].transform.rotate.x = generator.GenerateInRange(gJsonInfo[emitterIndex].initRotateMin.x, gJsonInfo[emitterIndex].initRotateMax.x);
                    gGrains[grainIndex].transform.rotate.y = generator.GenerateInRange(gJsonInfo[emitterIndex].initRotateMin.y, gJsonInfo[emitterIndex].initRotateMax.y);
                    gGrains[grainIndex].transform.rotate.z = generator.GenerateInRange(gJsonInfo[emitterIndex].initRotateMin.z, gJsonInfo[emitterIndex].initRotateMax.z);
                    gGrains[grainIndex].transform.scale.x = generator.GenerateInRange(gJsonInfo[emitterIndex].initScaleMin.x, gJsonInfo[emitterIndex].initScaleMax.x);
                    gGrains[grainIndex].transform.scale.y = generator.GenerateInRange(gJsonInfo[emitterIndex].initScaleMin.y, gJsonInfo[emitterIndex].initScaleMax.y);
                    gGrains[grainIndex].transform.scale.z = generator.GenerateInRange(gJsonInfo[emitterIndex].initScaleMin.z, gJsonInfo[emitterIndex].initScaleMax.z);
                    gGrains[grainIndex].initialTransform = gGrains[grainIndex].transform; //初期値保存
                    gGrains[grainIndex].angularVelocity.x = generator.GenerateInRange(gJsonInfo[emitterIndex].angularVelocityMin.x, gJsonInfo[emitterIndex].angularVelocityMax.x);
                    gGrains[grainIndex].angularVelocity.y = generator.GenerateInRange(gJsonInfo[emitterIndex].angularVelocityMin.y, gJsonInfo[emitterIndex].angularVelocityMax.y);
                    gGrains[grainIndex].angularVelocity.z = generator.GenerateInRange(gJsonInfo[emitterIndex].angularVelocityMin.z, gJsonInfo[emitterIndex].angularVelocityMax.z);
                    gGrains[grainIndex].sizeVelocity = generator.GenerateInRange(gJsonInfo[emitterIndex].sizeVelocityMin, gJsonInfo[emitterIndex].sizeVelocityMax);
                    float4 startColor = { generator.GenerateInRange(gJsonInfo[emitterIndex].startColorMin.r, gJsonInfo[emitterIndex].startColorMax.r), generator.GenerateInRange(gJsonInfo[emitterIndex].startColorMin.g, gJsonInfo[emitterIndex].startColorMax.g), generator.GenerateInRange(gJsonInfo[emitterIndex].startColorMin.b, gJsonInfo[emitterIndex].startColorMax.b), generator.GenerateInRange(gJsonInfo[emitterIndex].startColorMin.a, gJsonInfo[emitterIndex].startColorMax.a) };
                    float4 endColor = { generator.GenerateInRange(gJsonInfo[emitterIndex].endColorMin.r, gJsonInfo[emitterIndex].endColorMax.r), generator.GenerateInRange(gJsonInfo[emitterIndex].endColorMin.g, gJsonInfo[emitterIndex].endColorMax.g), generator.GenerateInRange(gJsonInfo[emitterIndex].endColorMin.b, gJsonInfo[emitterIndex].endColorMax.b), generator.GenerateInRange(gJsonInfo[emitterIndex].endColorMin.a, gJsonInfo[emitterIndex].endColorMax.a) };
                    gGrains[grainIndex].startColor = PackColor(startColor);
                    gGrains[grainIndex].endColor = PackColor(endColor);
                    gGrains[grainIndex].velocity = velocity;
                    gGrains[grainIndex].lifeTime = lifeTime;
                    gGrains[grainIndex].currentTime = 0.0f;
                    gGrains[grainIndex].emitterID = emitterIndex;
                    
                    //最後の周ならここでreturn
                    if (j == gJsonInfo[emitterIndex].clumpNum - 1)
                        return;
                    
                    //粒のインデックスを次の値へ
                    InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);
                    if ((0 <= freeListIndex) && (freeListIndex < gGeneralInfo.maxGrains))
                    {
                        grainIndex = gFreeList[freeListIndex];
                    }
                    //空きがなかったらそのまま返す
                    else
                    {
                        //発生させられなかったので減らした分元に戻す。
                        InterlockedAdd(gFreeListIndex[0], 1);
                        //Emit中にParticleは消えないので、この後発生することはないためreturnして終わらせる
                        return;
                    }
                }
            }
            else
            {
                //発生させられなかったので減らした分元に戻す。
                InterlockedAdd(gFreeListIndex[0], 1);
                //Emit中にParticleは消えないので、この後発生することはないためreturnして終わらせる
                return;
            }
        }
    }
}