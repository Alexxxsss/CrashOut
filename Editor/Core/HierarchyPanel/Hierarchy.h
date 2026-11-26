#pragma once
#include <EditorVector.h>
#include <vector>
#include <CrashOut/Scene/SceneSerializer.h>
#include <CrashOut/Scene/Scene.h>


class Inspector;

class Hierarchy
{
public:
	Hierarchy();
	~Hierarchy() = default;


	void Init();
	void Update(CrashOut::Scene& scene, Entity& selectedEntity, Inspector& inspector);
	

private:
	CrashOut::SceneSerializer ss;

};