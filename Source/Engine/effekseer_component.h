#pragma once
#include "component_base.h"

namespace cumulonimbus::component
{
	class EffekseerComponent : public ComponentBase
	{
	public:
		explicit EffekseerComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit EffekseerComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const EffekseerComponent& copy_comp);
		explicit EffekseerComponent(mapping::component_tag::ComponentTag tag);
		explicit EffekseerComponent()  = default; // for cereal
		~EffekseerComponent() override;

		void RenderImGui() override;

		void Load(ecs::Registry* registry) override;

		template<class Archive>
		void load(Archive&& archive, uint32_t version);

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;

		void Play();
		void ChangeEffect();

	private:
		mapping::rename_type::UUID effect_id{};
		Effekseer::Effect* effect{ nullptr };
		Effekseer::Handle handle{ -1 };
	};
} // cumulonimbus::component