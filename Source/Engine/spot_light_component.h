#pragma once
#include "light_component.h"

#include "cbuffer_lights.h"

namespace cumulonimbus::component
{
	class SpotLightComponent final : public LightComponent
	{
	public:
		using LightComponent::LightComponent;
		explicit SpotLightComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit SpotLightComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const SpotLightComponent& copy_comp); // for prefab
		explicit SpotLightComponent() = default; // for cereal
		~SpotLightComponent() override = default;

		template<class Archive>
		void load(Archive && archive, uint32_t version);

		template<class Archive>
		void save(Archive && archive, uint32_t version) const;

		void CommonUpdate(float dt) override;
		void PostCommonUpdate(float dt) override;

		void RenderImGui() override;

		void Load(ecs::Registry* registry) override;

		[[nodiscard]]
		const SpotLight& GetSpotLight() const
		{
			return spot_light;
		}
	private:
		SpotLight spot_light{};
	};
} // cumulonimbus::component
