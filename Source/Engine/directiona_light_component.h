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
		 * @brief : �R���X�^���g�o�b�t�@�̃o�C���h
		 */
		void BindCBuffer() override;

		/**
		 * @brief : �R���X�^���g�o�b�t�@�̈ăo���h
		 */
		void UnbindCBuffer() override;

		/**
		 * @brief : ���C�����C�g�̐ݒ�
		 * @remark : ���R���|�[�l���g�ȊO��DirectionalLightComponent
		 *			 ��is_main_light��false�ɂ���
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