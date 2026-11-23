#pragma once

class Registry;
class Camera;
class Shader;
class Material;
class LightingSystem;
class ShadowSystem;

class RenderSystem
{
public:
    RenderSystem() = default;
    ~RenderSystem() = default;

    void Render(
        Registry& aRegistry,
        Camera& aCamera,
        Shader& aShader,
        Material& aDefaultMaterial,
        LightingSystem& aLightingSystem,
        ShadowSystem& aShadowSystem
    );
};
