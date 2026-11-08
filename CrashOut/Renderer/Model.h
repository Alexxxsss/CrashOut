#pragma once

#include <cstdint>
#include <wrl/client.h>

class Renderer;
struct ID3D11Buffer;
struct ID3D11ShaderResourceView;
struct ID3D11ShaderResourceView;

struct VertexPosition
{
	VertexPosition(float x, float y, float z) : x(x), y(y), z(z) {}

	float x = 0;
	float y = 0;
	float z = 0;
};

struct VertexColour
{
	VertexColour(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}

	float r = 0;
	float g = 0;
	float b = 0;
	float a = 0;
};

struct VertexTextureUV
{
	VertexTextureUV(float u, float v) : u(u), v(v) {}

	float u = 0;
	float v = 0;
};

struct Vertex
{
	VertexPosition position;
	VertexTextureUV texture;
};

class Model
{
public:
    Model(Renderer* renderer);
    virtual ~Model(); 

    void Create();
    void CreateIndexBuffer();

    void Render();
	void LoadTexture();


private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> myIndexBuffer = nullptr;
    int myIndexCount = 0;

    uint32_t myVertexCount = 0;
    Microsoft::WRL::ComPtr<ID3D11Buffer> myVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_DiffuseTexture = nullptr;

    Renderer* myRenderer = nullptr; 
};