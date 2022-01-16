#include "effect_spawner_component.h"

#include "locator.h"
#include "effekseer_loader.h"
#include "effekseer_component.h"
#include "scene.h"
#include "scene_manager.h"
#include "transform_component.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::EffectSpawnerComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::SpawnerComponent, cumulonimbus::component::EffectSpawnerComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::EffectSpawnerComponent, 0)

namespace cumulonimbus::component
{
	template <class Archive>
	void EffectSpawnerComponent::load(Archive&& archive, uint32_t version)
	{
		archive(
			cereal::base_class<SpawnerComponent>(this)
		);
	}

	template <class Archive>
	void EffectSpawnerComponent::save(Archive&& archive, uint32_t version) const
	{
		archive(
			cereal::base_class<SpawnerComponent>(this)
		);
	}

	EffectSpawnerComponent::EffectSpawnerComponent(ecs::Registry* registry, mapping::rename_type::Entity ent)
		: SpawnerComponent{ registry,ent }
	{
	}

	EffectSpawnerComponent::EffectSpawnerComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const EffectSpawnerComponent& copy_comp)
		: SpawnerComponent{ registry,ent }
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
	}

	void EffectSpawnerComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);
	}

	void EffectSpawnerComponent::GameUpdate(const float dt)
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

	void EffectSpawnerComponent::RenderImGui()
	{
		if (GetRegistry()->CollapsingHeader<EffectSpawnerComponent>(GetEntity(), "Effect Spawner"))
		{
			auto& asset_manager = *locator::Locator::GetAssetManager();
			auto& effect_loader = *asset_manager.GetLoader<asset::EffekseerLoader>();

			const std::string im_id = "effect_spawner" + GetEntity();

			ImGui::PushID(im_id.c_str());

			if(GetRegistry()->GetScene()->GetSceneManager()->IsInGame())
			{
				ImSpawnButton();
			}

			ImGui::Text("Effect");
			ImGui::SameLine();
			effect_loader.ImSelectableEffect(asset_manager, asset_id);
			SpawnerComponent::RenderImGui();

			ImGui::PopID();
		}
	}

	void EffectSpawnerComponent::Spawn(
		const bool is_first,
		const DirectX::SimpleMath::Vector3& pos,
		const DirectX::SimpleMath::Vector3& direction)
	{
		SpawnerComponent::Spawn(is_first, pos, direction);

		auto& asset_manager = *locator::Locator::GetAssetManager();
		auto& effect_loader = *asset_manager.GetLoader<asset::EffekseerLoader>();

		// エフェクトのアセット番号(asset_id)が存在しない場合処理を抜ける
		if (!effect_loader.HasEffect(asset_id))
			return;

		// 生成数が0個以下の場合処理を抜ける
		if (spawn_count <= 0)
			return;

		// エフェクトの生成
		const auto& spawn_ent = GetRegistry()->CreateEntity();
		GetRegistry()->GetComponent<TransformComponent>(spawn_ent).SetPosition(pos);
		GetRegistry()->AddComponent<EffekseerComponent>(spawn_ent);
		GetRegistry()->GetComponent<EffekseerComponent>(spawn_ent).ChangeEffect(asset_id);
		GetRegistry()->GetComponent<EffekseerComponent>(spawn_ent).SetPos(pos);
		GetRegistry()->GetComponent<EffekseerComponent>(spawn_ent).Play();

		++current_count;
	}
} // cumulonimbus::component
