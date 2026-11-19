#include "ResourceManager.h"

#include "../Renderer/Renderer.h"
#include "../Renderer/Model.h"
#include "../Renderer/Texture.h"
#include "../Renderer/Material.h"
#include "../Renderer/Shader.h"

#include <stdexcept>
#include <iostream>

ResourceManager::ResourceManager(Renderer& aRenderer)
    : myRenderer(aRenderer)
{
}

std::shared_ptr<Model> ResourceManager::LoadModel(
    const std::string& aPath)
{
    auto it = myModels.find(aPath);

    if (it != myModels.end())
    {
        return it->second;
    }

    auto model = std::make_shared<Model>();

    model->Init(&myRenderer);
    model->Create(aPath);

    myModels.emplace(aPath, model);

    return model;
}

std::shared_ptr<Texture> ResourceManager::LoadTexture(
    const std::wstring& aPath)
{
    auto it = myTextures.find(aPath);

    if (it != myTextures.end())
    {
        return it->second;
    }

    auto texture = std::make_shared<Texture>();

    texture->Load(myRenderer, aPath);

    myTextures.emplace(aPath, texture);

    return texture;
}


std::shared_ptr<Material> ResourceManager::CreateMaterial(
    const std::string& aName,
    std::shared_ptr<Texture> aTexture)
{
    auto it = myMaterials.find(aName);

    if (it != myMaterials.end())
    {
        return it->second;
    }

    if (!aTexture)
    {
        throw std::invalid_argument(
            "Material requires a valid texture"
        );
    }

    auto material = std::make_shared<Material>();

    material->Init(&myRenderer);
    material->SetTexture(aTexture);

    myMaterials.emplace(aName, material);

    return material;
}

std::shared_ptr<Material> ResourceManager::GetMaterial(
    const std::string& aName) const
{
    auto it = myMaterials.find(aName);

    if (it != myMaterials.end())
    {
        return it->second;
    }

    return nullptr;
}


std::shared_ptr<Shader> ResourceManager::LoadShader(
    const std::filesystem::path& aVertexPath,
    const std::filesystem::path& aPixelPath)
{
    ShaderKey key = {
        std::filesystem::absolute(aVertexPath)
            .lexically_normal(),

        std::filesystem::absolute(aPixelPath)
            .lexically_normal()
    };

    auto it = myShaders.find(key);

    if (it != myShaders.end())
    {
        return it->second;
    }

    auto shader = std::make_shared<Shader>();

    shader->Init(&myRenderer);

    shader->LoadVertexShader(key.first);
    shader->LoadPixelShader(key.second);

    shader->CreateWorldViewProjectionConstantBuffer();

    myShaders.emplace(key, shader);

    return shader;
}

namespace
{
    template<typename Cache>
    std::size_t RemoveUnused(Cache& cache)
    {
        std::size_t removed = 0;

        for (auto it = cache.begin();
            it != cache.end();)
        {
            if (it->second.use_count() == 1)
            {
                it = cache.erase(it);
                ++removed;
            }
            else
            {
                ++it;
            }
        }

        return removed;
    }
}


std::size_t ResourceManager::CollectUnused()
{
    std::size_t removed = 0;

    // Materials reference textures, so remove
    // unused materials before unused textures.
    removed += RemoveUnused(myMaterials);

    removed += RemoveUnused(myTextures);

    removed += RemoveUnused(myModels);

    removed += RemoveUnused(myShaders);

    return removed;
}