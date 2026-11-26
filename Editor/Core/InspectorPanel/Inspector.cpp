#include "Inspector.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <EditorVector.h>
#include "Component.h"

Inspector::Inspector() = default;

void Inspector::Init()
{

}

void Inspector::Update()
{

	comp01.Update();
	comp02.Update();
	comp03.Update();
}