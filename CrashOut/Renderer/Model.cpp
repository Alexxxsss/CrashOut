
#include "Model.h"
#include "Renderer.h"

#include <iostream>
#include <vector>
#include <map>
#include <filesystem>
#include <stdexcept>
#include <algorithm>
#include <tuple>
#include <cmath>

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

    // Separate vertices by position, UV AND normal index.
    // A cube needs multiple normals at the same position (sharp edges).
    std::map<std::tuple<int, int, int>, uint32_t> uniqueVertices;
    std::vector<bool> missingNormals;

    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            auto key = std::make_tuple(
                index.vertex_index,
                index.texcoord_index,
                index.normal_index
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

            float nx = 0.0f;
            float ny = 0.0f;
            float nz = 0.0f;
            const bool missingNormal = index.normal_index < 0;

            if (!missingNormal)
            {
                if (static_cast<size_t>(index.normal_index) >=
                    attrib.normals.size() / 3)
                    throw std::runtime_error("Invalid normal index: " + path);

                size_t n = 3 * static_cast<size_t>(index.normal_index);
                nx = attrib.normals[n + 0];
                ny = attrib.normals[n + 1];
                nz = attrib.normals[n + 2];
            }

            Vertex vertex =
            {
                VertexPosition(x, y, -z),
                VertexTextureUV(u, v),
                VertexNormal(nx, ny, -nz)
            };

            uint32_t newIndex =
                static_cast<uint32_t>(vertices.size());

            uniqueVertices.emplace(key, newIndex);

            vertices.push_back(vertex);
            missingNormals.push_back(missingNormal);
            indices.push_back(newIndex);
        }
    }

    if (vertices.empty() || indices.empty())
    {
        throw std::runtime_error("Empty model: " + path);
    }

    for (size_t i = 0; i + 2 < indices.size(); i += 3)
    {
        // Mirroring Z changes winding, so fix it before computing normals.
        std::swap(indices[i + 1], indices[i + 2]);

        const auto& a = vertices[indices[i + 0]].position;
        const auto& b = vertices[indices[i + 1]].position;
        const auto& c = vertices[indices[i + 2]].position;

        const float abx = b.x - a.x, aby = b.y - a.y, abz = b.z - a.z;
        const float acx = c.x - a.x, acy = c.y - a.y, acz = c.z - a.z;
        const float nx = aby * acz - abz * acy;
        const float ny = abz * acx - abx * acz;
        const float nz = abx * acy - aby * acx;

        for (size_t j = 0; j < 3; ++j)
        {
            const uint32_t vertexIndex = indices[i + j];
            if (!missingNormals[vertexIndex])
                continue;
            auto& n = vertices[vertexIndex].normal;
            n.x += nx; n.y += ny; n.z += nz;
        }
    }

    // If an OBJ has no normals, compute smooth normals from its triangles.
    for (size_t i = 0; i < vertices.size(); ++i)
    {
        if (!missingNormals[i])
            continue;
        auto& n = vertices[i].normal;
        const float length = std::sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
        if (length > 0.00001f)
        {
            n.x /= length; n.y /= length; n.z /= length;
        }
        else
        {
            n.x = 0.0f; n.y = 1.0f; n.z = 0.0f;
        }
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
