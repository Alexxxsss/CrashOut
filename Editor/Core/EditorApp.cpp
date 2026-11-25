#include <iostream>
#include <string>
#include "EditorWindow.h"
#include <CrashOut/Renderer/Renderer.h>
#include <CrashOut/Core/Engine.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

int main()
{
	Renderer renderer;
	EditorWindow editorWindow;
	RasterState resterState;

	editorWindow.StartUpWindow(1280, 720, L"CrashOut");
	HWND hwnd = editorWindow.GetHwnd();


	renderer.Initialize(hwnd,1280, 720);
	resterState.Init(&renderer);
	ID3D11Device *device = renderer.GetDevice();
	ID3D11DeviceContext *context = renderer.GetContext();
	IDXGISwapChain *swapChain = renderer.GetSwapChain();
	ID3D11RenderTargetView *renderTargetView = renderer.GetRenderTargetView();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(device, context);


	while (true)
	{
		editorWindow.UpdateOpenWindow();
		renderer.BeginFrame(0.1f, 0.2f, 0.4f, 1.0f);

		resterState.Use();




		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

		ImGui::Begin("Hierarchy");
		ImGui::Text("Rendered inside a DX11 HWND!");
		ImGui::End();

		ImGui::Begin("Inspector");
		ImGui::Text("Properties go here");
		ImGui::End();

		ImGui::Begin("Assets");
		ImGui::Text("Properties go here");
		ImGui::End();

		ImGui::Begin("Scene");
		ImGui::Text("Properties go here");
		ImGui::End();

		ImGui::Render();
		context->OMSetRenderTargets(1, &renderTargetView, nullptr);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}



		

		renderer.EndFrame();
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	return 0;


}

void Resized()
{
	//renderer.Resize(1280, 720);

}