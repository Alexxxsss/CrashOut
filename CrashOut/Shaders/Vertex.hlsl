#include "ShaderData.hlsli"

PixelInput main(VertexInput input)
{
    PixelInput pixel_input;

    // Transform position using the Camera's Constant Buffer matrix
    pixel_input.position = mul(float4(input.position, 1.0f), cModelViewProjection);
    
    // Pass texture coordinates to pixel shader
    pixel_input.tex = input.tex;

    return pixel_input;
}