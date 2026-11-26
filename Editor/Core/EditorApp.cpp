#include <iostream>
#include <string>
#include "EditorWindow.h"
#include <CrashOut/Renderer/Renderer.h>
#include <CrashOut/Core/Engine.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include"InspectorPanel/Inspector.h"
#include "HierarchyPanel/Hierarchy.h"
#include "Scene/Scene.h"
#include "Scene/SceneSerializer.h"
#include "Resources/ResourceManager.h"
#include "Core/AssetPath.h"
#include "Renderer/SceneRenderTarget.h"
#include "Resources/ResourceManager.h"
#include "Renderer/Texture.h"
#include "Core/AssetPath.h"
void OnResize(UINT width, UINT height);
void Resized();
const UINT myScreenWidth = 1280;
const UINT myScreenHeight = 720;
EditorWindow editorWindow;
Renderer renderer;
Camera editorCamera;
Camera gameCamera;
int main()
{

	editorWindow.StartUpWindow(1280, 720, L"CrashOut");
	HWND hwnd = editorWindow.GetHwnd();

	renderer.Initialize(hwnd, 1280, 720);

	ID3D11Device* device = renderer.GetDevice();
	ID3D11DeviceContext* context = renderer.GetContext();
	IDXGISwapChain* swapChain = renderer.GetSwapChain();
	ID3D11RenderTargetView* renderTargetView =
		renderer.GetRenderTargetView();

	RasterState resterState;
	resterState.Init(&renderer);

	Inspector inspector;
	Hierarchy hierarchy;

	Entity selectedEntity = 0;

	ResourceManager resources(renderer);
	RenderSystem renderSystem;
	LightingSystem lightingSystem;
	ShadowSystem shadowSystem;
	SceneRenderTarget sceneTarget;

	lightingSystem.Init(renderer);
	shadowSystem.Init(renderer);

	

	editorCamera.Init(1280, 720);

	auto shader = resources.LoadShader(
		CrashOut::AssetPath(L"Shaders/Vertex.hlsl"),
		CrashOut::AssetPath(L"Shaders/Pixel.hlsl")
	);

	auto defaultTexture = resources.LoadTexture(
		CrashOut::AssetPath(
			L"Assets/Textures/PrototypeTexture.png"
		).wstring()
	);

	auto defaultMaterial = resources.CreateMaterial(
		"DefaultMaterial", defaultTexture
	);

	auto scene = CrashOut::SceneSerializer::Load(
		CrashOut::AssetPath(L"Scenes/StartScene.json"),
		resources
	);

	bool useGameCamera = false;





	
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(device, context);


	CommonUtilities::InputHandler& inputHandler = *editorWindow.GetInputHandler();


	

	while (true)
	{
		editorWindow.UpdateOpenWindow();
		renderer.BeginFrame(0.1f, 0.2f, 0.4f, 1.0f);

		resterState.Use();


		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

		hierarchy.Update(*scene, selectedEntity, inspector);

		inspector.Update(*scene, selectedEntity);

		ImGui::Begin("Assets");
		ImGui::Text("Properties go here");
		ImGui::End();


		///
		/// Starting
		///
		ImGui::Begin("Scene");

		ImGui::Checkbox("Game Camera", &useGameCamera);

		ImVec2 size = ImGui::GetContentRegionAvail();

		if (size.x >= 1 && size.y >= 1)
		{
			UINT width = static_cast<UINT>(size.x);
			UINT height = static_cast<UINT>(size.y);

			sceneTarget.Resize(renderer, width, height);
			sceneTarget.Bind(renderer);

			resterState.Use();

			editorCamera.UpdateAspectRatio(width, height);

			Camera* activeCamera = &editorCamera;

			if (useGameCamera &&
				scene->BuildPrimaryCamera(gameCamera, width, height))
			{
				activeCamera = &gameCamera;
			}

			lightingSystem.SetAmbient(
				scene->ambientColor,
				scene->ambientIntensity
			);

			float speed = 5.0f;
			float deltaTime = ImGui::GetIO().DeltaTime;

			float movement = speed * deltaTime;

			if (ImGui::IsWindowHovered() && !useGameCamera)
			{
				if (inputHandler.IsKeyDown('W'))
					editorCamera.Move(0, 0, movement);

				if (inputHandler.IsKeyDown('S'))
					editorCamera.Move(0, 0, -movement);

				if (inputHandler.IsKeyDown('A'))
					editorCamera.Move(-movement, 0, 0);

				if (inputHandler.IsKeyDown('D'))
					editorCamera.Move(movement, 0, 0);

				if (inputHandler.IsKeyDown('E'))
					editorCamera.Move(0, movement, 0);

				if (inputHandler.IsKeyDown('Q'))
					editorCamera.Move(0, -movement, 0);
			}

			renderSystem.Render(
				scene->GetRegistry(),
				*activeCamera,
				*shader,
				*defaultMaterial,
				lightingSystem,
				shadowSystem
			);

			auto* backBuffer = renderer.GetRenderTargetView();

			context->OMSetRenderTargets(
				1, &backBuffer, nullptr);

			ImGui::Image(
				(ImTextureID)(intptr_t)sceneTarget.GetSRV(),
				size
			);
		}

		ImGui::End();
		///
		/// ENDING
		///

		ImGui::Render();
		context->OMSetRenderTargets(1, &renderTargetView, nullptr);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}



		
		//Resized();

		renderer.EndFrame();
	}


	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	return 0;


}


void OnResize(UINT width, UINT height)
{
	editorCamera.UpdateAspectRatio(width, height);
	gameCamera.UpdateAspectRatio(width, height);
}
void Resized()
{
	UINT width = myScreenWidth;
	UINT height = myScreenHeight;
	if (editorWindow.WasResized())
	{
		if (width > 0 && myScreenHeight > 0)
		{
			renderer.Resize(width, height);

			OnResize(width, height);
		}
	}

}