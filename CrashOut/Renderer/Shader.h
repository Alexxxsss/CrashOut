#pragma once

#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <filesystem>

class Renderer;

// Three 4x4 matrices = 192 bytes; matches ObjectBuffer in ShaderData.hlsli.
struct ObjectConstantBuffer
{
    DirectX::XMMATRIX modelViewProjection;
    DirectX::XMMATRIX world;
    DirectX::XMMATRIX worldInverseTranspose;
};

class Shader
{
public:
    Shader() = default;
    virtual ~Shader();

    void Init(Renderer* renderer);
    void LoadVertexShader(const std::filesystem::path& path);
    void LoadPixelShader(const std::filesystem::path& path);
    void CreateObjectConstantBuffer();
    void UpdateObjectBuffer(DirectX::XMMATRIX world,
        DirectX::XMMATRIX modelViewProjection);
    void Use();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

private:
    Renderer* myRenderer = nullptr;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> myVertexShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> myVertexLayout;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> myPixelShader;
    Microsoft::WRL::ComPtr<ID3D11Buffer> myObjectConstantBuffer;
};
