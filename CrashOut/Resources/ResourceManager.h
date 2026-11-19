#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <map>
#include <utility>
#include <cstddef>
class Shader;
class Renderer;
class Model;
class Texture;
class Material;

class ResourceManager
{
public:
    explicit ResourceManager(Renderer& aRenderer);

    std::shared_ptr<Model> LoadModel(
        const std::string& aPath
    );

    std::shared_ptr<Texture> LoadTexture(
        const std::wstring& aPath
    );

    std::shared_ptr<Material> CreateMaterial(
        const std::string& aName,
        std::shared_ptr<Texture> aTexture
    );

    std::shared_ptr<Material> GetMaterial(
        const std::string& aName
    ) const;

    std::shared_ptr<Shader> LoadShader(
        const std::filesystem::path& aVertexPath,
        const std::filesystem::path& aPixelPath
    );

    std::size_t CollectUnused();

private:
    Renderer& myRenderer;

    std::unordered_map<
        std::string,
        std::shared_ptr<Model>
    > myModels;

    std::unordered_map<
        std::wstring,
        std::shared_ptr<Texture>
    > myTextures;

    std::unordered_map<
        std::string,
        std::shared_ptr<Material>
    > myMaterials;


    using ShaderKey = std::pair<
        std::filesystem::path,
        std::filesystem::path
    >;

    std::map<
        ShaderKey,
        std::shared_ptr<Shader>
    > myShaders;
};