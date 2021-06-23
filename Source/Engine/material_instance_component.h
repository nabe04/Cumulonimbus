#pragma once
#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>

#include "component_base.h"
#include "enum_state_map.h"
#include "shader_asset_mapping.h"

namespace cumulonimbus::component
{
	class MaterialInstance3DComponent : public ComponentBase
	{
	public:
		MaterialInstance3DComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		MaterialInstance3DComponent() = default; // for cereal

		void NewFrame(float dt) override {}
		void Update(float dt)   override {}
		void RenderImGui()      override;

		void Save(const std::string& file_path)			 override;
		void Load(const std::string& file_path_and_name) override;

	private:
		EnumStateMap<mapping::shader_assets::ShaderAsset3D> shader_assets;

		// for ImGui
		int current_asset;
		std::vector<std::string > s_name;
		std::vector<const char*> asset_items{};
		/*
		 * brief : shader_assets‚Ìstd::string‚Ìƒf[ƒ^‚ğ
		 *         asset_items‚Ìconst char*‚É•ÏŠ·‚µ‚Ä’Ç‰Á
		 */
		void RegisterItem();
	};
}

CEREAL_REGISTER_TYPE(cumulonimbus::component::MaterialInstance3DComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::MaterialInstance3DComponent)
