#pragma once
#include "light_component.h"

#include "constant_buffer.h"
#include "cbuffer_lights.h"

namespace cumulonimbus::component
{
	class PointLightComponent final : public LightComponent
	{
	public:
		using LightComponent::LightComponent;
		explicit PointLightComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit PointLightComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const PointLightComponent& copy_comp); // for prefab
		explicit PointLightComponent() = default; // for cereal
		~PointLightComponent() override = default;

		template<class Archive>
		void load(Archive&& archive, uint32_t version);

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;

		void Initialize(ecs::Registry* registry, mapping::rename_type::Entity ent) override;

		void CommonUpdate(float dt) override;
		void PostCommonUpdate(float dt) override;

		void RenderImGui() override;

		void Load(ecs::Registry* registry) override;

		[[nodiscard]]
		const PointLight& GetPointLight() const
		{
			return point_light;
		}
	private:
		PointLight point_light{};
	};
} // cumulonimbus::component