// Object constants: vertex shader b0.
cbuffer ObjectBuffer : register(b0)
{
    matrix cModelViewProjection;
    matrix cWorld;
    matrix cWorldInverseTranspose;
};

struct VertexInput
{
    float3 position : POSITION;
    float2 tex : TEXTURE;
    float3 normal : NORMAL;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 tex : TEXTURE;
    float3 worldPosition : TEXCOORD0;
    float3 worldNormal : TEXCOORD1;
};

SamplerState gTextureSampler : register(s0);
Texture2D gTextureDiffuse : register(t0);

// Lighting constants: pixel shader b1. Must match LightingSystem.cpp exactly.
struct DirectionalLightData
{
    float4 direction;
    float4 colorIntensity;
};
struct PointLightData
{
    float4 positionRange;
    float4 colorIntensity;
};
struct SpotLightData
{
    float4 positionRange;
    float4 directionInner;
    float4 colorOuter;
    float4 intensityPad;
};

cbuffer LightBuffer : register(b1)
{
    float4 cAmbient;
    uint cDirectionalCount;
    uint cPointCount;
    uint cSpotCount;
    uint cPadding;
    DirectionalLightData cDirectionalLights[1];
    PointLightData cPointLights[8];
    SpotLightData cSpotLights[4];
};


// Directional shadow map. PS slot b2, texture t1, comparison sampler s1.
cbuffer ShadowBuffer : register(b2)
{
    matrix cShadowViewProjection;
    float4 cShadowSettings; // x: shadows enabled, y: depth comparison bias
};
Texture2D<float> gShadowMap : register(t1);
SamplerComparisonState gShadowSampler : register(s1);
