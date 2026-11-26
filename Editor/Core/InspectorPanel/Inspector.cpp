#include "Inspector.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <EditorVector.h>
#include "Component.h"

Inspector::Inspector() = default;

void Inspector::Init()
{

}

void Inspector::Update(CrashOut::Scene& scene, Entity& selectedEntity)
{
	allComponents.clear();

	auto& registry = scene.GetRegistry();
	for (const auto& type : registry.GetComponentTypes(selectedEntity))
	{
		Component component(type.name());
		allComponents.push_back(component);
	}
	ImGui::Begin("Hierarchy");

	for (Component &component : allComponents)
	{
		component.Update(selectedEntity);
	}

	ImGui::End();

}

void LoadInspectorComponents(CrashOut::Scene& scene, Entity& selectedEntity)
{

}