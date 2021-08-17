#include "enemy_forest_demon_component.h"

#include "ecs.h"

// std::unordered_map<std::string, RandomFloat> transition_timerのキー値用変数
namespace
{
	const std::string transition_idle_to_walk{ "idle_to_walk" };
	const std::string transition_walk_to_idle{ "walk_to_idle" };
}

namespace cumulonimbus::component
{
	EnemyForestDemonComponent::EnemyForestDemonComponent(ecs::Registry* registry, mapping::rename_type::Entity ent)
		:EnemyBaseComponent{registry,ent}
	{
		// stateに応じての処理の登録
		forest_demon_state.AddState(ForestDemonState::T_Pose			, [ent, registry](const float dt) {registry->GetComponent<EnemyForestDemonComponent>(ent).TPose(dt); });
		forest_demon_state.AddState(ForestDemonState::Idle				, [ent, registry](const float dt) { registry->GetComponent<EnemyForestDemonComponent>(ent).Idle(dt); });
		forest_demon_state.AddState(ForestDemonState::Ready				, [ent, registry](const float dt) {registry->GetComponent<EnemyForestDemonComponent>(ent).Ready(dt); });
		forest_demon_state.AddState(ForestDemonState::Walk				, [ent, registry](const float dt) {registry->GetComponent<EnemyForestDemonComponent>(ent).Walk(dt); });
		forest_demon_state.AddState(ForestDemonState::Tracking			, [ent, registry](const float dt) {registry->GetComponent<EnemyForestDemonComponent>(ent).Tracking(dt); });
		forest_demon_state.AddState(ForestDemonState::Death				, [ent, registry](const float dt) {registry->GetComponent<EnemyForestDemonComponent>(ent).Death(dt); });
		forest_demon_state.AddState(ForestDemonState::Damage			, [ent, registry](const float dt) {registry->GetComponent<EnemyForestDemonComponent>(ent).Damage(dt); });
		forest_demon_state.AddState(ForestDemonState::Attack_Normal		, [ent, registry](const float dt) {registry->GetComponent<EnemyForestDemonComponent>(ent).AttackNormal(dt); });
		forest_demon_state.AddState(ForestDemonState::Attack_Mow_Down	, [ent, registry](const float dt) {registry->GetComponent<EnemyForestDemonComponent>(ent).AttackMowDown(dt); });

		// 初期stateの設定(ForestDemonState::Idle)
		forest_demon_state.SetState(ForestDemonState::Idle);

		// transition_timerの設定
		RegisterTransitionTimer(transition_idle_to_walk, 3.f, 7.f);
		RegisterTransitionTimer(transition_walk_to_idle, 3.f, 5.f);

		// yaw回転でのおランダム値設定
		SetRandomRotationAngle(-180.f, 180.f);
	}

	void EnemyForestDemonComponent::Update(const float dt)
	{
		forest_demon_state.Update(dt);
	}

	void EnemyForestDemonComponent::RenderImGui()
	{
	}

	void EnemyForestDemonComponent::Save(const std::string& file_path)
	{
	}

	void EnemyForestDemonComponent::Load(const std::string& file_path_and_name)
	{
	}

	void EnemyForestDemonComponent::SetRandomRotationAngle(float min, float max)
	{
		random_rotation_angle.min = min;
		random_rotation_angle.max = max;
		random_rotation_angle.SetRandomVal();
	}

	int EnemyForestDemonComponent::GetAnimDataIndex(AnimationData anim_data) const
	{
		return static_cast<int>(anim_data);
	}

	void EnemyForestDemonComponent::TPose(float dt)
	{
	}

	void EnemyForestDemonComponent::Idle(float dt)
	{
	}

	void EnemyForestDemonComponent::Ready(float dt)
	{
	}

	void EnemyForestDemonComponent::Walk(float dt)
	{
	}

	void EnemyForestDemonComponent::Tracking(float dt)
	{
	}

	void EnemyForestDemonComponent::Death(float dt)
	{
	}

	void EnemyForestDemonComponent::Damage(float dt)
	{
	}

	void EnemyForestDemonComponent::AttackNormal(float dt)
	{
	}

	void EnemyForestDemonComponent::AttackMowDown(float dt)
	{
	}
} // cumulonimbus::component
