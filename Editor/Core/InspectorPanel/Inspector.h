#pragma once
#include <EditorVector.h>
#include "Component.h"

class Inspector
{
public:
	Inspector();
	~Inspector() = default;


	void Init();
	void Update();

private:
	Component comp01 {"Transform"};
	Component comp02{ "Something" };
	Component comp03{ "Something" };
};