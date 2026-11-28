#pragma once

#include <imgui.h>
#include <DirectXMath.h>
#include "../../External/Boost.PFR/include/boost/pfr.hpp"
#include "../../External/Boost.PFR/include/boost/pfr/core_name.hpp"
//#include <boost/pfr/core_name.hpp>
#include <Ecs/Components.h>
#include <type_traits>
#include <string>
#include <string_view>

namespace PropertyDrawer
{
    struct DrawResult
    {
        bool changed = false;
        bool finished = false;
    };

    template<typename T>
    DrawResult DrawField(std::string_view name, T& value, float min = 0.0f, float max = 0.0f)
    {
        std::string label(name);
        const char* text = label.c_str();

        bool changed = false;
        ImGuiSliderFlags flags = min < max ? ImGuiSliderFlags_AlwaysClamp : ImGuiSliderFlags_None;




        if constexpr (std::is_same_v<T, float>)
        {
            changed = ImGui::DragFloat(
                text, &value, 0.1f, min, max,
                "%.3f", flags
            );
        }

        else if constexpr (std::is_same_v<T, int>)
        {
            changed = ImGui::DragInt(text, &value);
        }
        else if constexpr (std::is_same_v<T, bool>)
        {
            if (name == "isInitialized")
            {
                ImGui::Text(
                    "%s: %s", text,
                    value ? "Yes" : "No"
                );

                return {};
            }

            changed = ImGui::Checkbox(text, &value);

            return { changed, changed };
        }
        else if constexpr (std::is_same_v<T, DirectX::XMFLOAT2>)
        {
            changed = ImGui::DragFloat2(
                text, &value.x, 0.1f
            );
        }
        else if constexpr (std::is_same_v<T, DirectX::XMFLOAT3>)
        {
            if (name == "color")
            {
                changed = ImGui::ColorEdit3(
                    text, &value.x
                );
            }
            else
            {
                changed = ImGui::DragFloat3(
                    text, &value.x, 0.1f, min, max,
                    "%.3f", flags
                );
            }
        }
        else if constexpr (std::is_same_v<T, DirectX::XMFLOAT4>)
        {
            changed = ImGui::DragFloat4(
                text, &value.x, 0.01f
            );

            if (changed && name == "rotation")
            {
                using namespace DirectX;

                auto q = XMLoadFloat4(&value);

                if (XMVectorGetX(XMVector4LengthSq(q)) > 0.000001f)
                {
                    XMStoreFloat4(
                        &value, XMQuaternionNormalize(q)
                    );
                }
                else
                {
                    value = { 0, 0, 0, 1 };
                }
            }
        }
        else if constexpr (std::is_same_v<T, std::string>)
        {
            //if (name == "assetPath" || name == "texturePath")
            //{
            //    ImGui::TextWrapped("%s: %s", text, value.c_str());
            //
            //    return {};
            //}

            char buffer[256] = {};

            value.copy(buffer, sizeof(buffer) - 1);

            changed = ImGui::InputText(text, buffer, sizeof(buffer));

            if (changed)
            {
                value = buffer;
            }
        }
        else if constexpr (std::is_same_v<T, std::shared_ptr<Model>> || std::is_same_v<T, std::shared_ptr<Material>>)
        {
            ImGui::Text("%s: %s", text, value ? "Loaded" : "Not loaded");

            return {};
        }
        else if constexpr (std::is_same_v<T, b3BodyId>)
        {
            ImGui::Text("%s: %llu", text, static_cast<unsigned long long>(b3StoreBodyId(value)));

            return {};
        }
        else if constexpr (std::is_same_v<T, b3ShapeId>)
        {
            ImGui::Text("%s: %llu", text, static_cast<unsigned long long>(b3StoreShapeId(value)));

            return {};
        }
        else
        {
            // Unsupported data type.
            return {};
        }

        return {
            changed,
            ImGui::IsItemDeactivatedAfterEdit() || (changed && !ImGui::IsItemActive())
        };
    }

    template<typename T>
    DrawResult Draw(T& component)
    {
        DrawResult result;

        boost::pfr::for_each_field_with_name(
            component,
            [&](std::string_view name, auto& value)
            {
                float min = 0.0f;
                float max = 0.0f;

                if constexpr (
                    std::is_same_v<T, BoxColliderComponent>)
                {
                    if (name == "halfExtents" ||
                        name == "density")
                    {
                        min = 0.001f;
                        max = 10000.0f;
                    }
                    else if (name == "friction")
                    {
                        min = 0.0f;
                        max = 10.0f;
                    }
                    else if (name == "restitution")
                    {
                        min = 0.0f;
                        max = 1.0f;
                    }
                }

                if constexpr (
                    std::is_same_v<T, CameraComponent>)
                {
                    if (name == "fov")
                    {
                        min = 1.0f;
                        max = 179.0f;
                    }
                    else if (name == "nearPlane")
                    {
                        min = 0.001f;
                        max = component.farPlane - 0.001f;
                    }
                    else if (name == "farPlane")
                    {
                        min = component.nearPlane + 0.001f;
                        max = 100000.0f;
                    }
                }

                auto field = DrawField(
                    name, value, min, max
                );

                result.changed |= field.changed;
                result.finished |= field.finished;
            }
        );

        return result;
    }
}