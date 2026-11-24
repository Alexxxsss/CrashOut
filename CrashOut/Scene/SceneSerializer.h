#pragma once
#include <filesystem>
#include <memory>
#include "Scene.h"

class ResourceManager;

namespace CrashOut
{
    class SceneSerializer
    {
    public:
        static void Save(const Scene& scene, const std::filesystem::path& path);
        static std::unique_ptr<Scene> Load(const std::filesystem::path& path,
            ResourceManager& resources);
        static std::unique_ptr<Scene> Clone(const Scene& scene,
            ResourceManager& resources);
    };
}
