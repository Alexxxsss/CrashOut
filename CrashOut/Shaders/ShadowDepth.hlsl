// Depth-only vertex shader. The rasterizer writes SV_POSITION.z into
// our shadow map's D32_FLOAT depth buffer; no pixel shader is needed.
cbuffer ShadowObjectBuffer : register(b0)
{
    matrix cWorldLightViewProjection;
};

struct DepthVertexInput
{
    float3 position : POSITION;
};

float4 main(DepthVertexInput input) : SV_POSITION
{
    return mul(float4(input.position, 1.0f), cWorldLightViewProjection);
}
