#pragma once
#include "component_base.h"

namespace cumulonimbus::component
{
	class LightComponent : public ComponentBase
	{
	public:
		using ComponentBase::ComponentBase;
		explicit LightComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit LightComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const LightComponent& copy_comp); // for prefab
		explicit LightComponent() = default; // for cereal
		~LightComponent() override = default;

		template<class Archive>
		void load(Archive&& archive, uint32_t version);

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;

		void CommonUpdate(float dt) override;
		void RenderImGui() override;

		void Load(ecs::Registry* registry) override;

		/**
		 * @brief : コンスタントバッファのバインド
		 */
		virtual void BindCBuffer(){};

		/**
		 * @brief : コンスタントバッファのアンバインド
		 */
		virtual void UnbindCBuffer(){};

	protected:
		// ライトの向き
		DirectX::SimpleMath::Vector3 light_direction{};
		// ライトの輝度
		float light_intensity{1.f};
		// ライトの色
		DirectX::SimpleMath::Vector4 light_color{};

		/**
		 * @brief : ImGui上でのパラメータ変更
		 */
		void ImChangeParameter();
	};
} // cumulonimbus::component