#pragma once

#include <cstdint>
#include <wrl/client.h>
#include <string>
#include <vector>
class Renderer;
struct ID3D11Buffer;
struct ID3D11ShaderResourceView;
struct ID3D11SamplerState;

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

struct VertexNormal
{
	VertexNormal(float x, float y, float z) : x(x), y(y), z(z) {}

	float x = 0;
	float y = 0;
	float z = 0;
};

struct Vertex
{
	VertexPosition position;
	VertexTextureUV texture;
	VertexNormal normal;
};

class Model
{
public:
	Model() = default;
	virtual ~Model();

	void Init(Renderer* renderer);

	void Create(const std::string& path);
	void Render();

	Model(const Model&) = delete;
	Model& operator=(const Model&) = delete;

private:
	void CreateVertexBuffer(const std::vector<Vertex>& vertices);
	void CreateIndexBuffer(const std::vector<uint32_t>& indices);

	Microsoft::WRL::ComPtr<ID3D11Buffer> myIndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> myVertexBuffer;

	uint32_t myIndexCount = 0;
	uint32_t myVertexCount = 0;

	Renderer* myRenderer = nullptr;
};