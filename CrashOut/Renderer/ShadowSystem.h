#pragma once

#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl/client.h>

class Renderer;
class Registry;

// First shadow-mapping pass: one enabled directional light (the sun).
// The regular LightingSystem still owns color/intensity. This system only
// creates a depth map from the sun and exposes it to Pixel.hlsl.
class ShadowSystem
{
public:
    void Init(Renderer& renderer, UINT resolution = 2048);
    void RenderDepth(Registry& registry);
    void BindForLighting();

    void SetEnabled(bool enabled) { myEnabled = enabled; }
    void SetFocus(const DirectX::XMFLOAT3& focus) { myFocus = focus; }
    void SetCoverage(float width, float height);
    void SetBias(float bias);

private:
    struct ShadowObjectConstants
    {
        DirectX::XMMATRIX worldLightViewProjection;
    };

    struct ShadowPixelConstants
    {
        DirectX::XMMATRIX lightViewProjection;
        DirectX::XMFLOAT4 settings; // x: active, y: pixel depth bias
    };

    Renderer* myRenderer = nullptr;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> myDepthTexture;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> myDepthView;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> myShaderResource;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> myDepthVS;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> myInputLayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer> myDepthObjectBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> myShadowPixelBuffer;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> myComparisonSampler;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> myShadowRasterizer;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> myShadowDepthState;

    D3D11_VIEWPORT myViewport = {};
    DirectX::XMMATRIX myLightViewProjection = DirectX::XMMatrixIdentity();
    DirectX::XMFLOAT3 myFocus = { 0.0f, 0.0f, 0.0f };
    float myCoverageWidth = 90.0f;
    float myCoverageHeight = 90.0f;
    float myBias = 0.0008f;
    bool myEnabled = true;
    bool myHasShadow = false;
};
