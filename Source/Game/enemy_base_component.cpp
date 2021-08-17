#include "enemy_base_component.h"

#include "arithmetic.h"
#include "ecs.h"
#include "rename_type_mapping.h"
// components
#include "transform_component.h"
#include "player_component.h"

namespace cumulonimbus::component
{
	void EnemyBaseComponent::RandomFloat::SetRandomVal()
	{
		random_val = arithmetic::RandomFloatInRange(min, max);
	}

	EnemyBaseComponent::EnemyBaseComponent(ecs::Registry* registry, mapping::rename_type::Entity ent)
		:Actor3DComponent{ registry,ent }
	{
	}

	void EnemyBaseComponent::RegisterTransitionTimer(
		const std::string& key_value,
		const float min, const float max)
	{
		if(transition_timer.contains(key_value))
		{
			transition_timer.at(key_value).min = min;
			transition_timer.at(key_value).max = max;
			transition_timer.at(key_value).SetRandomVal();
		}

		const RandomFloat range{ min,max };
		transition_timer.insert(std::make_pair(key_value, range));
		transition_timer.at(key_value).SetRandomVal();
	}


	bool EnemyBaseComponent::Search(
		const DirectX::SimpleMath::Vector3& target_pos,
		const float search_range, const float search_length) const
	{
		const auto& transform_comp = GetRegistry()->GetComponent<TransformComponent>(GetEntity());

		DirectX::SimpleMath::Vector3 self_to_target{ target_pos - transform_comp.GetPosition() };

		// õ“G‹——£ŠO‚È‚çˆ—‚ð’†’f
		if (self_to_target.Length() > search_length)
			return false;
		// õ“G”ÍˆÍŠO‚È‚çˆ—‚ð’†’f
		self_to_target.Normalize();
		if (self_to_target.Dot(transform_comp.GetModelFront()) > DirectX::XMConvertToRadians(search_range / 2))
			return false;

		return true;
	}

	void EnemyBaseComponent::RotateToPlayerDirection() const
	{
		const mapping::rename_type::Entity player_ent = GetRegistry()->GetArray<PlayerComponent>().GetComponents().at(0).GetEntity();
		const DirectX::SimpleMath::Vector3 player_pos = GetRegistry()->GetComponent<TransformComponent>(player_ent).GetPosition();
		auto& transform_comp = GetRegistry()->GetComponent<TransformComponent>(GetEntity());

		DirectX::SimpleMath::Vector3 self_to_player{ player_pos - transform_comp.GetPosition() };
		self_to_player.y = 0;
		self_to_player.Normalize();
		DirectX::SimpleMath::Vector3 model_front = transform_comp.GetModelFront();
		model_front.y = 0;
		model_front.Normalize();

		float dot = model_front.Dot(self_to_player);
		dot		  = arithmetic::Clamp(dot, -1.0f, 1.0f);
		float rad = acosf(dot);
		if (model_front.Cross(self_to_player).y < 0)
			rad *= -1;

		transform_comp.AdjustRotationFromAxis({ 0,1,0 }, rad);
	}

} // cumulonimbus::component
