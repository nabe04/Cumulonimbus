#include "effekseer_component.h"

#include "scene.h"
#include "scene_manager.h"
#include "effekseer_manager.h"
#include "effekseer_loader.h"
#include "locator.h"
#include "transform_component.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::EffekseerComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::EffekseerComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::EffekseerComponent, 2)

namespace cumulonimbus::component
{
	template <class Archive>
	void EffekseerComponent::load(Archive&& archive, const uint32_t version)
	{
		archive(
			cereal::base_class<ComponentBase>(this)
		);

		if(version == 1)
		{
			archive(
				CEREAL_NVP(effect_id)
			);
		}

		if(version == 2)
		{
			archive(
				CEREAL_NVP(effect_id),
				CEREAL_NVP(is_delete_at_end_of_spawn_time),
				CEREAL_NVP(is_delete_all_entity)
			);
		}
	}

	template <class Archive>
	void EffekseerComponent::save(Archive&& archive, const uint32_t version) const
	{
		archive(
			cereal::base_class<ComponentBase>(this)
		);

		if (version >= 1)
		{
			archive(
				CEREAL_NVP(effect_id),
				CEREAL_NVP(is_delete_at_end_of_spawn_time),
				CEREAL_NVP(is_delete_all_entity)
			);
		}
	}

	EffekseerComponent::EffekseerComponent(ecs::Registry* registry, const mapping::rename_type::Entity ent)
		:ComponentBase{ registry,ent }
	{

	}

	EffekseerComponent::EffekseerComponent(
		ecs::Registry* registry,
		const mapping::rename_type::Entity ent,
		const EffekseerComponent& copy_comp)
		: ComponentBase{ registry,ent }
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
		ChangeEffect(effect_id);
	}

	EffekseerComponent::EffekseerComponent(const mapping::component_tag::ComponentTag tag)
		: ComponentBase{ tag }
	{

	}

	EffekseerComponent::~EffekseerComponent()
	{
		//ES_SAFE_RELEASE(effect_);
	}

	void EffekseerComponent::Start()
	{
		//is_spawn = false;
	}

	void EffekseerComponent::CommonUpdate(float dt)
	{
		auto& transform_comp = GetRegistry()->GetComponent<TransformComponent>(GetEntity());
		//const DirectX::SimpleMath::Vector3 l_pos = transform_comp.GetPosition();
		//const DirectX::SimpleMath::Vector4 local_pos = DirectX::SimpleMath::Vector4{ l_pos.x,l_pos.y,l_pos.z,1.0f };

		const DirectX::SimpleMath::Matrix lm = transform_comp.GetLocalMatrix();
		const DirectX::SimpleMath::Matrix wm = transform_comp.GetWorldMatrix();
		//wm.
		const DirectX::SimpleMath::Vector3 local_pos = DirectX::SimpleMath::Matrix{ wm }.Translation();

		//SetPos(transform_comp.GetPosition());
		SetPos(local_pos);
		SetScale(transform_comp.GetScale());
	}

	void EffekseerComponent::GameUpdate(const float dt)
	{
		// 再生削除時の削除フラグがfalseの場合処理を抜ける
		if (!is_delete_at_end_of_spawn_time)
			return;

		// エフェクトが存在しない場合処理を抜ける
		if (!is_spawn)
			return;

		if(!IsPlaying())
		{
			if(is_delete_all_entity)
			{
				GetRegistry()->AddDestroyEntity(GetEntity());
			}
			else
			{
				GetRegistry()->RemoveComponent<EffekseerComponent>(GetEntity());
			}
		}
	}

	void EffekseerComponent::End()
	{
		if (!effect)
			return;

		auto* effekseer_manager = GetRegistry()->GetScene()->GetSceneManager()->GetEffekseerManager();
		const Effekseer::Manager* manager = effekseer_manager->GetManager();

		const_cast<Effekseer::Manager*>(manager)->StopEffect(handle);
	}

	void EffekseerComponent::RenderImGui()
	{
		if (GetRegistry()->CollapsingHeader<EffekseerComponent>(GetEntity(), "Effekseer"))
		{
			auto& asset_manager = *locator::Locator::GetAssetManager();
			auto& asset_sheet_manager = asset_manager.GetAssetSheetManager();

			if(auto* effekseer_loader = asset_manager.GetLoader<asset::EffekseerLoader>();
			   effekseer_loader->ImSelectableEffect(asset_manager, effect_id))
			{
				effect = &effekseer_loader->GetEffect(effect_id);
			}

			if(ImGui::Button("Play"))
			{
				Play();
			}
			ImGui::Checkbox("Is Delete at End of Spawn Time", &is_delete_at_end_of_spawn_time);
			ImGui::Checkbox("Is Delete Entity", &is_delete_all_entity);
		}
	}

	void EffekseerComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);
		ChangeEffect(effect_id);
	}

	void EffekseerComponent::Play()
	{
		auto* effekseer_manager = GetRegistry()->GetScene()->GetSceneManager()->GetEffekseerManager();
		const Effekseer::Manager* manager = effekseer_manager->GetManager();

		if (!effect)
			return;

		is_spawn = true;
		handle   = const_cast<Effekseer::Manager*>(manager)->Play(effect->GetEffect(), { 0, 0.01f, 0 });
	}

	void EffekseerComponent::ChangeEffect(const mapping::rename_type::UUID& efk_id)
	{
		if (efk_id.empty())
			return;

		effect_id = efk_id;
		auto& asset_manager = *locator::Locator::GetAssetManager();
		auto* effekseer_loader = asset_manager.GetLoader<asset::EffekseerLoader>();
		effect = &effekseer_loader->GetEffect(effect_id);
	}

	void EffekseerComponent::SetPos(const DirectX::SimpleMath::Vector3& pos) const
	{
		auto* effekseer_manager = GetRegistry()->GetScene()->GetSceneManager()->GetEffekseerManager();
		const Effekseer::Manager* manager = effekseer_manager->GetManager();

		const_cast<Effekseer::Manager*>(manager)->SetLocation(handle, pos.x, pos.y, pos.z);

	}

	void EffekseerComponent::SetScale(const DirectX::SimpleMath::Vector3& scale) const
	{
		auto* effekseer_manager = GetRegistry()->GetScene()->GetSceneManager()->GetEffekseerManager();
		const Effekseer::Manager* manager = effekseer_manager->GetManager();

		const_cast<Effekseer::Manager*>(manager)->SetScale(handle, scale.x, scale.y, scale.z);
	}

	bool EffekseerComponent::IsPlaying() const
	{
		auto* effekseer_manager = GetRegistry()->GetScene()->GetSceneManager()->GetEffekseerManager();
		const Effekseer::Manager* manager = effekseer_manager->GetManager();
		return const_cast<Effekseer::Manager*>(manager)->GetShown(handle);
	}

} // cumulonimbus::component