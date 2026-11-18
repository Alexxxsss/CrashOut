#pragma once
#include <iostream>
#include <string>
#include <wrl/client.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <filesystem>

class Renderer;
struct ModelViewProjectionBuffer
{
	DirectX::XMMATRIX modelViewProjection;
};

class Shader
{
public:
	Shader() = default;
	virtual ~Shader();
	void Init(Renderer* renderer);

	// Create vertex shader
	void LoadVertexShader(const std::filesystem::path& aPath);

	// Create pixel shader
	void LoadPixelShader(const std::filesystem::path& aPath);

	void CreateWorldViewProjectionConstantBuffer();
	void UpdateModelViewProjectionBuffer(DirectX::XMMATRIX matrix);

	// Bind the shader to the pipeline
	void Use();
	

	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;
private:
	Renderer* myRenderer = nullptr;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> myVertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> myVertexLayout;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> myPixelShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer> myModelViewProjectionConstantBuffer = nullptr;

};