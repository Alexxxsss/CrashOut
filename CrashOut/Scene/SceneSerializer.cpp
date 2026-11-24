#include "SceneSerializer.h"
#include "Resources/ResourceManager.h"
#include <External/nlohmann/json.hpp>
#include <windows.h>
#include <fstream>
#include <stdexcept>
#include <cmath>
#include <limits>
#include <system_error>

namespace CrashOut
{
    namespace
    {
        using Json = nlohmann::json;
        using namespace DirectX;
        constexpr int Version = 1;

        void Require(bool condition, const char* message)
        {
            if (!condition) throw std::runtime_error(message);
        }

        float Number(const Json& j)
        {
            Require(j.is_number(), "Expected number");
            float value = j.get<float>();
            Require(std::isfinite(value), "Non-finite number");
            return value;
        }

        Json Write3(const XMFLOAT3& v)
        {
            Require(std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z),
                "Invalid vector");
            return Json::array({v.x, v.y, v.z});
        }

        Json Write4(const XMFLOAT4& v)
        {
            Require(std::isfinite(v.x) && std::isfinite(v.y) &&
                std::isfinite(v.z) && std::isfinite(v.w), "Invalid quaternion");
            return Json::array({v.x, v.y, v.z, v.w});
        }

        XMFLOAT3 Read3(const Json& j)
        {
            Require(j.is_array() && j.size() == 3, "Expected three-vector");
            return {Number(j[0]), Number(j[1]), Number(j[2])};
        }

        XMFLOAT4 Read4(const Json& j)
        {
            Require(j.is_array() && j.size() == 4, "Expected quaternion (x,y,z,w)");
            XMFLOAT4 q{Number(j[0]), Number(j[1]), Number(j[2]), Number(j[3])};
            auto v = XMLoadFloat4(&q);
            float lengthSq = XMVectorGetX(XMVector4LengthSq(v));
            Require(std::isfinite(lengthSq) && lengthSq > 1e-8f, "Invalid quaternion");
            XMStoreFloat4(&q, XMQuaternionNormalize(v));
            return q;
        }

        Entity ReadId(const Json& j)
        {
            Require(j.is_number_integer(), "Expected integer entity ID");
            if (!j.is_number_unsigned())
                Require(j.get<int64_t>() > 0, "Invalid entity ID");
            uint64_t id = j.get<uint64_t>();
            Require(id > 0 && id <= (std::numeric_limits<Entity>::max)(), "Invalid entity ID");
            return static_cast<Entity>(id);
        }

        void ValidateCamera(const CameraComponent& c)
        {
            Require(std::isfinite(c.fov) && c.fov > 0 && c.fov < 180 &&
                std::isfinite(c.nearPlane) && std::isfinite(c.farPlane) &&
                c.nearPlane > 0 && c.farPlane > c.nearPlane, "Invalid camera lens");
        }

        void ValidateCollider(const BoxColliderComponent& c, const TransformComponent& t)
        {
            auto positive = [](float v) { return std::isfinite(v) && v > 0; };
            Require(positive(c.halfExtents.x) && positive(c.halfExtents.y) &&
                positive(c.halfExtents.z) &&
                positive(c.halfExtents.x * std::abs(t.scale.x)) &&
                positive(c.halfExtents.y * std::abs(t.scale.y)) &&
                positive(c.halfExtents.z * std::abs(t.scale.z)), "Invalid collider size");
            Require(std::isfinite(c.friction) && c.friction >= 0 &&
                std::isfinite(c.restitution) && c.restitution >= 0 && c.restitution <= 1 &&
                std::isfinite(c.density) && c.density >= 0 && (c.isStatic || c.density > 0),
                "Invalid collider material");
        }

        Json Encode(const Scene& scene)
        {
            const auto& registry = scene.GetRegistry();
            Json entities = Json::array();
            int primaryCount = 0;
            for (Entity id : scene.GetEntities())
            {
                auto* name = registry.GetComponent<NameComponent>(id);
                auto* t = registry.GetComponent<TransformComponent>(id);
                Require(name && t, "Scene entities require Name and Transform");
                Json c = Json::object();
                auto q = Write4(t->rotation);
                (void)Read4(q); // Reject invalid quaternions before saving.
                c["Transform"] = {{"position", Write3(t->position)},
                    {"rotation", q}, {"scale", Write3(t->scale)}};

                if (auto* m = registry.GetComponent<MeshComponent>(id))
                {
                    Require(bool(m->model) == !m->assetPath.empty(), "Mesh/path mismatch; use SetMesh");
                    c["Mesh"] = {{"asset_path", m->assetPath}, {"cast_shadows", m->castShadows}};
                }
                if (auto* m = registry.GetComponent<MaterialComponent>(id))
                {
                    Require(bool(m->material) == !m->texturePath.empty(), "Material/path mismatch; use SetMaterial");
                    c["Material"] = {{"texture_path", m->texturePath}};
                }
                if (auto* a = registry.GetComponent<CameraComponent>(id))
                {
                    ValidateCamera(*a);
                    primaryCount += a->primary ? 1 : 0;
                    c["Camera"] = {{"fov", a->fov}, {"near_plane", a->nearPlane},
                        {"far_plane", a->farPlane}, {"is_primary", a->primary}};
                }
                if (auto* b = registry.GetComponent<BoxColliderComponent>(id))
                {
                    ValidateCollider(*b, *t);
                    c["BoxCollider"] = {{"half_extents", Write3(b->halfExtents)},
                        {"local_offset", Write3(b->localOffset)}, {"is_static", b->isStatic},
                        {"friction", b->friction}, {"restitution", b->restitution},
                        {"density", b->density}, {"is_sensor", b->isSensor}};
                }
                if (auto* l = registry.GetComponent<DirectionalLightComponent>(id))
                    c["DirectionalLight"] = {{"direction", Write3(l->direction)},
                        {"color", Write3(l->color)}, {"intensity", Number(l->intensity)},
                        {"enabled", l->enabled}, {"casts_shadows", l->castsShadows}};
                if (auto* l = registry.GetComponent<PointLightComponent>(id))
                    c["PointLight"] = {{"color", Write3(l->color)},
                        {"intensity", Number(l->intensity)}, {"range", Number(l->range)}, {"enabled", l->enabled}};
                if (auto* l = registry.GetComponent<SpotLightComponent>(id))
                    c["SpotLight"] = {{"direction", Write3(l->direction)}, {"color", Write3(l->color)},
                        {"intensity", Number(l->intensity)}, {"range", Number(l->range)},
                        {"inner_angle", Number(l->innerAngleDegrees)}, {"outer_angle", Number(l->outerAngleDegrees)},
                        {"enabled", l->enabled}};
                entities.push_back({{"entity_id", id}, {"name", name->name}, {"components", c}});
            }
            Require(primaryCount <= 1, "Multiple primary cameras");
            return {{"version", Version}, {"scene", scene.GetName()},
                {"ambient", {{"color", Write3(scene.ambientColor)}, {"intensity", Number(scene.ambientIntensity)}}},
                {"entities", entities}};
        }

        std::unique_ptr<Scene> Decode(const Json& root, ResourceManager& resources)
        {
            Require(root.at("version").is_number_integer() && root.at("version") == Version,
                "Unsupported scene version");
            auto scene = std::make_unique<Scene>(root.at("scene").get<std::string>());
            const auto& ambient = root.at("ambient");
            scene->ambientColor = Read3(ambient.at("color"));
            scene->ambientIntensity = Number(ambient.at("intensity"));
            const auto& entities = root.at("entities");
            Require(entities.is_array(), "Expected entities array");
            auto& registry = scene->GetRegistry();
            Entity primary = 0;

            for (const auto& e : entities)
            {
                Entity id = scene->RestoreEntity(ReadId(e.at("entity_id")), e.at("name").get<std::string>());
                const auto& c = e.at("components");
                Require(c.is_object(), "Expected components object");
                auto& t = *registry.GetComponent<TransformComponent>(id);
                const auto& transform = c.at("Transform");
                t.position = Read3(transform.at("position"));
                t.rotation = Read4(transform.at("rotation"));
                t.scale = Read3(transform.at("scale"));

                for (auto it = c.begin(); it != c.end(); ++it)
                {
                    const auto& key = it.key();
                    const auto& d = it.value();
                    if (key == "Transform") continue;
                    else if (key == "Mesh")
                    {
                        auto path = d.at("asset_path").get<std::string>();
                        bool shadows = d.at("cast_shadows").get<bool>();
                        if (path.empty()) registry.AddComponent<MeshComponent>(id).castShadows = shadows;
                        else scene->SetMesh(id, resources, path, shadows);
                    }
                    else if (key == "Material")
                    {
                        auto path = d.at("texture_path").get<std::string>();
                        if (path.empty()) registry.AddComponent<MaterialComponent>(id);
                        else scene->SetMaterial(id, resources, path);
                    }
                    else if (key == "Camera")
                    {
                        auto& a = registry.AddComponent<CameraComponent>(id);
                        a.fov = Number(d.at("fov"));
                        a.nearPlane = Number(d.at("near_plane"));
                        a.farPlane = Number(d.at("far_plane"));
                        ValidateCamera(a);
                        if (d.at("is_primary").get<bool>())
                        {
                            Require(primary == 0, "Multiple primary cameras");
                            primary = id;
                        }
                    }
                    else if (key == "BoxCollider")
                    {
                        auto& b = registry.AddComponent<BoxColliderComponent>(id);
                        b.halfExtents = Read3(d.at("half_extents"));
                        b.localOffset = Read3(d.at("local_offset"));
                        b.isStatic = d.at("is_static").get<bool>();
                        b.friction = Number(d.at("friction"));
                        b.restitution = Number(d.at("restitution"));
                        b.density = Number(d.at("density"));
                        b.isSensor = d.at("is_sensor").get<bool>();
                        ValidateCollider(b, t);
                        // Runtime body/shape IDs stay null. Never deserialize them.
                    }
                    else if (key == "DirectionalLight")
                    {
                        auto& l = registry.AddComponent<DirectionalLightComponent>(id);
                        l.direction = Read3(d.at("direction"));
                        l.color = Read3(d.at("color"));
                        l.intensity = Number(d.at("intensity"));
                        l.enabled = d.at("enabled").get<bool>();
                        l.castsShadows = d.at("casts_shadows").get<bool>();
                    }
                    else if (key == "PointLight")
                    {
                        auto& l = registry.AddComponent<PointLightComponent>(id);
                        l.color = Read3(d.at("color"));
                        l.intensity = Number(d.at("intensity"));
                        l.range = Number(d.at("range"));
                        l.enabled = d.at("enabled").get<bool>();
                    }
                    else if (key == "SpotLight")
                    {
                        auto& l = registry.AddComponent<SpotLightComponent>(id);
                        l.direction = Read3(d.at("direction"));
                        l.color = Read3(d.at("color"));
                        l.intensity = Number(d.at("intensity"));
                        l.range = Number(d.at("range"));
                        l.innerAngleDegrees = Number(d.at("inner_angle"));
                        l.outerAngleDegrees = Number(d.at("outer_angle"));
                        l.enabled = d.at("enabled").get<bool>();
                    }
                    else throw std::runtime_error("Unsupported scene component: " + key);
                }
            }
            scene->SetPrimaryCamera(primary);
            return scene;
        }
    }

    void SceneSerializer::Save(const Scene& scene, const std::filesystem::path& path)
    {
        auto data = Encode(scene).dump(2);
        auto temp = path;
        temp += L".tmp";
        {
            std::ofstream file(temp, std::ios::binary | std::ios::trunc);
            if (!file) throw std::runtime_error("Cannot open temporary scene file");
            file << data << '\n';
            file.close();
            if (!file) throw std::runtime_error("Cannot write scene file");
        }
        // Same-directory replacement: do not truncate the existing scene first.
        if (!MoveFileExW(temp.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
            throw std::system_error(GetLastError(), std::system_category(), "Cannot replace scene file");
    }

    std::unique_ptr<Scene> SceneSerializer::Load(const std::filesystem::path& path,
        ResourceManager& resources)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file) throw std::runtime_error("Cannot open scene: " + path.string());
        return Decode(Json::parse(file), resources);
    }

    std::unique_ptr<Scene> SceneSerializer::Clone(const Scene& scene, ResourceManager& resources)
    {
        return Decode(Encode(scene), resources);
    }
}
