#include "prefab_spawner_component.h"

#include "locator.h"
#include "prefab_loader.h"
#include "transform_component.h"
#include "scene.h"
#include "scene_manager.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::PrefabSpawnerComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::SpawnerComponent, cumulonimbus::component::PrefabSpawnerComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::PrefabSpawnerComponent, 0)

namespace cumulonimbus::component
{
	template <class Archive>
	void PrefabSpawnerComponent::load(Archive&& archive, uint32_t version)
	{
		archive(
			cereal::base_class<SpawnerComponent>(this)
		);
	}

	template <class Archive>
	void PrefabSpawnerComponent::save(Archive&& archive, uint32_t version) const
	{
		archive(
			cereal::base_class<SpawnerComponent>(this)
		);
	}

	PrefabSpawnerComponent::PrefabSpawnerComponent(ecs::Registry* registry, const mapping::rename_type::Entity ent)
		:SpawnerComponent{ registry,ent }
	{
	}

	PrefabSpawnerComponent::PrefabSpawnerComponent(ecs::Registry* registry, const mapping::rename_type::Entity ent, const PrefabSpawnerComponent& copy_comp)
		: SpawnerComponent{ registry,ent }
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
	}

	void PrefabSpawnerComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);
	}

	void PrefabSpawnerComponent::Start()
	{
		auto& transform_comp = GetRegistry()->GetComponent<TransformComponent>(GetEntity());
		auto pos = transform_comp.GetPosition();
		Spawn(true, transform_comp.GetPosition(), transform_comp.GetModelFront());
	}

	void PrefabSpawnerComponent::GameUpdate(const float dt)
	{
		// 生成フラグが立っていない場合処理を抜ける
		if (!is_spawn)
			return;

		elapsed_time += dt;

		if (elapsed_time < spawn_interval_time)
			return;

		elapsed_time = 0;

		const DirectX::SimpleMath::Vector3 spawn_pos = root_pos + (direction * spawn_interval_distance * static_cast<float>(current_count));
		Spawn(false, spawn_pos, direction);

		if (current_count >= spawn_count)
			is_spawn = false;
	}

	void PrefabSpawnerComponent::RenderImGui()
	{
		if (GetRegistry()->CollapsingHeader<PrefabSpawnerComponent>(GetEntity(), "Prefab Spawner"))
		{
			auto& asset_manager = *locator::Locator::GetAssetManager();
			auto& prefab_loader = *asset_manager.GetLoader<asset::PrefabLoader>();

			if (GetRegistry()->GetScene()->GetSceneManager()->IsInGame())
			{
				ImSpawnButton();
			}

			ImGui::Text("Prefab");
			ImGui::SameLine();
			prefab_loader.ImSelectablePrefab(asset_manager, asset_id);
			SpawnerComponent::RenderImGui();
		}
	}

	void PrefabSpawnerComponent::Spawn(const bool is_first, const DirectX::SimpleMath::Vector3& pos, const DirectX::SimpleMath::Vector3& direction)
	{
		SpawnerComponent::Spawn(is_first, pos, direction);

		auto& asset_manager = *locator::Locator::GetAssetManager();
		auto& prefab_loader = *asset_manager.GetLoader<asset::PrefabLoader>();

		// プレハブのアセット番号(asset_id)が存在しない場合処理を抜ける
		if (!prefab_loader.HasPrefab(asset_id))
			return;

		// 生成数が0個以下の場合処理を抜ける
		if (spawn_count <= 0)
			return;

		// プレハブの生成
		const mapping::rename_type::Entity spawn_ent = prefab_loader.Instantiate(GetRegistry(), asset_id);
		const auto p = pos;
		GetRegistry()->GetComponent<TransformComponent>(spawn_ent).SetPosition(p);
		//GetRegistry()->GetComponent<TransformComponent>(spawn_ent).SetPosition(DirectX::SimpleMath::Vector3{ 50,0,50 });

		++current_count;
	}

} // cumulonimbus::component
