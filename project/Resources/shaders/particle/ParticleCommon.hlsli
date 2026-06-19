
struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
};

struct Transform
{
    float4 scale;
    float4 rotate;
    float4 translate;
};

struct EmitterInfo
{
    Transform worldTransform;
    uint isAffectedField;
    uint isPlay;
    uint isAlive;
};

struct JsonInfo
{
    Transform localTransform;
    float4 velocityMax;
    float4 velocityMin;
    float4 initRotateMax;
    float4 initRotateMin;
    float4 initScaleMax;
    float4 initScaleMin;
    float4 startColorMax;
    float4 startColorMin;
    float4 endColorMax;
    float4 endColorMin;
    float4 angularVelocityMax;
    float4 angularVelocityMin;
    float sizeVelocityMax;
    float sizeVelocityMin;
    float lifeTimeMax;
    float lifeTimeMin;
    float gravity;
    float repulsion;
    float floorHeight;
    int emitRate;
    int maxGrains;
    int generateMethod;
    int clumpNum;
    int effectStyle;
    int isGravity;
    int isBound;
    int isBillboard;
};

struct Grain
{
    Transform initialTransform;
    Transform transform;
    float4 velocity;
    float4 angularVelocity;
    float sizeVelocity;
    float lifeTime;
    float currentTime;
    uint startColor;
    uint endColor;
    int emitterID;
};

struct GeneralInfo
{
    float time;
    float deltaTime;
    uint maxGrains;
    uint maxEmitters;
};

struct TargetEmitterID
{
    uint id;
};

struct EmitterRange
{
    int start;
    uint aliveCount;
};