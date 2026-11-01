cbuffer ConstantBuffer : register(b0)
{
    matrix finalMatrix;
};

struct VS_INPUT
{
    float3 position : POSITION;
    float3 color : COLOR;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 color : COLOR;
};

PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output;
    // Transform 3D coordinate with MVP matrix
    output.position = mul(float4(input.position, 1.0f), finalMatrix);
    output.color = input.color;
    return output;
}

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    return float4(input.color, 1.0f);
}