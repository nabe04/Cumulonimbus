#pragma once

#include <memory>

#include "scene.h"

class SceneLoad :public Scene
{
private:
	std::shared_ptr<Scene> next_scene;
	bool is_initialized = true;

public:
	SceneLoad(Scene* next_scene);

	void InitializeScene() override;
	void UnInitializeScene() override;
	void UpdateScene(const float delta_time) override;

	static void LoadingThread(SceneLoad* scene);
};
