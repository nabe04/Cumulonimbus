#include "directiona_light_component.h"

#include "locator.h"
#include "scene.h"
#include "scene_manager.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::DirectionalLightComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::LightComponent, cumulonimbus::component::DirectionalLightComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::DirectionalLightComponent, 0)

namespace cumulonimbus::component
{
	template <class Archive>
	void DirectionalLightComponent::load(Archive&& archive, uint32_t version)
	{
		archive(
			cereal::base_class<LightComponent>(this)
		);
	}

	template <class Archive>
	void DirectionalLightComponent::save(Archive&& archive, uint32_t version) const
	{
		archive(
			cereal::base_class<LightComponent>(this)
		);
	}

	DirectionalLightComponent::DirectionalLightComponent(
		ecs::Registry* registry,
		const mapping::rename_type::Entity ent)
		:LightComponent{ registry,ent }
	{
		Initialize(registry, ent);
	}

	DirectionalLightComponent::DirectionalLightComponent(
		ecs::Registry* registry,
		const mapping::rename_type::Entity ent,
		const DirectionalLightComponent& copy_comp)
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
		Initialize(registry, ent);
	}

	void DirectionalLightComponent::Initialize(
		ecs::Registry* registry,
		mapping::rename_type::Entity ent)
	{
		// コンスタントバッファの作成
		cb_directional_light.CreateCBuffer(locator::Locator::GetDx11Device()->device.Get());
		// メインのライトにする
		SwitchMainLight();
	}

	void DirectionalLightComponent::CommonUpdate(const float dt)
	{
		LightComponent::CommonUpdate(dt);
	}

	void DirectionalLightComponent::PostCommonUpdate(const float dt)
	{
		directional_light.d_light_direction = light_direction;
		directional_light.d_light_intensity = light_intensity;
		directional_light.d_light_color		= light_color;
	}

	void DirectionalLightComponent::RenderImGui()
	{
		if (GetRegistry()->CollapsingHeader<DirectionalLightComponent>(GetEntity(), "Directional Light"))
		{
			if(ImGui::Button("Is Main Light"))
			{
				SwitchMainLight();
			}

			ImChangeParameter();
		}
	}

	void DirectionalLightComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);
	}

	void DirectionalLightComponent::BindCBuffer()
	{
		cb_directional_light.Activate(locator::Locator::GetDx11Device()->immediate_context.Get(), CBSlot_DirectionalLight, false);
	}

	void DirectionalLightComponent::UnbindCBuffer()
	{
		cb_directional_light.Deactivate(locator::Locator::GetDx11Device()->immediate_context.Get());
	}

	void DirectionalLightComponent::SwitchMainLight()
	{
		is_main_light = true;

		// 全てのシーン分処理を回す
		for (auto& scene : GetRegistry()->GetScene()->GetSceneManager()->GetActiveScenes() | std::views::values)
		{
			// 全てのDirectionalLightComponent分処理を回す
			for (auto& registry = *scene->GetRegistry();
				 auto& directional_light : registry.GetArray<DirectionalLightComponent>().GetComponents())
			{
				if (GetEntity() == directional_light.GetEntity())
					continue;

				// 自分のエンティティ以外のフラグをfalseにする
				directional_light.is_main_light = false;
			}
		}
	}

} /// cumulonimbus::component
