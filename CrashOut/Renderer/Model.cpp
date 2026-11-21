
#include "Model.h"
#include "Renderer.h"

#include <iostream>
#include <vector>
#include <map>
#include <filesystem>
#include <stdexcept>
#include <algorithm>

#define TINYOBJLOADER_IMPLEMENTATION
#include "../External/tiny_obj_loader.h"

Model::~Model() = default;

void Model::Init(Renderer* renderer)
{
    myRenderer = renderer;
}

void Model::Create(const std::string& path)
{
    if (!std::filesystem::is_regular_file(path))
    {
        throw std::runtime_error(
            "Model file does not exist: " + path
        );
    }

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string error;

    std::string baseDir =
        std::filesystem::path(path).parent_path().string();

    if (!baseDir.empty())
        baseDir += "/";

    bool success = tinyobj::LoadObj(
        &attrib,
        &shapes,
        &materials,
        &error,
        path.c_str(),
        baseDir.c_str(),
        true
    );

    if (!success)
    {
        throw std::runtime_error(
            "Failed to load model: " + path + "\n" + error
        );
    }

    if (!error.empty())
        std::cerr << error << std::endl;

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    // Maps OBJ position and UV indices to our vertex index.
    std::map<std::pair<int, int>, uint32_t> uniqueVertices;

    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            auto key = std::make_pair(
                index.vertex_index,
                index.texcoord_index
            );

            auto it = uniqueVertices.find(key);

            if (it != uniqueVertices.end())
            {
                indices.push_back(it->second);
                continue;
            }

            if (index.vertex_index < 0 ||
                static_cast<size_t>(index.vertex_index) >=
                attrib.vertices.size() / 3)
            {
                throw std::runtime_error(
                    "Invalid vertex index: " + path
                );
            }

            size_t positionIndex =
                3 * static_cast<size_t>(index.vertex_index);

            float x = attrib.vertices[positionIndex + 0];
            float y = attrib.vertices[positionIndex + 1];
            float z = attrib.vertices[positionIndex + 2];

            float u = 0.0f;
            float v = 0.0f;

            if (index.texcoord_index >= 0)
            {
                if (static_cast<size_t>(index.texcoord_index) >=
                    attrib.texcoords.size() / 2)
                {
                    throw std::runtime_error(
                        "Invalid UV index: " + path
                    );
                }

                size_t uvIndex =
                    2 * static_cast<size_t>(index.texcoord_index);

                u = attrib.texcoords[uvIndex + 0];
                v = 1.0f - attrib.texcoords[uvIndex + 1];
            }

            Vertex vertex =
            {
                VertexPosition(x, y, -z),
                VertexTextureUV(u, v)
            };

            uint32_t newIndex =
                static_cast<uint32_t>(vertices.size());

            uniqueVertices.emplace(key, newIndex);

            vertices.push_back(vertex);
            indices.push_back(newIndex);
        }
    }

    if (vertices.empty() || indices.empty())
    {
        throw std::runtime_error("Empty model: " + path);
    }

    for (size_t i = 0; i + 2 < indices.size(); i += 3)
    {
        std::swap(indices[i + 1], indices[i + 2]);
    }


    CreateVertexBuffer(vertices);
    CreateIndexBuffer(indices);

    std::cout << "Loaded model: " << path << "\n"
        << "Vertices: " << myVertexCount << "\n"
        << "Indices: " << myIndexCount << "\n"
        << "Triangles: " << myIndexCount / 3
        << std::endl;
}

void Model::CreateVertexBuffer(
    const std::vector<Vertex>& vertices)
{
    ID3D11Device* device = myRenderer->GetDevice();

    myVertexCount = static_cast<uint32_t>(vertices.size());

    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = static_cast<UINT>(
        sizeof(Vertex) * vertices.size()
        );
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA data = {};
    data.pSysMem = vertices.data();

    DX::Check(device->CreateBuffer(
        &desc,
        &data,
        myVertexBuffer.ReleaseAndGetAddressOf()
    ));
}

void Model::CreateIndexBuffer(
    const std::vector<uint32_t>& indices)
{
    ID3D11Device* device = myRenderer->GetDevice();

    myIndexCount = static_cast<uint32_t>(indices.size());

    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = static_cast<UINT>(
        sizeof(uint32_t) * indices.size()
        );
    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA data = {};
    data.pSysMem = indices.data();

    DX::Check(device->CreateBuffer(
        &desc,
        &data,
        myIndexBuffer.ReleaseAndGetAddressOf()
    ));
}

void Model::Render()
{
    if (!myVertexBuffer || !myIndexBuffer)
        return;

    ID3D11DeviceContext* context = myRenderer->GetContext();

    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    context->IASetVertexBuffers(
        0, 1, myVertexBuffer.GetAddressOf(),
        &stride, &offset
    );

    context->IASetIndexBuffer(
        myIndexBuffer.Get(),
        DXGI_FORMAT_R32_UINT,
        0
    );

    context->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
    );

    context->DrawIndexed(myIndexCount, 0, 0);
}
