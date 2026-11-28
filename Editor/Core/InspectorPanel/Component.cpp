#include "Component.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <EditorVector.h>
#include <string>
#include <iostream>
#include <random>
#include <CrashOut/Scene/SceneSerializer.h>
#include <CrashOut/Core/AssetPath.h>
#include <CrashOut/Scene/Scene.h>
#include <tuple>
#include "Inspector.h"
#include "PropertyDrawer.h"
#include <unordered_map>

template<typename T>
void DrawAutoComponent(Entity entity, CrashOut::Scene& scene, Inspector& inspector)
{
	auto* component = scene.GetRegistry().GetComponent<T>(entity);

	if (!component) return;

	bool wasPrimary = false;

	if constexpr (std::is_same_v<T, CameraComponent>)
	{
		wasPrimary = component->primary;
	}

	auto result = PropertyDrawer::Draw(*component);

	if constexpr (std::is_same_v<T, CameraComponent>)
	{
		if (wasPrimary != component->primary)
		{
			scene.SetPrimaryCamera(
				component->primary ? entity : 0
			);
		}
	}

	if constexpr (std::is_same_v<T, BoxColliderComponent>)
	{
		if (result.changed && component->isInitialized)
		{
			scene.GetPhysics().DestroyBoxCollider(
				*component
			);
		}
	}

	if (result.finished)
	{
		inspector.SaveInspector(scene);
	}
}


using DrawFunction = void(*)(Entity, CrashOut::Scene&, Inspector&);

static const std::unordered_map<std::type_index, DrawFunction> componentDrawers =
{
	{ typeid(NameComponent), DrawAutoComponent<NameComponent> },

	{ typeid(TransformComponent), DrawAutoComponent<TransformComponent> },

	{ typeid(CameraComponent), DrawAutoComponent<CameraComponent> },

	{ typeid(MeshComponent), DrawAutoComponent<MeshComponent> },

	{ typeid(MaterialComponent), DrawAutoComponent<MaterialComponent> },

	{ typeid(BoxColliderComponent), DrawAutoComponent<BoxColliderComponent> },

	{ typeid(PointLightComponent), DrawAutoComponent<PointLightComponent> },

	{ typeid(DirectionalLightComponent), DrawAutoComponent<DirectionalLightComponent> },

	{ typeid(SpotLightComponent), DrawAutoComponent<SpotLightComponent> }
};




Component::Component(std::type_index type) : myComponentType(type), myComponentName(type.name())
{
	if (myComponentName.rfind("struct ", 0) == 0)
		myComponentName.erase(0, 7);
}


void Component::Init(std::type_index aComponentType)
{

}

void Component::Update(Entity& entityId, CrashOut::Scene& scene, Inspector& aInspector)
{
	auto& registry = scene.GetRegistry();

	ImGui::Begin("Inspector");

	ImGui::PushID(static_cast<int>(entityId));
	ImGui::PushID(myComponentType.name());
	ImGui::Text(myComponentName.data());
	ImGui::SameLine();


	float buttonWidth = ImGui::CalcTextSize("...").x + ImGui::GetStyle().FramePadding.x * 2.0f;
	float posX = ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - buttonWidth;
	if (posX > ImGui::GetCursorPosX())
	{
		ImGui::SetCursorPosX(posX);
	}

	if (ImGui::SmallButton("..."))
	{
		ImGui::OpenPopup("ComponentSettings");
	}

	if (ImGui::BeginPopup("ComponentSettings"))
	{
		ImGui::TextColored(ImVec4(0, 0, 0, 1), myComponentName.data());
		if (ImGui::MenuItem("Remove Component"))
		{
			registry.RemoveComponent(entityId, myComponentType);
			aInspector.LoadInspectorComponents(scene, entityId);
			aInspector.SaveInspector(scene);
		}
		if (ImGui::MenuItem("Edit Component"))
		{
		}

		ImGui::EndPopup();
	}
	ImGui::Separator();

	ImGui::PopID();
	ImGui::PopID();



	DrawProperties(entityId, scene, aInspector);

	ImGui::Dummy(ImVec2(0.0f, 20.0f));


	ImGui::End();

}


void Component::DrawProperties(Entity entityId, CrashOut::Scene& scene, Inspector& aInspector)
{
	auto it = componentDrawers.find(myComponentType);

	if (it != componentDrawers.end())
	{
		ImGui::PushID(static_cast<int>(entityId));
		ImGui::PushID(myComponentType.name());

		it->second(entityId, scene, aInspector);

		ImGui::PopID();
		ImGui::PopID();
		return;
	}

}