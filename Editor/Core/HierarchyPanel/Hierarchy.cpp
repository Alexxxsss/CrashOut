#include "Hierarchy.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <EditorVector.h>
#include <vector>
#include <CrashOut/Scene/SceneSerializer.h>
#include <CrashOut/Core/AssetPath.h>
#include <CrashOut/Scene/Scene.h>
#include "../InspectorPanel/Inspector.h"

Hierarchy::Hierarchy()
{

}

void Hierarchy::Init()
{

}
void Hierarchy::Update(CrashOut::Scene& scene, Entity &selectedEntity, Inspector& inspector)
{
	auto& registry = scene.GetRegistry();

	int i = 0;
	ImGui::Begin("Hierarchy");
	for (Entity entity : scene.GetEntities())
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth;
		if (selectedEntity == entity)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		bool isOpen = ImGui::TreeNodeEx((void*)(intptr_t)i, flags, registry.GetComponent<NameComponent>(entity)->name.data());

		if (ImGui::IsItemClicked() && selectedEntity != entity)
		{
			selectedEntity = entity;
			inspector.LoadInspectorComponents(scene, selectedEntity);
		}

		//if (isOpen)
		//{
		//	ImGui::TreePop();
		//}
		i++;
	}
	ImGui::End();
}


