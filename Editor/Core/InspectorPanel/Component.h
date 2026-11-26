#pragma once
#include <EditorVector.h>
#include <string>
#include <iostream>

class Component
{
public:
	Component(std::string componentName);
	~Component() = default;


	void Init();
	void Update();

private:
	CommonUtilities::Vector3<float> vec;
	CommonUtilities::Vector3<float> vec02;
	CommonUtilities::Vector3<float> vec03;
	std::string myComponentName;
};