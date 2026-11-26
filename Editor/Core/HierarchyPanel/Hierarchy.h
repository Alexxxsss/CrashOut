#pragma once
#include <EditorVector.h>
#include <vector>
#include <CrashOut/Scene/SceneSerializer.h>
#include <CrashOut/Scene/Scene.h>



struct Node;

class Hierarchy
{
public:
	Hierarchy();
	~Hierarchy() = default;


	void Init();
	void Update(CrashOut::Scene& scene, Entity& selectedEntity);
	

private:
	CrashOut::SceneSerializer ss;

};