#pragma once
#include "component_base.h"

namespace cumulonimbus::component
{
	class EnergyShotComponent final : public ComponentBase
	{
	public:
		using ComponentBase::ComponentBase;
		explicit EnergyShotComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit EnergyShotComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const EnergyShotComponent& copy_comp);
		explicit EnergyShotComponent()  = default; // for cereal
		~EnergyShotComponent() override = default;

		void Load(ecs::Registry* registry) override;

		void Start() override;
		void GameUpdate(float dt) override;

		void RenderImGui() override;

		template<class Archive>
		void load(Archive&& archive, uint32_t version);

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;
	private:

	};
} // cumulonimbus::component