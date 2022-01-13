#pragma once
#include "component_base.h"
#include "effect.h"

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

		void End() override;

		void RenderImGui() override;

		void Load(ecs::Registry* registry) override;

		template<class Archive>
		void load(Archive&& archive, uint32_t version);

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;

		void Play();
		void ChangeEffect(const mapping::rename_type::UUID& efk_id);

		void SetPos(const DirectX::SimpleMath::Vector3& pos) const;
	private:
		mapping::rename_type::UUID effect_id{};
		asset::Effect* effect{};

		//Effekseer::Effect* effect_{ nullptr };
		Effekseer::Handle handle{ -1 };
	};
} // cumulonimbus::component