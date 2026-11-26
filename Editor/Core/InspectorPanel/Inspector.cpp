#include "Inspector.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <EditorVector.h>

Inspector::Inspector()
{
	
}

void Inspector::Init()
{

}

void Inspector::Update()
{
	ImGui::Begin("Inspector");
	ImGui::Text("Transform");
	ImGui::InputFloat3("Vector", &vec.x);
	ImGui::End();

	std::cout << vec.x << " : " << vec.y << " : " << vec.z << "\n";
}