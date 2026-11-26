#pragma once
#include <EditorVector.h>
#include <string>
#include <iostream>
#include <CrashOut/Scene/SceneSerializer.h>
#include <CrashOut/Scene/Scene.h>

class Component
{
public:
	Component(std::string componentName);
	~Component() = default;


	void Init();
	void Update(Entity& entityId);

private:
	CommonUtilities::Vector3<float> vec;
	CommonUtilities::Vector3<float> vec02;
	CommonUtilities::Vector3<float> vec03;
	std::string myComponentName;
	int myComponentID;
};