#pragma once
#include "component_base.h"

namespace cumulonimbus::component
{
	class MagicCircleSpawnerComponent final : public ComponentBase
	{
	public:
		using  ComponentBase::ComponentBase;
		explicit MagicCircleSpawnerComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit MagicCircleSpawnerComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const MagicCircleSpawnerComponent& copy_comp);
		explicit MagicCircleSpawnerComponent() = default;
		~MagicCircleSpawnerComponent() override = default;

		void Load(ecs::Registry * registry) override;

		void Start() override;

		void GameUpdate(float dt) override;

		void RenderImGui() override;

		template<class Archive>
		void load(Archive&& archive, uint32_t version);
		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;
	private:
		// ‰Î’ŒƒvƒŒƒnƒuID
		mapping::rename_type::UUID flame_prefab_id{};
	};
} // cumulonimbus::component
