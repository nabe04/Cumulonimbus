#pragma once

#include <array>
#include <memory>

#include "Sound.h"

enum class Player_SE
{
	Dodge,
	FootSteps,
	Jump,
	Landing,
	Slash,
	SlashHit,
	Step,
	Stop,
	Wind,
	WindHit,

	End,
};

enum class Enemy_SE
{
	Bite,
	BiteHit,
	RashHit,
	Roar,
	ScratchHit,
	Thunder,
	ThunderHit,
	ThunderOmen,

	End,
};

enum class System_SE
{
	GameStart,
	MenuCursor,
	MenuSelect,

	End,
};

enum class BGM
{
	Title,
	Game,

	End,
};

class SoundResource
{
private:
	std::unique_ptr<SoundManager> sound_manager = nullptr;

	std::array<std::unique_ptr<SoundSource>, static_cast<int>(Player_SE::End) > player_sounds;
	std::array<std::unique_ptr<SoundSource>, static_cast<int>(Enemy_SE::End)>  enemy_sounds;
	std::array<std::unique_ptr<SoundSource>, static_cast<int>(System_SE::End)>  system_sounds;
	std::array<std::unique_ptr<SoundSource>, static_cast<int>(BGM::End)>		bgm_sounds;

public:
	SoundResource(HWND hWnd);

	SoundManager* GetSoundManager() { return sound_manager.get(); }

	SoundSource* GetPlayerSound(Player_SE index) { return player_sounds.at(static_cast<int>(index)).get(); }
	SoundSource* GetEnemySound(Enemy_SE index) { return enemy_sounds.at(static_cast<int>(index)).get(); }
	SoundSource* GetSystemSound(System_SE index) { return system_sounds.at(static_cast<int>(index)).get(); }
	SoundSource* GetBGMSound(BGM index) { return bgm_sounds.at(static_cast<int>(index)).get(); }
};