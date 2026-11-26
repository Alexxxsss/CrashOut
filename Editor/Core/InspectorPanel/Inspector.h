#pragma once
#include <EditorVector.h>
#include "Component.h"
#include <vector>
#include <CrashOut/Scene/SceneSerializer.h>
#include <CrashOut/Scene/Scene.h>

class Inspector
{
public:
	Inspector();
	~Inspector() = default;


	void Init();
	void Update(CrashOut::Scene& scene, Entity& selectedEntity);
	void LoadInspectorComponents(CrashOut::Scene& scene, Entity& selectedEntity);

private:
	std::vector<Component> allComponents;
	
};