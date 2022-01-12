#include "energy_shot_component.h"

#include "effekseer_component.h"
#include "transform_component.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::EnergyShotComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::EnergyShotComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::EnergyShotComponent, 0)

namespace cumulonimbus::component
{
	template <class Archive>
	void EnergyShotComponent::load(Archive&& archive, uint32_t version)
	{
		archive(
			cereal::base_class<ComponentBase>(this)
		);
	}

	template <class Archive>
	void EnergyShotComponent::save(Archive&& archive, uint32_t version) const
	{
		archive(
			cereal::base_class<ComponentBase>(this)
		);
	}

	EnergyShotComponent::EnergyShotComponent(ecs::Registry* registry, const mapping::rename_type::Entity ent)
		:ComponentBase{ registry,ent }
	{
	}

	EnergyShotComponent::EnergyShotComponent(ecs::Registry* registry, const mapping::rename_type::Entity ent, const EnergyShotComponent& copy_comp)
		: ComponentBase{ registry,ent }
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
	}

	void EnergyShotComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);
	}

	void EnergyShotComponent::Start()
	{
		if(auto* efk_comp = GetRegistry()->TryGetComponent<EffekseerComponent>(GetEntity());
		   efk_comp)
		{
			efk_comp->Play();
		}
	}

	void EnergyShotComponent::GameUpdate(float dt)
	{
		auto& transform_comp = GetRegistry()->GetComponent<TransformComponent>(GetEntity());
		if (auto* efk_comp = GetRegistry()->TryGetComponent<EffekseerComponent>(GetEntity());
			efk_comp)
		{

			//efk_comp->Get();
		}
	}

	void EnergyShotComponent::RenderImGui()
	{
		if (GetRegistry()->CollapsingHeader<EnergyShotComponent>(GetEntity(), "Model"))
		{

		}
	}
} // cumulonimbus::component
