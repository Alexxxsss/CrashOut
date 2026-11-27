#pragma once
#include "Editor.h"
#include "EditorApp.h"

int main()
{
	EditorApp editorApp;
	editorApp.Init();
	while (editorApp.GetEditorWindow().UpdateOpenWindow())
	{
		if (editorApp.GetEditorWindow().IsMinimized())
			continue;
		editorApp.Update();
	}
	editorApp.Shutdown();
}