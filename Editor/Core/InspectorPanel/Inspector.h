#pragma once
#include <EditorVector.h>

class Inspector
{
public:
	Inspector();
	~Inspector() = default;


	void Init();
	void Update();

private:
	CommonUtilities::Vector3<float> vec;
};