#pragma once

#include <cstdint>
#include <wrl/client.h>

class Renderer;
struct ID3D11Buffer;

struct Vertex
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

class Model
{
public:
    Model(Renderer* renderer);
    virtual ~Model(); 

    void Create();
    void Render();

private:
    uint32_t myVertexCount = 0;
    Microsoft::WRL::ComPtr<ID3D11Buffer> myVertexBuffer;
    Renderer* myRenderer = nullptr; 
};