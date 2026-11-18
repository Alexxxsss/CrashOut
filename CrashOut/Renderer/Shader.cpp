#include "Shader.h"
#include "Renderer.h"
#include "DXError.h"

#include <d3dcompiler.h>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>


namespace
{
    Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
        const std::filesystem::path& aPath,
        const char* aEntryPoint,
        const char* aTarget)
    {
        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;

#ifdef _DEBUG
        flags |= D3DCOMPILE_DEBUG;
        flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

        Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

        const auto fullPath =
            std::filesystem::absolute(aPath);

        HRESULT hr = D3DCompileFromFile(
            fullPath.c_str(),
            nullptr,
            D3D_COMPILE_STANDARD_FILE_INCLUDE,
            aEntryPoint,
            aTarget,
            flags,
            0,
            shaderBlob.GetAddressOf(),
            errorBlob.GetAddressOf()
        );

        std::string diagnostics;

        if (errorBlob)
        {
            diagnostics.assign(
                static_cast<const char*>(
                    errorBlob->GetBufferPointer()
                    ),
                errorBlob->GetBufferSize()
            );

            if (!diagnostics.empty() &&
                diagnostics.back() == '\0')
            {
                diagnostics.pop_back();
            }
        }

        if (FAILED(hr))
        {
            throw std::runtime_error(
                std::string("Shader compilation failed") +
                "\nFile: " + fullPath.u8string() +
                "\nEntry point: " + aEntryPoint +
                "\nTarget: " + aTarget +
                "\nHRESULT: " + DX::FormatHRESULT(hr) +
                "\n" + diagnostics
            );
        }

        if (!diagnostics.empty())
        {
            std::cerr
                << "[Shader Warning]\n"
                << diagnostics
                << std::endl;
        }

        return shaderBlob;
    }
}


Shader::~Shader() = default;

void Shader::Init(Renderer* renderer)
{
    myRenderer = renderer;
}


void Shader::LoadVertexShader(const std::filesystem::path& aPath)
{
    ID3D11Device* device =
        myRenderer->GetDevice();

    auto shaderBlob = CompileShader(
        aPath,
        "main",
        "vs_5_0"
    );

    DX::Check(
        device->CreateVertexShader(
            shaderBlob->GetBufferPointer(),
            shaderBlob->GetBufferSize(),
            nullptr,
            myVertexShader.ReleaseAndGetAddressOf()
        ),
        "Failed to create vertex shader"
    );

    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {
            "POSITION",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            0,
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        },
        {
            "TEXTURE",
            0,
            DXGI_FORMAT_R32G32_FLOAT,
            0,
            12,
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        }
    };

    DX::Check(
        device->CreateInputLayout(
            layout,
            ARRAYSIZE(layout),
            shaderBlob->GetBufferPointer(),
            shaderBlob->GetBufferSize(),
            myVertexLayout.ReleaseAndGetAddressOf()
        ),
        "Failed to create vertex input layout"
    );
}




void Shader::LoadPixelShader(const std::filesystem::path& aPath)
{
    ID3D11Device* device =
        myRenderer->GetDevice();

    auto shaderBlob = CompileShader(
        aPath,
        "main",
        "ps_5_0"
    );

    DX::Check(
        device->CreatePixelShader(
            shaderBlob->GetBufferPointer(),
            shaderBlob->GetBufferSize(),
            nullptr,
            myPixelShader.ReleaseAndGetAddressOf()
        ),
        "Failed to create pixel shader"
    );
}




void Shader::CreateWorldViewProjectionConstantBuffer()
{
    ID3D11Device* device =
        myRenderer->GetDevice();

    D3D11_BUFFER_DESC bd = {};

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ModelViewProjectionBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    DX::Check(
        device->CreateBuffer(
            &bd,
            nullptr,
            myModelViewProjectionConstantBuffer
            .ReleaseAndGetAddressOf()
        ),
        "Failed to create MVP constant buffer"
    );
}


void Shader::UpdateModelViewProjectionBuffer(DirectX::XMMATRIX matrix)
{
    ModelViewProjectionBuffer buffer = {};
    buffer.modelViewProjection = DirectX::XMMatrixTranspose(matrix);

    ID3D11DeviceContext* context = myRenderer->GetContext();
    context->UpdateSubresource(myModelViewProjectionConstantBuffer.Get(), 0, nullptr, &buffer, 0, 0);
}





void Shader::Use()
{
    ID3D11DeviceContext* context =
        myRenderer->GetContext();

    context->IASetInputLayout(
        myVertexLayout.Get()
    );

    context->VSSetShader(
        myVertexShader.Get(),
        nullptr,
        0
    );

    context->PSSetShader(
        myPixelShader.Get(),
        nullptr,
        0
    );

    context->VSSetConstantBuffers(
        0,
        1,
        myModelViewProjectionConstantBuffer
        .GetAddressOf()
    );
}
