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

	//if (ImGui::BeginMenu("File"))
	//{
	//	ImGui::MenuItem("Save"); ImGui::EndMenu();
	//}
	ImGui::SameLine(0.0f, 20.f);
	static char entityName[128] = "New Entity";
	if (ImGui::Button("New")) 
	{
		ImGui::OpenPopup("Create Entity");
	}

	if (ImGui::BeginPopupModal("Create Entity", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::InputText("Name", entityName, sizeof(entityName));

		if (ImGui::Button("Create"))
		{
			selectedEntity = scene.CreateEntity(entityName);
			inspector.LoadInspectorComponents(scene, selectedEntity);

			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	ImGui::SameLine();


	if (ImGui::Button("Del")) 
	{ 
		ImGui::OpenPopup("Delete Entity");

	}
	if (ImGui::BeginPopupModal("Delete Entity", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text(registry.GetComponent<NameComponent>(selectedEntity)->name.data());
		if (ImGui::Button("Delete"))
		{
			scene.DestroyEntity(selectedEntity);
			ImGui::CloseCurrentPopup();
			SaveHierarchy(scene);
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}













	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

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


bool Hierarchy::SaveHierarchy(CrashOut::Scene& scene)
{
	try
	{
		CrashOut::SceneSerializer::Save(
			scene,
			CrashOut::AssetPath(L"Scenes/StartScene.json")
		);
		std::cout << "SAVED! \n";
		return true;
	}
	catch(const std::exception& e)
	{
		std::cout << "Could not save the scene! : " << e.what()<<"\n";
		return false;
	}
}