#include "Component.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <EditorVector.h>
#include <string>
#include <iostream>
#include <random>

Component::Component(std::string componentName)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dist(1000, 9999);

	myComponentName = componentName + std::to_string(dist(gen));
}

void Component::Init()
{

}

void Component::Update()
{
	ImGui::Begin("Inspector");
	ImGui::Text(myComponentName.data());
	ImGui::InputFloat3("Position", &vec.x);
	ImGui::InputFloat3("Rotation", &vec02.x);
	ImGui::InputFloat3("Scale", &vec03.x);


	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();


	ImGui::End();
}