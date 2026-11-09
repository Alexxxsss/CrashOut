#include "Model.h"
#include "Renderer.h"
#include <iostream>
#include <vector>
#include <filesystem>
#include "../External/WICTextureLoader11.h"





Model::Model(Renderer* renderer) : myRenderer(renderer) {}

Model::~Model() = default;



void Model::Create() 
{
	ID3D11Device* device = myRenderer->GetDevice();

	// Vertex data
	std::vector<Vertex> vertices =
	{
		// Front Face (Z = +0.5)
		{ VertexPosition(-0.5f, +0.5f, +0.5f), VertexTextureUV(0.0f, 0.0f) }, // Top left
		{ VertexPosition(+0.5f, +0.5f, +0.5f), VertexTextureUV(1.0f, 0.0f) }, // Top right
		{ VertexPosition(+0.5f, -0.5f, +0.5f), VertexTextureUV(1.0f, 1.0f) }, // Bottom right
		{ VertexPosition(-0.5f, -0.5f, +0.5f), VertexTextureUV(0.0f, 1.0f) }, // Bottom left

		// Back Face (Z = -0.5)
		{ VertexPosition(+0.5f, +0.5f, -0.5f), VertexTextureUV(0.0f, 1.0f) }, // Top left (from back view)
		{ VertexPosition(-0.5f, +0.5f, -0.5f), VertexTextureUV(1.0f, 1.0f) }, // Top right
		{ VertexPosition(-0.5f, -0.5f, -0.5f), VertexTextureUV(1.0f, 0.0f) }, // Bottom right
		{ VertexPosition(+0.5f, -0.5f, -0.5f), VertexTextureUV(0.0f, 0.0f) }, // Bottom left

		// Left Face (X = -0.5)
		{ VertexPosition(-0.5f, +0.5f, -0.5f), VertexTextureUV(0.0f, 1.0f) }, // Top left
		{ VertexPosition(-0.5f, +0.5f, +0.5f), VertexTextureUV(1.0f, 1.0f) }, // Top right
		{ VertexPosition(-0.5f, -0.5f, +0.5f), VertexTextureUV(1.0f, 0.0f) }, // Bottom right
		{ VertexPosition(-0.5f, -0.5f, -0.5f), VertexTextureUV(0.0f, 0.0f) }, // Bottom left

		// Right Face (X = +0.5)
		{ VertexPosition(+0.5f, +0.5f, +0.5f), VertexTextureUV(0.0f, 1.0f) }, // Top left
		{ VertexPosition(+0.5f, +0.5f, -0.5f), VertexTextureUV(1.0f, 1.0f) }, // Top right
		{ VertexPosition(+0.5f, -0.5f, -0.5f), VertexTextureUV(1.0f, 0.0f) }, // Bottom right
		{ VertexPosition(+0.5f, -0.5f, +0.5f), VertexTextureUV(0.0f, 0.0f) }, // Bottom left

		// Top Face (Y = +0.5)
		{ VertexPosition(-0.5f, +0.5f, -0.5f), VertexTextureUV(0.0f, 1.0f) }, // Top left
		{ VertexPosition(+0.5f, +0.5f, -0.5f), VertexTextureUV(1.0f, 1.0f) }, // Top right
		{ VertexPosition(+0.5f, +0.5f, +0.5f), VertexTextureUV(1.0f, 0.0f) }, // Bottom right
		{ VertexPosition(-0.5f, +0.5f, +0.5f), VertexTextureUV(0.0f, 0.0f) }, // Bottom left

		// Bottom Face (Y = -0.5)
		{ VertexPosition(-0.5f, -0.5f, +0.5f), VertexTextureUV(0.0f, 1.0f) }, // Top left
		{ VertexPosition(+0.5f, -0.5f, +0.5f), VertexTextureUV(1.0f, 1.0f) }, // Top right
		{ VertexPosition(+0.5f, -0.5f, -0.5f), VertexTextureUV(1.0f, 0.0f) }, // Bottom right
		{ VertexPosition(-0.5f, -0.5f, -0.5f), VertexTextureUV(0.0f, 0.0f) }  // Bottom left
	};



	myVertexCount = static_cast<UINT>(vertices.size());

	// Create vertex buffer
	D3D11_BUFFER_DESC vertexbuffer_desc = {};
	vertexbuffer_desc.Usage = D3D11_USAGE_DEFAULT;
	vertexbuffer_desc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertices.size());
	vertexbuffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertex_subdata = {};
	vertex_subdata.pSysMem = vertices.data();

	DX::Check(device->CreateBuffer(&vertexbuffer_desc, &vertex_subdata, myVertexBuffer.ReleaseAndGetAddressOf()));
	CreateIndexBuffer();
}

void Model::CreateIndexBuffer()
{
	ID3D11Device* device = myRenderer->GetDevice();

	// Set Indices
	std::vector<unsigned int> indices =
	{
		// Front Face
		0, 1, 2,  2, 3, 0,
		// Back Face
		4, 5, 6,  6, 7, 4,
		// Left Face
		8, 9, 10, 10, 11, 8,
		// Right Face
		12, 13, 14, 14, 15, 12,
		// Top Face
		16, 17, 18, 18, 19, 16,
		// Bottom Face
		20, 21, 22, 22, 23, 20
	};

	myIndexCount = static_cast<UINT>(indices.size());

	// Create index buffer
	D3D11_BUFFER_DESC index_buffer_desc = {};
	index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	index_buffer_desc.ByteWidth = static_cast<UINT>(sizeof(UINT) * indices.size());
	index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA index_subdata = {};
	index_subdata.pSysMem = indices.data();

	DX::Check(device->CreateBuffer(&index_buffer_desc, &index_subdata, myIndexBuffer.ReleaseAndGetAddressOf()));
}


void Model::Render()
{
	ID3D11DeviceContext* context = myRenderer->GetContext();

	// We need to define the stride and offset
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	// Bind the vertex buffer to the pipeline's Input Assembler stage
	context->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), &stride, &offset);

	// Bind the index buffer to the pipeline's Input Assembler stage
	context->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// Bind the geometry topology to the pipeline's Input Assembler stage
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Render geometry
	context->DrawIndexed(myIndexCount, 0, 0);
}


void Model::LoadTexture()
{
	
	std::wstring path = L"PrototypeTexture.png";

	// Check if file exists
	if (!std::filesystem::exists(path))
	{
		std::wstring error = L"Could not load file: " + path;
		MessageBoxW(NULL, error.c_str(), L"Error", MB_OK);
		return;
	}

	// Load texture into a resource shader view
	ID3D11Device* device = myRenderer->GetDevice();
	ID3D11DeviceContext* context = myRenderer->GetContext();

	ComPtr<ID3D11Resource> resource = nullptr;
	DX::Check(DirectX::CreateWICTextureFromFile(device, context, path.c_str(), resource.ReleaseAndGetAddressOf(), m_DiffuseTexture.ReleaseAndGetAddressOf()));

	D3D11_SAMPLER_DESC desc = {};
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	desc.MinLOD = 0;
	desc.MaxLOD = D3D11_FLOAT32_MAX;

	DX::Check(device->CreateSamplerState(&desc, mySamplerState.ReleaseAndGetAddressOf()));

}


void Model::UseTexture()
{
	ID3D11DeviceContext* context = myRenderer->GetContext();

	// Bind m_DiffuseTexture instead of myTextureView
	context->PSSetShaderResources(0, 1, m_DiffuseTexture.GetAddressOf());
	context->PSSetSamplers(0, 1, mySamplerState.GetAddressOf());
}
