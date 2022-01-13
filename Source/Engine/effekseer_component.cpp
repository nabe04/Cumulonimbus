#include "effekseer_component.h"

#include "scene.h"
#include "scene_manager.h"
#include "effekseer_manager.h"
#include "effekseer_loader.h"
#include "locator.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::EffekseerComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::EffekseerComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::EffekseerComponent, 1)

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
	}

	template <class Archive>
	void EffekseerComponent::save(Archive&& archive, const uint32_t version) const
	{
		archive(
			cereal::base_class<ComponentBase>(this)
		);

		if (version == 1)
		{
			archive(
				CEREAL_NVP(effect_id)
			);
		}
	}

	EffekseerComponent::EffekseerComponent(ecs::Registry* registry, const mapping::rename_type::Entity ent)
		:ComponentBase{ registry,ent }
	{
		auto* effekseer_manager = GetRegistry()->GetScene()->GetSceneManager()->GetEffekseerManager();

		char16_t utf16_filename[256];
		Effekseer::ConvertUtf8ToUtf16(utf16_filename, 256, "./Data/Assets/Effekseer/Homing_Laser01/Homing_Laser01.efk");

		//effect_ = Effekseer::Effect::Create(const_cast<Effekseer::Manager*>(effekseer_manager->GetManager()), static_cast<EFK_CHAR*>(utf16_filename));
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
		//if(!effect_)
		//{
		//	char16_t utf16_filename[256];
		//	Effekseer::ConvertUtf8ToUtf16(utf16_filename, 256, "./Data/Assets/Effekseer/magic_circle/magic_circle3.efk");
		//	effect_ = Effekseer::Effect::Create(const_cast<Effekseer::Manager*>(manager), static_cast<EFK_CHAR*>(utf16_filename));
		//}

		if (!effect)
			return;

		handle = const_cast<Effekseer::Manager*>(manager)->Play(effect->GetEffect(), { 0, 0.01f, 0 });
	}

	void EffekseerComponent::ChangeEffect(const mapping::rename_type::UUID& efk_id)
	{
		if (efk_id.empty())
			return;

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

} // cumulonimbus::component