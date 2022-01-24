#include "magic_circle_spawner_component.h"

#include "locator.h"
#include "prefab_loader.h"
#include "effekseer_component.h"
#include "transform_component.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::MagicCircleSpawnerComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::MagicCircleSpawnerComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::MagicCircleSpawnerComponent, 0)

namespace cumulonimbus::component
{
	template <class Archive>
	void MagicCircleSpawnerComponent::load(Archive&& archive, uint32_t version)
	{
		archive(
			cereal::base_class<ComponentBase>(this),
			CEREAL_NVP(flame_prefab_id)
		);
	}

	template <class Archive>
	void MagicCircleSpawnerComponent::save(Archive&& archive, uint32_t version) const
	{
		archive(
			cereal::base_class<ComponentBase>(this),
			CEREAL_NVP(flame_prefab_id)
		);
	}

	MagicCircleSpawnerComponent::MagicCircleSpawnerComponent(ecs::Registry* registry, mapping::rename_type::Entity ent)
		:ComponentBase{ registry,ent }
	{
	}

	MagicCircleSpawnerComponent::MagicCircleSpawnerComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const MagicCircleSpawnerComponent& copy_comp)
		: ComponentBase{ registry,ent }
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
	}

	void MagicCircleSpawnerComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);
	}

	void MagicCircleSpawnerComponent::Start()
	{
		auto* effect_comp = GetRegistry()->TryGetComponent<EffekseerComponent>(GetEntity());

		if (!effect_comp)
			return;

		effect_comp->Play();
	}

	void MagicCircleSpawnerComponent::GameUpdate(float dt)
	{
		auto* effect_comp = GetRegistry()->TryGetComponent<EffekseerComponent>(GetEntity());

		if (!effect_comp)
			return;

		if (effect_comp->IsPlaying())
			return;

		auto& asset_manager = *locator::Locator::GetAssetManager();
		auto& prefab_loader = *asset_manager.GetLoader<asset::PrefabLoader>();

		if (!prefab_loader.HasPrefab(flame_prefab_id))
			return;

		// 火柱プレハブの生成
		//const auto spawn_ent = prefab_loader.Instantiate(GetRegistry(), flame_prefab_id);
		//const auto self_pos  = GetRegistry()->GetComponent<TransformComponent>(GetEntity()).GetPosition();
		//GetRegistry()->GetComponent<TransformComponent>(spawn_ent).SetPosition(self_pos);
		Spawn();
		// マジックサークルエフェクトオブジェクトの削除
		GetRegistry()->AddDestroyEntity(GetEntity());
	}

	void MagicCircleSpawnerComponent::RenderImGui()
	{
		if(GetRegistry()->CollapsingHeader<MagicCircleSpawnerComponent>(GetEntity(),"Magic Circle Spawner"))
		{
			auto& asset_manager = *locator::Locator::GetAssetManager();
			auto& prefab_loader = *asset_manager.GetLoader<asset::PrefabLoader>();

			ImGui::Text("Flame Prefab");
			prefab_loader.ImSelectablePrefab(asset_manager, flame_prefab_id);

			if(ImGui::Button("Spawn"))
			{
				Spawn();
			}
		}
	}

	void MagicCircleSpawnerComponent::Spawn()
	{
		auto& asset_manager  = *locator::Locator::GetAssetManager();
		auto& prefab_loader  = *asset_manager.GetLoader<asset::PrefabLoader>();
		const auto spawn_ent = prefab_loader.Instantiate(GetRegistry(), flame_prefab_id);
		const auto self_pos  = GetRegistry()->GetComponent<TransformComponent>(GetEntity()).GetPosition();
		GetRegistry()->GetComponent<TransformComponent>(spawn_ent).SetPosition(self_pos);
	}

} // cumulonimbus::component
