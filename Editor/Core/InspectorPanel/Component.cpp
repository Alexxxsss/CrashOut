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

	ImGui::PopID();
	ImGui::PopID();


	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	DrawProperties(entityId, scene, aInspector);
	ImGui::End();

}


void Component::DrawProperties(Entity entityId, CrashOut::Scene& scene, Inspector& aInspector)
{
	auto& registry = scene.GetRegistry();

	if (myComponentType == typeid(NameComponent))
	{
		auto* c = registry.GetComponent<NameComponent>(entityId);

		char buffer[256] = {};
		c->name.copy(buffer, sizeof(buffer) - 1);

		if (ImGui::InputText("Name", buffer, sizeof(buffer)))
		{
			c->name = buffer;
		}
	}
	else if (myComponentType == typeid(TransformComponent))
	{
		auto* c = registry.GetComponent<TransformComponent>(entityId);
		ImGui::DragFloat3("Position", &c->position.x, 0.1f);
		if(ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}

		if (ImGui::DragFloat4("Rotation", &c->rotation.x, 0.01f))
		{
			using namespace DirectX;

			auto q = XMLoadFloat4(&c->rotation);

			if (XMVectorGetX(XMVector4LengthSq(q)) > 0.000001f)
				XMStoreFloat4(&c->rotation, XMQuaternionNormalize(q));
			else
				c->rotation = { 0, 0, 0, 1 };
		}
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}

		ImGui::DragFloat3("Scale", &c->scale.x, 0.1f);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}

	}
	else if (myComponentType == typeid(CameraComponent))
	{
		auto* c = registry.GetComponent<CameraComponent>(entityId);

		ImGui::DragFloat("FOV", &c->fov, 0.1f, 1.0f, 179.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}
		ImGui::DragFloat(
			"Near Plane", &c->nearPlane,
			0.01f, 0.001f, c->farPlane - 0.001f
		);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}
		ImGui::DragFloat(
			"Far Plane", &c->farPlane,
			1.0f, c->nearPlane + 0.001f, 100000.0f
		);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}
		bool primary = c->primary;

		if (ImGui::Checkbox("Primary", &primary))
		{
			scene.SetPrimaryCamera(primary ? entityId : 0);
		}
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}
	}
	else if (myComponentType == typeid(DirectionalLightComponent))
	{
		auto* c =
			registry.GetComponent<DirectionalLightComponent>(entityId);

		ImGui::DragFloat3("Direction", &c->direction.x, 0.01f);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}
		ImGui::ColorEdit3("Color", &c->color.x);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}
		ImGui::DragFloat("Intensity", &c->intensity, 0.1f);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}
		ImGui::Checkbox("Enabled", &c->enabled);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}
		ImGui::Checkbox("Cast Shadows", &c->castsShadows);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}
	}
	else if (myComponentType == typeid(PointLightComponent))
	{
		auto* c =
			registry.GetComponent<PointLightComponent>(entityId);

		ImGui::ColorEdit3("Color", &c->color.x);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}
		ImGui::DragFloat("Intensity", &c->intensity, 0.1f);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}
		ImGui::DragFloat("Range", &c->range, 0.1f);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}
		ImGui::Checkbox("Enabled", &c->enabled);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}
	}
	else if (myComponentType == typeid(SpotLightComponent))
	{
		auto* c =
			registry.GetComponent<SpotLightComponent>(entityId);

		ImGui::DragFloat3("Direction", &c->direction.x, 0.01f);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}
		ImGui::ColorEdit3("Color", &c->color.x);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}
		ImGui::DragFloat("Intensity", &c->intensity, 0.1f);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}
		ImGui::DragFloat("Range", &c->range, 0.1f);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}

		ImGui::DragFloat(
			"Inner Angle", &c->innerAngleDegrees, 0.1f
		);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}
		ImGui::DragFloat(
			"Outer Angle", &c->outerAngleDegrees, 0.1f
		);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}
		ImGui::Checkbox("Enabled", &c->enabled);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}
	}
	else if (myComponentType == typeid(BoxColliderComponent))
	{
		auto* c =
			registry.GetComponent<BoxColliderComponent>(entityId);

		bool changed = false;

		changed |= ImGui::DragFloat3(
			"Half Extents", &c->halfExtents.x,
			0.01f, 0.001f, 10000.0f
		);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}
		changed |= ImGui::DragFloat3(
			"Local Offset", &c->localOffset.x, 0.01f
		);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}
		changed |= ImGui::Checkbox("Static", &c->isStatic);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}
		changed |= ImGui::DragFloat(
			"Friction", &c->friction, 0.01f, 0.0f, 10.0f
		);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}
		changed |= ImGui::DragFloat(
			"Restitution", &c->restitution, 0.01f, 0.0f, 1.0f
		);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}
		changed |= ImGui::DragFloat(
			"Density", &c->density, 0.01f, 0.001f, 10000.0f
		);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}
		changed |= ImGui::Checkbox("Sensor", &c->isSensor);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}
		if (changed && c->isInitialized)
		{
			scene.GetPhysics().DestroyBoxCollider(*c);
		}
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}
		ImGui::Text("Initialized: %s",
			c->isInitialized ? "Yes" : "No");
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}
		ImGui::Text("Body ID: %llu",
			static_cast<unsigned long long>(
				b3StoreBodyId(c->bodyId)));
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}
		ImGui::Text("Shape ID: %llu",
			static_cast<unsigned long long>(
				b3StoreShapeId(c->shapeId)));
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}
	}
	else if (myComponentType == typeid(MeshComponent))
	{
		auto* c = registry.GetComponent<MeshComponent>(entityId);

		ImGui::TextWrapped("Asset: %s", c->assetPath.c_str());
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}
		ImGui::Text("Model: %s",
			c->model ? "Loaded" : "Not loaded");
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}
		ImGui::Checkbox("Cast Shadows", &c->castShadows);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}
	}
	else if (myComponentType == typeid(MaterialComponent))
	{
		auto* c =
			registry.GetComponent<MaterialComponent>(entityId);

		ImGui::TextWrapped(
			"Texture: %s", c->texturePath.c_str()
		);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}
		ImGui::Text("Material: %s",
			c->material ? "Loaded" : "Not loaded");
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aInspector.SaveInspector(scene);
		}
	}
	//automaize
}