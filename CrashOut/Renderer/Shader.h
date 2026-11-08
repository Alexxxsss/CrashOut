#pragma once
#include <iostream>
#include <string>
#include <wrl/client.h>
#include <d3d11.h>
#include <DirectXMath.h>

class Renderer;
struct ModelViewProjectionBuffer
{
	DirectX::XMMATRIX modelViewProjection;
};

class Shader
{
public:
	Shader(Renderer* renderer);
	virtual ~Shader();

	// Create vertex shader
	void LoadVertexShader(const std::string& vertex_shader_path);

	// Create pixel shader
	void LoadPixelShader(const std::string& pixel_shader_path);

	void CreateWorldViewProjectionConstantBuffer();
	void UpdateModelViewProjectionBuffer(DirectX::XMMATRIX matrix);

	// Bind the shader to the pipeline
	void Use();

private:
	Renderer* myRenderer;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> myVertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> myVertexLayout;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> myPixelShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer> myModelViewProjectionConstantBuffer = nullptr;

};