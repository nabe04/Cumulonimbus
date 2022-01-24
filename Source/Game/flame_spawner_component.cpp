#include "flame_spawner_component.h"

#include "effekseer_component.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::FlameSpownerComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::FlameSpownerComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::FlameSpownerComponent, 0)

namespace cumulonimbus::component
{
	template <class Archive>
	void FlameSpownerComponent::load(Archive&& archive, uint32_t version)
	{
		archive(
			cereal::base_class<ComponentBase>(this)
		);
	}

	template <class Archive>
	void FlameSpownerComponent::save(Archive&& archive, uint32_t version) const
	{
		archive(
			cereal::base_class<ComponentBase>(this)
		);
	}

	FlameSpownerComponent::FlameSpownerComponent(ecs::Registry* registry, const mapping::rename_type::Entity ent)
		:ComponentBase{ registry,ent }
	{
	}

	FlameSpownerComponent::FlameSpownerComponent(ecs::Registry* registry, const mapping::rename_type::Entity ent, const FlameSpownerComponent& copy_comp)
		: ComponentBase{ registry,ent }
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
	}

	void FlameSpownerComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);
	}

	void FlameSpownerComponent::Start()
	{
		auto* effect_comp = GetRegistry()->TryGetComponent<EffekseerComponent>(GetEntity());

		if (!effect_comp)
			return;

		effect_comp->Play();
	}

	void FlameSpownerComponent::GameUpdate(float dt)
	{
		auto* effect_comp = GetRegistry()->TryGetComponent<EffekseerComponent>(GetEntity());

		if (!effect_comp)
			return;

		if (effect_comp->IsPlaying())
			return;

		GetRegistry()->AddDestroyEntity(GetEntity());
	}

	void FlameSpownerComponent::RenderImGui()
	{
		if (GetRegistry()->CollapsingHeader<FlameSpownerComponent>(GetEntity(), "Flame Spawner"))
		{

		}
	}
} // cumulonimbus::component