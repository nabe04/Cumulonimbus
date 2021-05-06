#include "effekseer_resource.h"


EffekseerResource::EffekseerResource(const Effekseer::Manager* manager)
{
	// —á
	//resource.at(static_cast<int>(EffekseerResourceName::Default)) = Create(manager, ",,,");
	//Attack
	resource.at(static_cast<int>(EffekseerResourceName::EnemyClaw)) = Create(manager, "./Data/Assets/Effekseer/claw/claw.efk");
	resource.at(static_cast<int>(EffekseerResourceName::EnemyFang)) = Create(manager, "./Data/Assets/Effekseer/fang/fang.efk");
	resource.at(static_cast<int>(EffekseerResourceName::EnemyRush)) = Create(manager, "./Data/Assets/Effekseer/dash/dash.efk");
	//Rush end
	resource.at(static_cast<int>(EffekseerResourceName::EnemyBrake)) = Create(manager, "./Data/Assets/Effekseer/brake/brake.efk");
	//Lightning
	resource.at(static_cast<int>(EffekseerResourceName::EnemyLightning)) = Create(manager, "./Data/Assets/Effekseer/thunder/lightning.efk");
	resource.at(static_cast<int>(EffekseerResourceName::EnemyLightningCircle)) = Create(manager, "./Data/Assets/Effekseer/thunder/circle.efk");
	//Thunder
	resource.at(static_cast<int>(EffekseerResourceName::EnemyLightningFloor)) = Create(manager, "./Data/Assets/Effekseer/lightning/thunder01.efk");
	//Other effects
	resource.at(static_cast<int>(EffekseerResourceName::EnemyAwaking_begin)) = Create(manager, "./Data/Assets/Effekseer/awakening/awakening01.efk");
	resource.at(static_cast<int>(EffekseerResourceName::EnemyAwaking_end)) = Create(manager, "./Data/Assets/Effekseer/awakening/awakening02.efk");

	resource.at(static_cast<int>(EffekseerResourceName::PlayerMove)) = Create(manager, "./Data/Assets/Effekseer/dash_smoke/move_dash.efk");
	resource.at(static_cast<int>(EffekseerResourceName::PlayerEvasion)) = Create(manager, "./Data/Assets/Effekseer/dash/dash.efk");
	resource.at(static_cast<int>(EffekseerResourceName::PlayerJustEvasion)) = Create(manager, "./Data/Assets/Effekseer/dogde/just_evasion.efk");

	resource.at(static_cast<int>(EffekseerResourceName::PlayerDamage))      = Create(manager, "./Data/Assets/Effekseer/wolf_blow_02/player_damage.efk");
	resource.at(static_cast<int>(EffekseerResourceName::PlayerJump))        = Create(manager, "./Data/Assets/Effekseer/jump/jump.efk");
	resource.at(static_cast<int>(EffekseerResourceName::PlayerMagicCircle)) = Create(manager, "./Data/Assets/Effekseer/magic_circle/magic_circle2.efk");
	resource.at(static_cast<int>(EffekseerResourceName::PlayerAttackSlash)) = Create(manager, "./Data/Assets/Effekseer/slash/hit_player_attack.efk");
	resource.at(static_cast<int>(EffekseerResourceName::PlayerWind))        = Create(manager, "./Data/Assets/Effekseer/wind/magic_wind.efk");
	resource.at(static_cast<int>(EffekseerResourceName::PlayerDamageRush))  = Create(manager, "./Data/Assets/Effekseer/hit/player_hit_rush.efk");
	resource.at(static_cast<int>(EffekseerResourceName::PlayerAttack01))    = Create(manager, "./Data/Assets/Effekseer/swing_03/swing_01.efk");
	resource.at(static_cast<int>(EffekseerResourceName::PlayerAttack02))    = Create(manager, "./Data/Assets/Effekseer/swing_03/swing_02.efk");
	resource.at(static_cast<int>(EffekseerResourceName::PlayerAttack03))    = Create(manager, "./Data/Assets/Effekseer/swing_03/swing_03.efk");
	resource.at(static_cast<int>(EffekseerResourceName::PlayerAttack04))    = Create(manager, "./Data/Assets/Effekseer/swing_03/swing_04.efk");
	resource.at(static_cast<int>(EffekseerResourceName::PlayerAttack05))    = Create(manager, "./Data/Assets/Effekseer/swing_03/swing_05.efk");
	resource.at(static_cast<int>(EffekseerResourceName::PlayerAttack06))    = Create(manager, "./Data/Assets/Effekseer/swing_03/swing_06.efk");
	resource.at(static_cast<int>(EffekseerResourceName::PlayerAttack07))    = Create(manager, "./Data/Assets/Effekseer/swing_03/swing_07.efk");

}