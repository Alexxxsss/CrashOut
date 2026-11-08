#include "RasterState.h"
#include "Renderer.h"
#include <d3d11.h>

RasterState::RasterState(Renderer* renderer)
    : myRenderer(renderer)
{
    // Initialize both rasterizer states upon creation
    CreateRasterStateSolid();
    CreateRasterStateWireframe();
}

void RasterState::ToggleWireframe()
{
    myRenderWireframe = !myRenderWireframe;
}

void RasterState::Use()
{
    ID3D11DeviceContext* context = myRenderer->GetContext();

    if (myRenderWireframe)
    {
        context->RSSetState(myRasterStateWireframe.Get());
    }
    else
    {
        context->RSSetState(myRasterStateSolid.Get());
    }
}

void RasterState::CreateRasterStateSolid()
{
    D3D11_RASTERIZER_DESC rasterizerState = {};
    rasterizerState.FillMode = D3D11_FILL_SOLID;
    rasterizerState.CullMode = D3D11_CULL_BACK;         // Cull backfaces in solid mode
    rasterizerState.FrontCounterClockwise = true;       // Match winding order with wireframe
    rasterizerState.DepthClipEnable = true;
    rasterizerState.MultisampleEnable = true;
    rasterizerState.AntialiasedLineEnable = false;

    rasterizerState.DepthBias = 0;
    rasterizerState.DepthBiasClamp = 0.0f;
    rasterizerState.SlopeScaledDepthBias = 0.0f;

    ID3D11Device* device = myRenderer->GetDevice();
    DX::Check(device->CreateRasterizerState(&rasterizerState, myRasterStateSolid.ReleaseAndGetAddressOf()));
}

void RasterState::CreateRasterStateWireframe()
{
    D3D11_RASTERIZER_DESC rasterizerState = {};
    rasterizerState.FillMode = D3D11_FILL_WIREFRAME;
    rasterizerState.CullMode = D3D11_CULL_NONE;         // Render both front and back edges
    rasterizerState.FrontCounterClockwise = true;
    rasterizerState.DepthClipEnable = true;
    rasterizerState.MultisampleEnable = true;
    rasterizerState.AntialiasedLineEnable = false;

    rasterizerState.DepthBias = 0;
    rasterizerState.DepthBiasClamp = 0.0f;
    rasterizerState.SlopeScaledDepthBias = 0.0f;

    ID3D11Device* device = myRenderer->GetDevice();
    DX::Check(device->CreateRasterizerState(&rasterizerState, myRasterStateWireframe.ReleaseAndGetAddressOf()));
}