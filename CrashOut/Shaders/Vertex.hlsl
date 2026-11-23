#include "ShaderData.hlsli"

PixelInput main(VertexInput input)
{
    PixelInput output;
    output.position = mul(float4(input.position, 1.0f), cModelViewProjection);
    output.tex = input.tex;
    output.worldPosition = mul(float4(input.position, 1.0f), cWorld).xyz;
    output.worldNormal = mul(float4(input.normal, 0.0f), cWorldInverseTranspose).xyz;
    return output;
}
