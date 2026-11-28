#pragma once
#include <EditorVector.h>
#include <string>
#include <iostream>
#include <vector>
#include <CrashOut/Scene/SceneSerializer.h>
#include <CrashOut/Scene/Scene.h>
#include <typeindex>

class Inspector;
class Component
{
public:
	Component(std::type_index type);
	~Component() = default;


	void Init(std::type_index aComponentType);
	void Update(Entity& entityId, CrashOut::Scene& scene, Inspector& aInspector);

private:
	void DrawProperties(Entity entityId, CrashOut::Scene& scene, Inspector& aInspector);
	std::string myComponentName;
	std::type_index myComponentType;
};