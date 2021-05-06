#pragma once

#include <bitset>

#include "scene.h"

enum class SelectGameMode : int
{
	TraininigMode,
	MainGame,

	End,
};

class SceneTitle :public Scene
{
private:
	std::bitset<static_cast<size_t>(SelectGameMode::End)> select_game_mode{};

public:
	explicit SceneTitle() = default;
	~SceneTitle() = default;

	void InitializeScene() override;
	void UnInitializeScene() override;
	void UpdateScene(const float delta_time) override;
	void RenderImgui() override;

	const auto& GetGameMode() { return select_game_mode; }
};