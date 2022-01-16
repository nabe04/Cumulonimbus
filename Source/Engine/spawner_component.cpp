#include "spawner_component.h"

#include "cum_imgui_helper.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::SpawnerComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::SpawnerComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::SpawnerComponent, 1)

namespace cumulonimbus::component
{
	template <class Archive>
	void SpawnerComponent::load(Archive&& archive, const uint32_t version)
	{
		archive(
			cereal::base_class<ComponentBase>(this),
			CEREAL_NVP(asset_id),
			CEREAL_NVP(spawn_interval_time),
			CEREAL_NVP(spawn_interval_distance)
		);

		if(version == 1)
		{
			archive(
				CEREAL_NVP(spawn_count)
			);
		}
	}

	template <class Archive>
	void SpawnerComponent::save(Archive&& archive, const uint32_t version) const
	{
		archive(
			cereal::base_class<ComponentBase>(this),
			CEREAL_NVP(asset_id),
			CEREAL_NVP(spawn_interval_time),
			CEREAL_NVP(spawn_interval_distance)
		);

		if (version == 1)
		{
			archive(
				CEREAL_NVP(spawn_count)
			);
		}
	}

	SpawnerComponent::SpawnerComponent(ecs::Registry* registry, mapping::rename_type::Entity ent)
		:ComponentBase{ registry,ent }
	{
	}

	SpawnerComponent::SpawnerComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const SpawnerComponent& copy_comp)
		: ComponentBase{ registry,ent }
	{
	}

	void SpawnerComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);
	}

	void SpawnerComponent::SceneUpdate(float dt)
	{

	}

	void SpawnerComponent::RenderImGui()
	{
		IMGUI_LEFT_LABEL(ImGui::DragFloat3, "Position"			, &root_pos.x				, 0.1f   , FLT_MAX, FLT_MAX);
		IMGUI_LEFT_LABEL(ImGui::DragFloat3, "Direction"			, &direction.x				, 0.01f  , FLT_MIN, FLT_MAX);
		IMGUI_LEFT_LABEL(ImGui::DragInt   , "Count			 "	, &spawn_count				, 1		 , 0	  , INT_MAX);
		IMGUI_LEFT_LABEL(ImGui::DragFloat , "Interval Time	 "	, &spawn_interval_time		, 0.01f	 , 0.0f	  , FLT_MAX);
		IMGUI_LEFT_LABEL(ImGui::DragFloat , "Interval Distance" , &spawn_interval_distance	, 0.01f	 , 0.0f	  , FLT_MAX);
	}

	void SpawnerComponent::Spawn(
		const bool is_first,
		const DirectX::SimpleMath::Vector3& pos,
		const DirectX::SimpleMath::Vector3& direction)
	{
		if(is_first)
		{
			elapsed_time = 0;
			current_count = 0;
		}

		// ¶¬ƒtƒ‰ƒO‚ðOn
		is_spawn = true;
		root_pos = pos;
		this->direction = direction;
	}

	void SpawnerComponent::ImSpawnButton()
	{
		if(ImGui::Button("Spawn"))
		{
			Spawn(true, root_pos, direction);
		}
	}

} // cumulonimbus::component
