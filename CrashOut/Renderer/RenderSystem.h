#pragma once

class Registry;
class Camera;
class Shader;
class Material;

class RenderSystem
{
public:
    RenderSystem() = default;
    ~RenderSystem() = default;

    void Render(
        Registry& aRegistry,
        Camera& aCamera,
        Shader& aShader,
        Material& aDefaultMaterial
    );
};
