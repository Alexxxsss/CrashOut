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

void Component::Update(Entity& entityId)
{
	ImGui::Begin("Inspector");
	ImGui::PushID(entityId);
	ImGui::PushID(myComponentID);
	ImGui::Text(myComponentName.data());

	ImGui::DragFloat3("Position", &vec.x);
	ImGui::DragFloat3("Rotation", &vec02.x);
	ImGui::DragFloat3("Scale", &vec03.x);

	ImGui::PopID();
	ImGui::PopID();


	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();


	ImGui::End();
}