#include "Shader.h"
#include "Renderer.h"
#include <iostream>
#include <string>
#include <d3dcompiler.h>

Shader::Shader(Renderer* renderer) : myRenderer(renderer) {}

Shader::~Shader() = default;

void Shader::LoadVertexShader(const std::string& vertex_shader_path)
{
    ID3D11Device* device = myRenderer->GetDevice();
    std::wstring wide_path(vertex_shader_path.begin(), vertex_shader_path.end());

    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;

    // Compile from source at runtime
    HRESULT hr = D3DCompileFromFile(
        wide_path.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main",         // Entry point function name inside your HLSL
        "vs_5_0",        // Target profile (Vertex Shader 5.0)
        D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0,
        &vsBlob,
        &errorBlob
    );

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            // Prints the exact shader compile error (syntax errors, line numbers)
            std::cerr << "Shader compile error:\n"
                << static_cast<char*>(errorBlob->GetBufferPointer()) << std::endl;
            errorBlob->Release();
        }
        return;
    }

    DX::Check(device->CreateVertexShader(
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        nullptr,
        myVertexShader.ReleaseAndGetAddressOf()
    ));

    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    DX::Check(device->CreateInputLayout(
        layout,
        ARRAYSIZE(layout),
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        myVertexLayout.ReleaseAndGetAddressOf()
    ));

    vsBlob->Release();
}


void Shader::LoadPixelShader(const std::string& pixel_shader_path)
{
    ID3D11Device* device = myRenderer->GetDevice();
    std::wstring wide_path(pixel_shader_path.begin(), pixel_shader_path.end());

    ID3DBlob* psBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;

    HRESULT hr = D3DCompileFromFile(
        wide_path.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main",         // Entry point
        "ps_5_0",        // Target profile for pixel shader
        D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0,
        &psBlob,
        &errorBlob
    );

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            std::cerr << "Pixel shader compile error:\n"
                << static_cast<char*>(errorBlob->GetBufferPointer()) << std::endl;
            errorBlob->Release();
        }
        return;
    }

    DX::Check(device->CreatePixelShader(
        psBlob->GetBufferPointer(),
        psBlob->GetBufferSize(),
        nullptr,
        myPixelShader.ReleaseAndGetAddressOf()
    ));

    psBlob->Release();
}

void Shader::Use()
{
    ID3D11DeviceContext* context = myRenderer->GetContext();

    // Bind the input layout to the pipeline's Input Assembler stage
    context->IASetInputLayout(myVertexLayout.Get());

    // Bind the vertex shader to the pipeline's Vertex Shader stage
    context->VSSetShader(myVertexShader.Get(), nullptr, 0);

    // Bind the pixel shader to the pipeline's Pixel Shader stage
    context->PSSetShader(myPixelShader.Get(), nullptr, 0);
}
