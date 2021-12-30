#pragma once
#include "light_component.h"

#include "constant_buffer.h"
#include "cbuffer_lights.h"

namespace cumulonimbus::component
{
	class DirectionalLightComponent final : public LightComponent
	{
	public:
		using LightComponent::LightComponent;
		explicit DirectionalLightComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit DirectionalLightComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const DirectionalLightComponent& copy_comp); // for prefab
		explicit DirectionalLightComponent()  = default; // for cereal
		~DirectionalLightComponent() override = default;

		template<class Archive>
		void load(Archive&& archive, uint32_t version);

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;

		void Initialize(ecs::Registry* registry, mapping::rename_type::Entity ent) override;

		void CommonUpdate(float dt) override;
		void PostCommonUpdate(float dt) override;

		void RenderImGui() override;

		void Load(ecs::Registry* registry) override;

		/**
		 * @brief : コンスタントバッファのバインド
		 */
		void BindCBuffer() override;

		/**
		 * @brief : コンスタントバッファの案バンド
		 */
		void UnbindCBuffer() override;

		/**
		 * @brief : メインライトの設定
		 * @remark : 現コンポーネント以外のDirectionalLightComponent
		 *			 のis_main_lightをfalseにする
		 */
		void SwitchMainLight();

		[[nodiscard]]
		const DirectionalLightCB& GetDirectionalLight() const
		{
			return directional_light;
		}

		[[nodiscard]]
		bool GetIsMainLight() const
		{
			return is_main_light;
		}
	private:
		buffer::ConstantBuffer<DirectionalLightCB> cb_directional_light{};
		DirectionalLightCB directional_light{};
		bool is_main_light{ false };
	};
} // cumulonimbus::component