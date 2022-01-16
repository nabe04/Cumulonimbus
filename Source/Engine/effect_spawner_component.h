#pragma once
#include "spawner_component.h"

namespace cumulonimbus::component
{
	class EffectSpawnerComponent final : public SpawnerComponent
	{
	public:
		using SpawnerComponent::SpawnerComponent;
		explicit EffectSpawnerComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit EffectSpawnerComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const EffectSpawnerComponent& copy_comp);
		explicit EffectSpawnerComponent()  = default;
		~EffectSpawnerComponent() override = default;

		void Load(ecs::Registry* registry) override;

		void GameUpdate(float dt) override;

		void RenderImGui() override;

		template<class Archive>
		void load(Archive&& archive, uint32_t version);
		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;

		void Spawn(bool is_first, const DirectX::SimpleMath::Vector3& pos, const DirectX::SimpleMath::Vector3& direction) override;
	private:

	};
} // cumulonimbus::component
