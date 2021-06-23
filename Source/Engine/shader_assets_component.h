#pragma once
#include <unordered_map>
#include <memory>

#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>

#include "component_base.h"
#include "shader_asset.h"
#include "shader_asset_mapping.h"

namespace cumulonimbus::component
{
	class ShaderAssets3DComponent final : public ComponentBase
	{
	public:
		explicit ShaderAssets3DComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit ShaderAssets3DComponent()  = default; // for cereal
		~ShaderAssets3DComponent() override = default;

		void NewFrame(float dt) override {}
		void Update(float dt)	override {}
		void RenderImGui()		override;

		void BindCBuffer(mapping::shader_assets::ShaderAsset3D asset);
		void UnbindCBuffer(mapping::shader_assets::ShaderAsset3D asset);

		void RenderImGui(mapping::shader_assets::ShaderAsset3D asset);

		void Load(const std::string& file_path_and_name) override {}
		void Save(const std::string& file_path)			 override {}
	private:
		std::unordered_map<mapping::shader_assets::ShaderAsset3D, std::shared_ptr<shader_asset::ShaderAsset>> shader_asset_map;

		template<typename T>
		void RegisterShaderAsset(mapping::shader_assets::ShaderAsset3D asset_type)
		{
			if (shader_asset_map.contains(asset_type))
				return;

			shader_asset_map.emplace(asset_type, std::make_shared<T>());
		}
	};
}

CEREAL_REGISTER_TYPE(cumulonimbus::component::ShaderAssets3DComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::ShaderAssets3DComponent)
