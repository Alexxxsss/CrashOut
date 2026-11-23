#include "ShaderData.hlsli"

float SampleSunShadow(float3 worldPosition)
{
    if (cShadowSettings.x < 0.5f)
        return 1.0f;

    float4 lightClip = mul(float4(worldPosition, 1.0f), cShadowViewProjection);
    if (lightClip.w <= 0.0f)
        return 1.0f;

    float3 projected = lightClip.xyz / lightClip.w;
    float2 uv = float2(projected.x * 0.5f + 0.5f,
                       0.5f - projected.y * 0.5f);

    // Outside the shadow camera = treat as lit, not black.
    if (uv.x < 0.0f || uv.x > 1.0f ||
        uv.y < 0.0f || uv.y > 1.0f ||
        projected.z < 0.0f || projected.z > 1.0f)
        return 1.0f;

    // Comparison sampler does the depth test and filters 4 neighbors (PCF).
    return gShadowMap.SampleCmpLevelZero(
        gShadowSampler, uv, projected.z - cShadowSettings.y);
}

float3 CalculateLighting(float3 worldPosition, float3 normal)
{
    float3 total = cAmbient.rgb * cAmbient.a;

    // 1. Directional (sun): same light direction everywhere.
    for (uint i = 0; i < cDirectionalCount; ++i)
    {
        float3 toLight = -normalize(cDirectionalLights[i].direction.xyz);
        float lambert = max(dot(normal, toLight), 0.0f);
        total += cDirectionalLights[i].colorIntensity.rgb *
                 cDirectionalLights[i].colorIntensity.a * lambert *
                 SampleSunShadow(worldPosition);
    }

    // 2. Point (bulb): light spreads from a position and fades with distance.
    for (uint j = 0; j < cPointCount; ++j)
    {
        float3 delta = cPointLights[j].positionRange.xyz - worldPosition;
        float distanceToLight = length(delta);
        float range = cPointLights[j].positionRange.w;
        if (distanceToLight >= range || distanceToLight < 0.0001f)
            continue;

        float3 toLight = delta / distanceToLight;
        float lambert = max(dot(normal, toLight), 0.0f);
        float falloff = saturate(1.0f - distanceToLight / range);
        falloff *= falloff;
        total += cPointLights[j].colorIntensity.rgb *
                 cPointLights[j].colorIntensity.a * lambert * falloff;
    }

    // 3. Spot (flashlight): point light clipped to a soft cone.
    for (uint k = 0; k < cSpotCount; ++k)
    {
        float3 delta = cSpotLights[k].positionRange.xyz - worldPosition;
        float distanceToLight = length(delta);
        float range = cSpotLights[k].positionRange.w;
        if (distanceToLight >= range || distanceToLight < 0.0001f)
            continue;

        float3 toLight = delta / distanceToLight;
        float lambert = max(dot(normal, toLight), 0.0f);
        float falloff = saturate(1.0f - distanceToLight / range);
        falloff *= falloff;

        // -toLight points from the light toward this surface pixel.
        float cosAngle = dot(-toLight, normalize(cSpotLights[k].directionInner.xyz));
        float innerCos = cSpotLights[k].directionInner.w;
        float outerCos = cSpotLights[k].colorOuter.w;
        float cone = saturate((cosAngle - outerCos) / max(innerCos - outerCos, 0.0001f));
        cone = cone * cone * (3.0f - 2.0f * cone);

        total += cSpotLights[k].colorOuter.rgb *
                 cSpotLights[k].intensityPad.x * lambert * falloff * cone;
    }
    return total;
}

float4 main(PixelInput input) : SV_TARGET
{
    float4 albedo = gTextureDiffuse.Sample(gTextureSampler, input.tex);
    float3 normal = normalize(input.worldNormal);
    float3 lighting = CalculateLighting(input.worldPosition, normal);
    return float4(albedo.rgb * lighting, albedo.a);
}
