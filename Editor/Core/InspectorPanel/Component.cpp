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


Component::Component(std::string componentName)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dist(1, 2000000);

	myComponentName = componentName;
	myComponentID = dist(gen);
}

void Component::Init()
{

}

void Component::Update(Entity& entityId, CrashOut::Scene& scene, Component &aMyComponents)
{
	auto& registry = scene.GetRegistry();

	ImGui::Begin("Inspector");
	ImGui::PushID(entityId);
	ImGui::PushID(myComponentID);
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
			//scene.component
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


	ImGui::End();
}

