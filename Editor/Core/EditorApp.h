#pragma once
#include <CrashOut/Core/Engine.h>
#include <CrashOut/Scene/Scene.h>
#include "EditorWindow.h"

class EditorApp
{
public:
	EditorApp() = default;
	~EditorApp() = default;
	void OnResize(UINT width, UINT height);
	void Resized();
	int Init();
	int Update();
	void Shutdown();


	EditorWindow& GetEditorWindow()
	{
		return editorWindow;
	}

private:
	const UINT myScreenWidth = 1280;
	const UINT myScreenHeight = 720;
	EditorWindow editorWindow;
	Renderer renderer;
	Camera editorCamera;
	Camera gameCamera;
	Inspector inspector;
	Hierarchy hierarchy;
	RenderSystem renderSystem;
	LightingSystem lightingSystem;
	ShadowSystem shadowSystem;
	SceneRenderTarget sceneTarget;
	RasterState resterState;
	std::shared_ptr<Texture> defaultTexture;
	std::shared_ptr<Material> defaultMaterial;
	std::shared_ptr<Shader> shader;
	std::unique_ptr<CrashOut::Scene> scene;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	IDXGISwapChain* swapChain;
	ImGuiIO* io;

	Entity selectedEntity = 0;
	bool useGameCamera = false;
	CommonUtilities::InputHandler* inputHandler;
};