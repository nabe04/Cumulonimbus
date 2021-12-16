#include "effekseer_component.h"

#include "scene.h"
#include "scene_manager.h"
#include "effekseer_manager.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::EffekseerComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::EffekseerComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::EffekseerComponent, 0)

namespace cumulonimbus::component
{
	template <class Archive>
	void EffekseerComponent::load(Archive&& archive, uint32_t version)
	{
		archive(
			cereal::base_class<ComponentBase>(this)
		);
	}

	template <class Archive>
	void EffekseerComponent::save(Archive&& archive, uint32_t version) const
	{
		archive(
			cereal::base_class<ComponentBase>(this)
		);
	}

	EffekseerComponent::EffekseerComponent(ecs::Registry* registry, const mapping::rename_type::Entity ent)
		:ComponentBase{ registry,ent }
	{
		auto* effekseer_manager = GetRegistry()->GetScene()->GetSceneManager()->GetEffekseerManager();

		char16_t utf16_filename[256];
		Effekseer::ConvertUtf8ToUtf16(utf16_filename, 256, "./Data/Assets/Effekseer/Homing_Laser01/Homing_Laser01.efk");

		effect = Effekseer::Effect::Create(const_cast<Effekseer::Manager*>(effekseer_manager->GetManager()), static_cast<EFK_CHAR*>(utf16_filename));
	}

	EffekseerComponent::EffekseerComponent(
		ecs::Registry* registry,
		const mapping::rename_type::Entity ent,
		const EffekseerComponent& copy_comp)
		: ComponentBase{ registry,ent }
	{
		auto* effekseer_manager = GetRegistry()->GetScene()->GetSceneManager()->GetEffekseerManager();

		char16_t utf16_filename[256];
		Effekseer::ConvertUtf8ToUtf16(utf16_filename, 256, "./Data/Assets/Effekseer/Homing_Laser01/Homing_Laser01.efk");

		effect = Effekseer::Effect::Create(const_cast<Effekseer::Manager*>(effekseer_manager->GetManager()), static_cast<EFK_CHAR*>(utf16_filename));
	}

	EffekseerComponent::EffekseerComponent(const mapping::component_tag::ComponentTag tag)
		: ComponentBase{ tag }
	{

	}

	EffekseerComponent::~EffekseerComponent()
	{
		ES_SAFE_RELEASE(effect);
	}

	void EffekseerComponent::RenderImGui()
	{
		if (GetRegistry()->CollapsingHeader<EffekseerComponent>(GetEntity(), "Effekseer"))
		{
			if(ImGui::Button("Play"))
			{
				Play();
			}
		}
	}

	void EffekseerComponent::Load(ecs::Registry* registry)
	{
	}

	void EffekseerComponent::Play()
	{
		auto* effekseer_manager = GetRegistry()->GetScene()->GetSceneManager()->GetEffekseerManager();
		const Effekseer::Manager* manager = effekseer_manager->GetManager();
		if(!effect)
		{
			char16_t utf16_filename[256];
			Effekseer::ConvertUtf8ToUtf16(utf16_filename, 256, "./Data/Assets/Effekseer/magic_circle/magic_circle3.efk");
			effect = Effekseer::Effect::Create(const_cast<Effekseer::Manager*>(manager), static_cast<EFK_CHAR*>(utf16_filename));
		}

		handle = const_cast<Effekseer::Manager*>(manager)->Play(effect, { 0, 0.01f, 0 });

	}
} // cumulonimbus::component