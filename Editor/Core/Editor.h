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