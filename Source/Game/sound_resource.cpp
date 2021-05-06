#include "sound_resource.h"

SoundResource::SoundResource(HWND hWnd)
{
	sound_manager = std::make_unique<SoundManager>(hWnd);

	enemy_sounds.at(static_cast<int>(Enemy_SE::Bite))			= sound_manager->CreateSoundSource("./Data/SE/Enemy/bite.wav");
	enemy_sounds.at(static_cast<int>(Enemy_SE::BiteHit))		= sound_manager->CreateSoundSource("./Data/SE/Enemy/bite_hit.wav");
	enemy_sounds.at(static_cast<int>(Enemy_SE::RashHit))		= sound_manager->CreateSoundSource("./Data/SE/Enemy/rash_hit.wav");
	enemy_sounds.at(static_cast<int>(Enemy_SE::Roar))			= sound_manager->CreateSoundSource("./Data/SE/Enemy/roar.wav");
	enemy_sounds.at(static_cast<int>(Enemy_SE::ScratchHit))		= sound_manager->CreateSoundSource("./Data/SE/Enemy/scratch_hit.wav");
	enemy_sounds.at(static_cast<int>(Enemy_SE::Thunder))		= sound_manager->CreateSoundSource("./Data/SE/Enemy/thunder.wav");
	enemy_sounds.at(static_cast<int>(Enemy_SE::ThunderHit))		= sound_manager->CreateSoundSource("./Data/SE/Enemy/thunder_hit.wav");
	enemy_sounds.at(static_cast<int>(Enemy_SE::ThunderOmen))	= sound_manager->CreateSoundSource("./Data/SE/Enemy/thunder_omen.wav");

	player_sounds.at(static_cast<int>(Player_SE::Dodge))		= sound_manager->CreateSoundSource("./Data/SE/Player/dodge.wav");
	player_sounds.at(static_cast<int>(Player_SE::FootSteps))	= sound_manager->CreateSoundSource("./Data/SE/Player/foot_steps.wav");
	player_sounds.at(static_cast<int>(Player_SE::Jump))			= sound_manager->CreateSoundSource("./Data/SE/Player/jump.wav");
	player_sounds.at(static_cast<int>(Player_SE::Landing))		= sound_manager->CreateSoundSource("./Data/SE/Player/landing.wav");
	player_sounds.at(static_cast<int>(Player_SE::Slash))		= sound_manager->CreateSoundSource("./Data/SE/Player/slash.wav");
	player_sounds.at(static_cast<int>(Player_SE::SlashHit))		= sound_manager->CreateSoundSource("./Data/SE/Player/slash_hit.wav");
	player_sounds.at(static_cast<int>(Player_SE::Step))			= sound_manager->CreateSoundSource("./Data/SE/Player/step.wav");
	player_sounds.at(static_cast<int>(Player_SE::Stop))			= sound_manager->CreateSoundSource("./Data/SE/Player/stop.wav");
	player_sounds.at(static_cast<int>(Player_SE::Wind))			= sound_manager->CreateSoundSource("./Data/SE/Player/wind.wav");
	player_sounds.at(static_cast<int>(Player_SE::WindHit))		= sound_manager->CreateSoundSource("./Data/SE/Player/wind_hit.wav");

	system_sounds.at(static_cast<int>(System_SE::GameStart))  = sound_manager->CreateSoundSource("./Data/SE/System/game_start.wav");
	system_sounds.at(static_cast<int>(System_SE::MenuCursor)) = sound_manager->CreateSoundSource("./Data/SE/System/menu_cursor.wav");
	system_sounds.at(static_cast<int>(System_SE::MenuSelect)) = sound_manager->CreateSoundSource("./Data/SE/System/menu_select.wav");

	bgm_sounds.at(static_cast<int>(BGM::Title)) = sound_manager->CreateSoundSource("./Data/BGM/title.wav");
	bgm_sounds.at(static_cast<int>(BGM::Game)) = sound_manager->CreateSoundSource("./Data/BGM/battle.wav");
}