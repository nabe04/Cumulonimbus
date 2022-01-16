#pragma once
#include "spawner_component.h"

namespace cumulonimbus::component
{
	class PrefabSpawnerComponent final : public SpawnerComponent
	{
	public:
		using SpawnerComponent::SpawnerComponent;
		explicit PrefabSpawnerComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit PrefabSpawnerComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const PrefabSpawnerComponent& copy_comp);
		explicit PrefabSpawnerComponent()  = default;
		~PrefabSpawnerComponent() override = default;

		void Load(ecs::Registry* registry) override;

		void Start() override;
		
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
